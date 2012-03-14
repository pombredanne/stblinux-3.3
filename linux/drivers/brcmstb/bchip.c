/*
 * Copyright (C) 2009 - 2012 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <linux/init.h>
#include <linux/types.h>
#include <linux/dma-mapping.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/bitops.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/compiler.h>

#include <linux/brcmstb/brcmstb.h>

#include "../drivers/mmc/host/sdhci.h"
#include "../drivers/mmc/host/sdhci-pltfm.h"

/* chip features */
int brcm_sata_enabled;
int brcm_pcie_enabled;
int brcm_moca_enabled;
int brcm_usb_enabled;
int brcm_pm_enabled;

/* synchronize writes to shared registers */
DEFINE_SPINLOCK(brcm_magnum_spinlock);
EXPORT_SYMBOL(brcm_magnum_spinlock);

/* system / device settings */
unsigned long brcm_dram0_size_mb;
unsigned long brcm_dram1_size_mb;
unsigned long brcm_dram1_linux_mb;
unsigned long brcm_dram1_start = MEMC1_START;
unsigned long brcm_min_auth_region_size = 0x1000;

unsigned char brcm_eth0_phy[CFE_STRING_SIZE];
unsigned long brcm_eth0_speed;
unsigned long brcm_eth0_no_mdio;

u8 brcm_primary_macaddr[IFHWADDRLEN] = { 0x00, 0x00, 0xde, 0xad, 0xbe, 0xef };

unsigned long brcm_base_baud0 = BRCM_BASE_BAUD_STB;	/* UPG UARTA */
unsigned long brcm_base_baud = BRCM_BASE_BAUD_STB;	/* UPG_UART[BC] */

struct bcmemac_platform_data genet_pdata[BRCM_MAX_GENET];

/***********************************************************************
 * Per-chip operations
 ***********************************************************************/

#define ALT_CHIP_ID(chip, rev) do { \
	u32 arg_id = 0x ## chip; \
	const u8 rev_name[] = #rev; \
	u32 arg_rev = ((rev_name[0] - 'a') << 4) | (rev_name[1] - '0'); \
	if (!kernel_chip_id && arg_id == chip_id) { \
		kernel_chip_id = arg_id; \
		kernel_chip_rev = arg_rev; \
	} \
	} while (0)

#define MAIN_CHIP_ID(chip, rev) do { \
	u32 arg_id = 0x ## chip; \
	const u8 rev_name[] = #rev; \
	u32 arg_rev = ((rev_name[0] - 'a') << 4) | (rev_name[1] - '0'); \
	if (!kernel_chip_id) { \
		kernel_chip_id = arg_id; \
		kernel_chip_rev = arg_rev; \
	} \
	} while (0)

/*
 * NOTE: This is a quick sanity test to catch known incompatibilities and
 * obvious chip ID mismatches.  It is not comprehensive.  Higher revs may
 * or may not maintain software compatibility.
 *
 * MAIN_CHIP_ID() must always be the final entry.
 */

void __init bchip_check_compat(void)
{
	u32 chip_id = BRCM_CHIP_ID(), chip_rev = BRCM_CHIP_REV();
	u32 kernel_chip_id = 0, kernel_chip_rev = 0;

#if defined(CONFIG_BCM7231)
	MAIN_CHIP_ID(7231, a0);
#elif defined(CONFIG_BCM7344)
	MAIN_CHIP_ID(7344, a0);
#elif defined(CONFIG_BCM7346)
	MAIN_CHIP_ID(7346, a0);
#elif defined(CONFIG_BCM7358)
	/* 7358 kernel can boot on 7552, but not vice-versa */
	ALT_CHIP_ID(7552, a0);
	MAIN_CHIP_ID(7358, a0);
#elif defined(CONFIG_BCM7552)
	MAIN_CHIP_ID(7552, a0);
#elif defined(CONFIG_BCM7425)
	MAIN_CHIP_ID(7425, a0);
#endif
	if (!kernel_chip_id)
		return;

	if (chip_id != kernel_chip_id)
		cfe_die("PANIC: BCM%04x kernel cannot boot on "
			"BCM%04x chip.\n", kernel_chip_id, chip_id);

	if (chip_rev < kernel_chip_rev)
		cfe_die("PANIC: This kernel requires BCM%04x rev >= %02X "
			"(P%02x)\n", kernel_chip_id,
			kernel_chip_rev + 0xa0, kernel_chip_rev + 0x10);
}

/***********************************************************************
 * Common operations for all chips
 ***********************************************************************/

#ifdef CONFIG_BRCM_HAS_SATA3

#ifdef CONFIG_CPU_BIG_ENDIAN
#define DATA_ENDIAN             2       /* AHCI->DDR inbound accesses */
#define MMIO_ENDIAN             2       /* MIPS->AHCI outbound accesses */
#else
#define DATA_ENDIAN             0
#define MMIO_ENDIAN             0
#endif /* CONFIG_CPU_BIG_ENDIAN */

static int sata3_enable_ssc;

#define SATA3_MDIO_TXPMD_0_REG_BANK	0x1A0
#define SATA3_MDIO_BRIDGE_BASE		(BCHP_SATA_GRB_REG_START + 0x100)
#define SATA3_MDIO_BASE_REG_ADDR	(SATA3_MDIO_BRIDGE_BASE + 0x8F * 4)

#define SATA_AHCI_GHC_PORTS_IMPLEMENTED	(BCHP_SATA_AHCI_GHC_REG_START + 0xC)

#define SATA3_TXPMD_CONTROL1			0x81
#define SATA3_TXPMD_TX_FREQ_CTRL_CONTROL1	0x82
#define SATA3_TXPMD_TX_FREQ_CTRL_CONTROL2	0x83
#define SATA3_TXPMD_TX_FREQ_CTRL_CONTROL3	0x84

static inline void brcm_sata3_mdio_wr_reg(u32 bank, unsigned int ofs, u32 msk,
		u32 enable)
{
	u32 tmp;
	BDEV_WR(SATA3_MDIO_BASE_REG_ADDR, bank);
	/* Read, mask, enable */
	tmp = BDEV_RD(ofs * 4 + SATA3_MDIO_BRIDGE_BASE);
	tmp = (tmp & msk) | enable;
	/* Write */
	BDEV_WR(ofs * 4 + SATA3_MDIO_BRIDGE_BASE, tmp);
}

static void brcm_sata3_init_freq(int port, int ssc_enable)
{
	u32 bank = SATA3_MDIO_TXPMD_0_REG_BANK + port * 0x10;

	/* TXPMD_control1 - enable SSC force */
	brcm_sata3_mdio_wr_reg(bank, SATA3_TXPMD_CONTROL1, 0xFFFFFFFC,
			0x00000003);

	/* TXPMD_tx_freq_ctrl_control2 - set fixed min freq */
	brcm_sata3_mdio_wr_reg(bank, SATA3_TXPMD_TX_FREQ_CTRL_CONTROL2,
			0xFFFFFC00, 0x000003DF);

	/*
	 * TXPMD_tx_freq_ctrl_control3 - set fixed max freq
	 *  If ssc_enable == 0, center frequencies
	 *  Otherwise, spread spectrum frequencies
	 */
	if (ssc_enable)
		brcm_sata3_mdio_wr_reg(bank, SATA3_TXPMD_TX_FREQ_CTRL_CONTROL3,
				0xFFFFFC00, 0x00000083);
	else
		brcm_sata3_mdio_wr_reg(bank, SATA3_TXPMD_TX_FREQ_CTRL_CONTROL3,
				0xFFFFFC00, 0x000003DF);
}

static int __init sata3_ssc_setup(char *str)
{
	sata3_enable_ssc = 1;
	return 0;
}

__setup("sata3_ssc", sata3_ssc_setup);

#endif /* CONFIG_BRCM_HAS_SATA3 */

void bchip_sata3_init(void)
{
#ifdef CONFIG_BRCM_HAS_SATA3
	int i, ports = fls(BDEV_RD(SATA_AHCI_GHC_PORTS_IMPLEMENTED));

	BDEV_WR(BCHP_SATA_TOP_CTRL_BUS_CTRL, (DATA_ENDIAN << 4) |
			(DATA_ENDIAN << 2) | (MMIO_ENDIAN << 0));

	for (i = 0; i < ports; i++)
		brcm_sata3_init_freq(i, sata3_enable_ssc);
#endif
}

#ifdef CONFIG_CPU_LITTLE_ENDIAN
#define USB_ENDIAN		0x03 /* !WABO !FNBO FNHW BABO */
#else
#define USB_ENDIAN		0x0e /* WABO FNBO FNHW !BABO */
#endif

#define USB_ENDIAN_MASK		0x0f
#define USB_IOC			BCHP_USB_CTRL_SETUP_IOC_MASK
#define USB_IPP			BCHP_USB_CTRL_SETUP_IPP_MASK

#define USB_REG(x, y)		(x + BCHP_USB_CTRL_##y - \
				 BCHP_USB_CTRL_REG_START)

static void bchip_usb_init_one(int id, uintptr_t base)
{
	/* endianness setup */
	BDEV_UNSET_RB(USB_REG(base, SETUP), USB_ENDIAN_MASK);
	BDEV_SET_RB(USB_REG(base, SETUP), USB_ENDIAN);

	/* power control setup */
#ifdef CONFIG_BRCM_OVERRIDE_USB

#ifdef CONFIG_BRCM_FORCE_USB_OC_LO
	BDEV_SET(USB_REG(base, SETUP), USB_IOC);
#else
	BDEV_UNSET(USB_REG(base, SETUP), USB_IOC);
#endif

#ifdef CONFIG_BRCM_FORCE_USB_PWR_LO
	BDEV_SET(USB_REG(base, SETUP), USB_IPP);
#else
	BDEV_UNSET(USB_REG(base, SETUP), USB_IPP);
#endif

#else /* CONFIG_BRCM_OVERRIDE_USB */
	if ((BDEV_RD(USB_REG(base, SETUP)) & USB_IOC) == 0) {
		printk(KERN_WARNING "USB%d: IOC was not set by the bootloader;"
			" forcing default settings\n", id);
		BDEV_SET(USB_REG(base, SETUP), USB_IOC);
		BDEV_SET(USB_REG(base, SETUP), USB_IPP);
	}
#endif /* CONFIG_BRCM_OVERRIDE_USB */

	printk(KERN_INFO "USB%d: power enable is active %s; overcurrent is "
		"active %s\n", id,
		BDEV_RD(USB_REG(base, SETUP)) & USB_IPP ? "low" : "high",
		BDEV_RD(USB_REG(base, SETUP)) & USB_IOC ? "low" : "high");

	/* PR45703 - for OHCI->SCB bridge lockup */
	BDEV_UNSET(USB_REG(base, OBRIDGE),
		BCHP_USB_CTRL_OBRIDGE_OBR_SEQ_EN_MASK);

	/* Disable EHCI transaction combining */
	BDEV_UNSET(USB_REG(base, EBRIDGE),
		BCHP_USB_CTRL_EBRIDGE_EBR_SEQ_EN_MASK);

	/* SWLINUX-1705: Avoid OUT packet underflows */
	BDEV_UNSET(USB_REG(base, EBRIDGE),
		BCHP_USB_CTRL_EBRIDGE_EBR_SCB_SIZE_MASK);
	BDEV_SET(USB_REG(base, EBRIDGE),
		0x08 << BCHP_USB_CTRL_EBRIDGE_EBR_SCB_SIZE_SHIFT);

#if defined(CONFIG_BRCM_HAS_1GB_MEMC1)
	/* enable access to SCB1 */
	BDEV_SET(USB_REG(base, SETUP), BIT(14));
#endif

#if defined(BCHP_USB_CTRL_GENERIC_CTL_1_PLL_SUSPEND_EN_MASK)
	BDEV_SET(USB_REG(base, GENERIC_CTL_1),
		BCHP_USB_CTRL_GENERIC_CTL_1_PLL_SUSPEND_EN_MASK);
#elif defined(BCHP_USB_CTRL_GENERIC_CTL_PLL_SUSPEND_EN_MASK)
	BDEV_SET(USB_REG(base, GENERIC_CTL),
		BCHP_USB_CTRL_GENERIC_CTL_PLL_SUSPEND_EN_MASK);
#elif defined(BCHP_USB_CTRL_PLL_CTL_1_PLL_SUSPEND_EN_MASK)
	BDEV_SET(USB_REG(base, PLL_CTL_1),
		BCHP_USB_CTRL_PLL_CTL_1_PLL_SUSPEND_EN_MASK);
#elif defined(BCHP_USB_CTRL_PLL_CTL_PLL_SUSPEND_EN_MASK)
	BDEV_SET(USB_REG(base, PLL_CTL),
		BCHP_USB_CTRL_PLL_CTL_PLL_SUSPEND_EN_MASK);
#endif

}

void bchip_usb_init(void)
{
	bchip_usb_init_one(0, BCHP_USB_CTRL_REG_START);
#ifdef BCHP_USB1_CTRL_REG_START
	bchip_usb_init_one(1, BCHP_USB1_CTRL_REG_START);
#endif
}

#if defined(CONFIG_BRCM_HAS_MOCA)
void bchip_moca_init(void)
{
#ifdef BCHP_SUN_TOP_CTRL_SW_RESET
	BDEV_WR_F_RB(SUN_TOP_CTRL_SW_RESET, moca_sw_reset, 0);
#else
	BDEV_WR_F_RB(SUN_TOP_CTRL_SW_INIT_0_CLEAR, moca_sw_init, 1);
#endif

#ifdef BCHP_MOCA_HOSTMISC_SW_RESET_moca_enet_reset_MASK
	BDEV_WR_F_RB(MOCA_HOSTMISC_SW_RESET, moca_enet_reset, 0);
#endif
}
#endif

#if defined(CONFIG_BRCM_SDIO)

/* Use custom I/O accessors to avoid readl/readw byte swapping in BE mode */

static u32 sdhci_brcm_readl(struct sdhci_host *host, int reg)
{
	return __raw_readl(host->ioaddr + reg);
}

static u16 sdhci_brcm_readw(struct sdhci_host *host, int reg)
{
	return __raw_readw(host->ioaddr + reg);
}

static void sdhci_brcm_writel(struct sdhci_host *host, u32 val, int reg)
{
	__raw_writel(val, host->ioaddr + reg);
}

static void sdhci_brcm_writew(struct sdhci_host *host, u16 val, int reg)
{
	__raw_writew(val, host->ioaddr + reg);
}

static struct sdhci_ops __maybe_unused sdhci_be_ops = {
	.read_l			= sdhci_brcm_readl,
	.read_w			= sdhci_brcm_readw,
	.write_l		= sdhci_brcm_writel,
	.write_w		= sdhci_brcm_writew,
};

struct sdhci_pltfm_data sdhci_brcm_pdata = { };

static int nommc;

static int __init nommc_setup(char *str)
{
	nommc = 1;
	return 0;
}

__setup("nommc", nommc_setup);

int __init bchip_sdio_init(int id, uintptr_t cfg_base)
{
#define SDIO_REG(x, y)		(x + BCHP_SDIO_0_CFG_##y - \
				 BCHP_SDIO_0_CFG_REG_START)

	if (nommc) {
		printk(KERN_INFO "SDIO_%d: disabled via command line\n", id);
		return -ENODEV;
	}
	if (BDEV_RD(SDIO_REG(cfg_base, SCRATCH)) & 0x01) {
		printk(KERN_INFO "SDIO_%d: disabled by bootloader\n", id);
		return -ENODEV;
	}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 37)
	printk(KERN_INFO "SDIO_%d: this core requires Linux 2.6.37 or higher; "
		"disabling\n", id);
	return -ENODEV;
#endif
	printk(KERN_INFO "SDIO_%d: enabling controller\n", id);

	BDEV_UNSET(SDIO_REG(cfg_base, SDIO_EMMC_CTRL1), 0xf000);
	BDEV_UNSET(SDIO_REG(cfg_base, SDIO_EMMC_CTRL2), 0x00ff);
#ifdef CONFIG_CPU_LITTLE_ENDIAN
	/* FRAME_NHW | BUFFER_ABO */
	BDEV_SET(SDIO_REG(cfg_base, SDIO_EMMC_CTRL1), 0x3000);
#else
	/* WORD_ABO | FRAME_NBO | FRAME_NHW */
	BDEV_SET(SDIO_REG(cfg_base, SDIO_EMMC_CTRL1), 0xe000);
	/* address swap only */
	BDEV_SET(SDIO_REG(cfg_base, SDIO_EMMC_CTRL2), 0x0050);
	sdhci_brcm_pdata.ops = &sdhci_be_ops;
#endif

#if defined(CONFIG_BCM7231B0) || defined(CONFIG_BCM7346B0)
	BDEV_SET(SDIO_REG(cfg_base, CAP_REG1), BIT(31));	/* Override=1 */
#endif

#if defined(CONFIG_BCM7344B0)
	BDEV_UNSET(SDIO_REG(cfg_base, CAP_REG0), BIT(19));	/* Highspd=0 */
	BDEV_SET(SDIO_REG(cfg_base, CAP_REG1), BIT(31));	/* Override=1 */
#endif

	return 0;
}
#endif /* defined(CONFIG_BRCM_SDIO) */

void __init bchip_set_features(void)
{
#if defined(CONFIG_BRCM_HAS_SATA)
	brcm_sata_enabled = 1;
#endif
#if defined(CONFIG_BRCM_HAS_PCIE)
	brcm_pcie_enabled = 1;
#endif
#if defined(CONFIG_SMP)
	bmips_smp_enabled = 1;
#endif
#if defined(CONFIG_BRCM_HAS_MOCA)
	brcm_moca_enabled = 1;
#endif
#if defined(CONFIG_BRCM_PM)
	brcm_pm_enabled = 1;
#endif
	brcm_usb_enabled = 1;

	/* now remove any features disabled in hardware */

#ifdef BCHP_SUN_TOP_CTRL_OTP_OPTION_STATUS_0_otp_option_sata_disable_MASK
	if (BDEV_RD_F(SUN_TOP_CTRL_OTP_OPTION_STATUS_0,
			otp_option_sata_disable) == 1)
		brcm_sata_enabled = 0;
#endif

#ifdef BCHP_SUN_TOP_CTRL_OTP_OPTION_STATUS_0_otp_option_usb_disable_MASK
	if (BDEV_RD_F(SUN_TOP_CTRL_OTP_OPTION_STATUS_0,
			otp_option_usb_disable) == 1)
		brcm_usb_enabled = 0;
#endif

#ifdef BCHP_SUN_TOP_CTRL_OTP_OPTION_STATUS_0_otp_option_moca_disable_MASK
	if (BDEV_RD_F(SUN_TOP_CTRL_OTP_OPTION_STATUS_0,
			otp_option_moca_disable) == 1)
		brcm_moca_enabled = 0;
#endif

#ifdef BCHP_SUN_TOP_CTRL_OTP_OPTION_STATUS_0_otp_option_pcie_disable_MASK
	if (BDEV_RD_F(SUN_TOP_CTRL_OTP_OPTION_STATUS_0,
			otp_option_pcie_disable) == 1)
		brcm_pcie_enabled = 0;
#endif

#ifdef CONFIG_BCM7425
	/* disable PCIe initialization in EP mode */
	if (BDEV_RD_F(SUN_TOP_CTRL_STRAP_VALUE_0, strap_rc_ep) == 0)
		brcm_pcie_enabled = 0;
#endif
}

void __init bchip_early_setup(void)
{
#if defined(CONFIG_BRCM_HAS_WKTMR)
	struct wktmr_time t;

	BDEV_WR_F_RB(WKTMR_EVENT, wktmr_alarm_event, 1);
	BDEV_WR_F_RB(WKTMR_PRESCALER, wktmr_prescaler, WKTMR_FREQ);
	BDEV_WR_F_RB(WKTMR_COUNTER, wktmr_counter, 0);

	/* wait for first tick so we know the counter is ready to use */
	wktmr_read(&t);
	while (wktmr_elapsed(&t) == 0)
		;
#endif

#ifdef CONFIG_PCI
	if (brcm_pcie_enabled)
		brcm_early_pcie_setup();
#endif

	/*
	 * Initial GENET defaults
	 * These can be overridden by board_pinmux_setup() or by CFE vars
	 */
#if defined(CONFIG_BRCM_HAS_GENET_0)

	genet_pdata[0].base_reg = BCHP_GENET_0_SYS_REG_START;
	genet_pdata[0].irq0 = BRCM_IRQ_GENET_0_A;
	genet_pdata[0].irq1 = BRCM_IRQ_GENET_0_B;

#if defined(CONFIG_BRCM_MOCA_ON_GENET_0)
	if (brcm_moca_enabled) {
		genet_pdata[0].phy_type = BRCM_PHY_TYPE_MOCA;
		genet_pdata[0].phy_id = BRCM_PHY_ID_NONE;
	} else {
		genet_pdata[0].phy_type = BRCM_PHY_TYPE_EXT_RGMII;
		genet_pdata[0].phy_id = BRCM_PHY_ID_AUTO;
	}
#else
	genet_pdata[0].phy_type = BRCM_PHY_TYPE_INT;
	genet_pdata[0].phy_id = 1;
#endif

#endif

#if defined(CONFIG_BRCM_HAS_GENET_1)

	genet_pdata[1].base_reg = BCHP_GENET_1_SYS_REG_START;
	genet_pdata[1].irq0 = BRCM_IRQ_GENET_1_A;
	genet_pdata[1].irq1 = BRCM_IRQ_GENET_1_B;

	genet_pdata[1].phy_type = BRCM_PHY_TYPE_EXT_RGMII;
	genet_pdata[1].phy_id = BRCM_PHY_ID_AUTO;

#if defined(CONFIG_BRCM_MOCA_ON_GENET_1)
	if (brcm_moca_enabled) {
		genet_pdata[1].phy_type = BRCM_PHY_TYPE_MOCA;
		genet_pdata[1].phy_id = BRCM_PHY_ID_NONE;
	}
#endif

#endif
}

int brcm_alloc_macaddr(u8 *buf)
{
	memcpy(buf, brcm_primary_macaddr, ETH_ALEN);
	brcm_primary_macaddr[4]++;
	return 0;
}
EXPORT_SYMBOL(brcm_alloc_macaddr);

/***********************************************************************
 * WKTMR utility functions (boot time only)
 ***********************************************************************/

void wktmr_read(struct wktmr_time *t)
{
	uint32_t tmp;

	do {
		t->sec = BDEV_RD(BCHP_WKTMR_COUNTER);
		tmp = BDEV_RD(BCHP_WKTMR_PRESCALER_VAL);
	} while (tmp >= WKTMR_FREQ);

	t->pre = WKTMR_FREQ - tmp;
}

unsigned long wktmr_elapsed(struct wktmr_time *t)
{
	struct wktmr_time now;

	wktmr_read(&now);
	now.sec -= t->sec;
	if (now.pre > t->pre) {
		now.pre -= t->pre;
	} else {
		now.pre = WKTMR_FREQ + now.pre - t->pre;
		now.sec--;
	}
	return (now.sec * WKTMR_FREQ) + now.pre;
}
