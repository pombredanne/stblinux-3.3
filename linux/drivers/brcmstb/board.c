/*
 * Copyright (C) 2009 Broadcom Corporation
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
#include <linux/kernel.h>
#include <linux/smp.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/bitops.h>
#include <linux/bmoca.h>
#include <linux/mtd/partitions.h>
#include <linux/brcmstb/brcmstb.h>

/* board features */
int brcm_docsis_platform;
char brcm_cfe_boardname[CFE_STRING_SIZE];

/* MTD partition layout */
unsigned long brcm_mtd_rootfs_start;
unsigned long brcm_mtd_rootfs_len;
unsigned long brcm_mtd_kernel_start;
unsigned long brcm_mtd_kernel_len;
unsigned long brcm_mtd_ocap_start;
unsigned long brcm_mtd_ocap_len;
unsigned long brcm_mtd_flash_size_mb;
char brcm_mtd_flash_type[CFE_STRING_SIZE];

/* MoCA 3450 I2C port */
unsigned long brcm_moca_i2c_base;

/* Default MoCA RF band (can be overridden by CFE_BOARDNAME in prom.c) */
#ifdef CONFIG_BRCM_HAS_MOCA_MIDRF
unsigned long brcm_moca_rf_band = MOCA_BAND_MIDRF;
#else
unsigned long brcm_moca_rf_band = MOCA_BAND_HIGHRF;
#endif

/***********************************************************************
 * PIN_MUX setup
 *
 * NOTE: This code assumes that the bootloader already set up the pinmux
 * for the primary UART (almost always UARTA) and EBI.
 *
 * This code is free to modify genet_pdata[].  For example, to set up an
 * external RGMII PHY on GENET_0, set up your pinmux / pad voltages /
 * pulldowns then add:
 *
 *   genet_pdata[0].phy_type = BRCM_PHY_TYPE_EXT_RGMII;
 *   genet_pdata[0].phy_id = BRCM_PHY_ID_AUTO;
 ***********************************************************************/

#define PINMUX(reg, field, val) do { \
	BDEV_WR(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_##reg, \
		(BDEV_RD(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_##reg) & \
		 ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_##reg##_##field##_MASK) | \
		((val) << \
		 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_##reg##_##field##_SHIFT)); \
	} while (0)

#define AON_PINMUX(reg, field, val) do { \
	BDEV_WR(BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_##reg, \
		(BDEV_RD(BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_##reg) & \
		 ~BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_##reg##_##field##_MASK) | \
		((val) << \
		 BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_##reg##_##field##_SHIFT)); \
	} while (0)

#define PADCTRL(reg, field, val) do { \
	BDEV_WR(BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_##reg, \
		(BDEV_RD(BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_##reg) & \
		 ~BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_##reg##_##field##_MASK) | \
		((val) << \
		 BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_##reg##_##field##_SHIFT)); \
	} while (0)

#define AON_PADCTRL(reg, field, val) do { \
	BDEV_WR(BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_##reg, \
		(BDEV_RD(BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_##reg) & \
		 ~BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_##reg##_##field##_MASK) | \
		((val) << \
		 BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_##reg##_##field##_SHIFT)); \
	} while (0)

void board_pinmux_setup(void)
{
#if !defined(CONFIG_BRCM_IKOS)
#if defined(CONFIG_BCM7231)

	PINMUX(11, gpio_94, 1);		/* UARTB TX */
	PINMUX(11, gpio_95, 1);		/* UARTB RX */

	if (BRCM_PROD_ID() == 0x7230) {
		/* 7230 is not the same ballout as 7231 */
		AON_PINMUX(0, aon_gpio_04, 6);	/* SDIO */
		AON_PINMUX(0, aon_gpio_05, 6);
		AON_PINMUX(1, aon_gpio_12, 5);
		AON_PINMUX(1, aon_gpio_13, 5);
		AON_PINMUX(2, aon_gpio_14, 5);
		AON_PINMUX(2, aon_gpio_15, 6);
		AON_PINMUX(2, aon_gpio_16, 6);
		AON_PINMUX(2, aon_gpio_17, 6);
		AON_PINMUX(2, aon_gpio_18, 6);
		AON_PINMUX(2, aon_gpio_19, 6);

		/* disable GPIO pulldowns */
		BDEV_WR_F_RB(AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0,
			aon_gpio_04_pad_ctrl, 0);
		BDEV_WR_F_RB(AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0,
			aon_gpio_05_pad_ctrl, 0);
		BDEV_WR_F_RB(AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1,
			aon_gpio_12_pad_ctrl, 0);
		BDEV_WR_F_RB(AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1,
			aon_gpio_13_pad_ctrl, 0);
		BDEV_WR_F_RB(AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1,
			aon_gpio_14_pad_ctrl, 0);
		BDEV_WR_F_RB(AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1,
			aon_gpio_15_pad_ctrl, 0);
		BDEV_WR_F_RB(AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1,
			aon_gpio_16_pad_ctrl, 0);
		BDEV_WR_F_RB(AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1,
			aon_gpio_17_pad_ctrl, 0);
		BDEV_WR_F_RB(AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1,
			aon_gpio_18_pad_ctrl, 0);
		BDEV_WR_F_RB(AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1,
			aon_gpio_19_pad_ctrl, 0);

		/* limit speed to 25MHz due to AON pad timing restrictions */
		BDEV_UNSET(BCHP_SDIO_0_CFG_CAP_REG0, 1 << 19);	/* Highspd=0 */
		BDEV_SET(BCHP_SDIO_0_CFG_CAP_REG1, 1 << 31);	/* Override=1 */
	} else {
		/* set RGMII lines to 2.5V */
		BDEV_WR_F(SUN_TOP_CTRL_GENERAL_CTRL_NO_SCAN_0,
			rgmii_0_pad_mode, 1);

		PINMUX(15, gpio_132, 1);	/* RGMII RX */
		PINMUX(15, gpio_133, 1);
		PINMUX(15, gpio_134, 1);
		PINMUX(16, gpio_139, 1);
		PINMUX(16, gpio_140, 1);
		PINMUX(16, gpio_141, 1);
		PINMUX(16, gpio_142, 1);

		PINMUX(16, gpio_138, 1);	/* RGMII TX */
		PINMUX(17, gpio_143, 1);
		PINMUX(17, gpio_144, 1);
		PINMUX(17, gpio_145, 1);
		PINMUX(17, gpio_146, 1);
		PINMUX(17, gpio_147, 1);

		PINMUX(17, gpio_149, 1);	/* RGMII MDIO */
		PINMUX(16, gpio_137, 1);

		/* no pulldown on RGMII lines */
		PADCTRL(8, gpio_132_pad_ctrl, 0);
		PADCTRL(8, gpio_133_pad_ctrl, 0);
		PADCTRL(8, gpio_134_pad_ctrl, 0);
		PADCTRL(8, gpio_137_pad_ctrl, 0);
		PADCTRL(8, gpio_138_pad_ctrl, 0);
		PADCTRL(9, gpio_139_pad_ctrl, 0);
		PADCTRL(9, gpio_140_pad_ctrl, 0);
		PADCTRL(9, gpio_141_pad_ctrl, 0);
		PADCTRL(9, gpio_142_pad_ctrl, 0);
		PADCTRL(9, gpio_143_pad_ctrl, 0);
		PADCTRL(9, gpio_144_pad_ctrl, 0);
		PADCTRL(9, gpio_145_pad_ctrl, 0);
		PADCTRL(9, gpio_146_pad_ctrl, 0);
		PADCTRL(9, gpio_147_pad_ctrl, 0);
		PADCTRL(9, gpio_149_pad_ctrl, 0);

		PINMUX(14, gpio_122, 1);	/* SDIO */
		PINMUX(14, gpio_123, 1);
		PINMUX(14, gpio_124, 1);
		PINMUX(14, gpio_125, 1);
		PINMUX(14, gpio_126, 1);
		PINMUX(15, gpio_127, 1);
		PINMUX(15, gpio_128, 1);
		PINMUX(15, gpio_129, 1);
		PINMUX(15, gpio_130, 1);
		PINMUX(15, gpio_131, 1);

		/* no pulldown on SDIO lines */
		PADCTRL(7, gpio_122_pad_ctrl, 0);
		PADCTRL(7, gpio_123_pad_ctrl, 0);
		PADCTRL(8, gpio_124_pad_ctrl, 0);
		PADCTRL(8, gpio_125_pad_ctrl, 0);
		PADCTRL(8, gpio_126_pad_ctrl, 0);
		PADCTRL(8, gpio_127_pad_ctrl, 0);
		PADCTRL(8, gpio_128_pad_ctrl, 0);
		PADCTRL(8, gpio_129_pad_ctrl, 0);
		PADCTRL(8, gpio_130_pad_ctrl, 0);
		PADCTRL(8, gpio_131_pad_ctrl, 0);
	}

#elif defined(CONFIG_BCM7344)

	PINMUX(15, gpio_79, 1);		/* MoCA link */
	PINMUX(15, gpio_80, 1);		/* MoCA activity */

	PINMUX(17, uart_rxdb, 0);	/* UARTB RX */
	PINMUX(17, uart_txdb, 0);	/* UARTB TX */

	PINMUX(0, gpio_68, 3);		/* SDIO */
	PINMUX(0, gpio_69, 3);
	PINMUX(0, gpio_70, 3);
	PINMUX(0, gpio_71, 2);
	PINMUX(0, gpio_72, 2);
	PINMUX(0, gpio_73, 2);
	PINMUX(0, gpio_74, 2);
	PINMUX(0, gpio_75, 2);
	PINMUX(1, gpio_76, 2);
	PINMUX(1, gpio_77, 2);

	/* enable pullup on SDIO lines */
	PADCTRL(0, gpio_68_pad_ctrl, 2);
	PADCTRL(0, gpio_69_pad_ctrl, 2);
	PADCTRL(0, gpio_70_pad_ctrl, 2);
	PADCTRL(0, gpio_71_pad_ctrl, 2);
	PADCTRL(0, gpio_72_pad_ctrl, 2);
	PADCTRL(0, gpio_73_pad_ctrl, 2);
	PADCTRL(0, gpio_74_pad_ctrl, 2);
	PADCTRL(0, gpio_75_pad_ctrl, 2);
	PADCTRL(0, gpio_76_pad_ctrl, 2);
	PADCTRL(0, gpio_77_pad_ctrl, 2);

	AON_PINMUX(0, aon_gpio_00, 3);	/* UARTC RX (NC) */
	AON_PINMUX(0, aon_gpio_01, 3);	/* UARTC TX (NC) */

	PINMUX(16, sgpio_04, 1);	/* MoCA I2C */
	PINMUX(16, sgpio_05, 1);
	brcm_moca_i2c_base = BPHYSADDR(BCHP_BSCD_REG_START);

#elif defined(CONFIG_BCM7346)

	PINMUX(15, gpio_068, 2);	/* MoCA link */
	PINMUX(16, gpio_069, 1);	/* MoCA activity */

	PINMUX(9, gpio_017, 1);		/* UARTB TX */
	PINMUX(9, gpio_018, 1);		/* UARTB RX */
	PINMUX(10, gpio_021, 1);	/* UARTC TX */
	PINMUX(10, gpio_022, 1);	/* UARTC RX */

	PINMUX(16, sgpio_02, 1);	/* MoCA I2C on BSCB */
	PINMUX(16, sgpio_03, 1);
	brcm_moca_i2c_base = BPHYSADDR(BCHP_BSCB_REG_START);

	/*
	 * To enable SDIO_LED (activity LED) on the BCM97346 reference boards:
	 * install R1127, remove R1120, uncomment this line, and don't use MoCA
	 */
	/* PINMUX(16, sgpio_02, 3); */

	PINMUX(17, vo_656_5, 2);	/* SDIO_PRES */
	PINMUX(17, vo_656_4, 1);	/* SDIO_PWR0 */
	PINMUX(17, vo_656_3, 2);	/* SDIO_DAT3 */
	PINMUX(17, vo_656_2, 2);	/* SDIO_DAT1 */
	PINMUX(17, vo_656_1, 2);	/* SDIO_DAT1 */
	PINMUX(17, vo_656_0, 2);	/* SDIO_DAT0 */

	PINMUX(18, vo_656_clk, 1);	/* SDIO_CLK */
	PINMUX(18, vo_656_7, 1);	/* SDIO_CMD */
	PINMUX(18, vo_656_6, 2);	/* SDIO_WPROT */

#elif defined(CONFIG_BCM7358) || defined(CONFIG_BCM7552)

	PINMUX(11, gpio_89, 1);		/* UARTB TX */
	PINMUX(11, gpio_90, 1);		/* UARTB RX */
	PINMUX(11, gpio_91, 1);		/* UARTC TX */
	PINMUX(11, gpio_92, 1);		/* UARTC RX */

	AON_PINMUX(1, aon_gpio_08, 6);	/* SDIO */
	AON_PINMUX(1, aon_gpio_12, 5);
	AON_PINMUX(1, aon_gpio_13, 5);
	AON_PINMUX(2, aon_gpio_14, 4);
	AON_PINMUX(2, aon_gpio_15, 5);
	AON_PINMUX(2, aon_gpio_16, 5);
	AON_PINMUX(2, aon_gpio_17, 5);
	AON_PINMUX(2, aon_gpio_18, 5);
	AON_PINMUX(2, aon_gpio_19, 5);
	AON_PINMUX(2, aon_gpio_20, 5);

	/* enable SDIO pullups */
	AON_PADCTRL(0, aon_gpio_08_pad_ctrl, 2);
	AON_PADCTRL(1, aon_gpio_12_pad_ctrl, 2);
	AON_PADCTRL(1, aon_gpio_13_pad_ctrl, 2);
	AON_PADCTRL(1, aon_gpio_14_pad_ctrl, 2);
	AON_PADCTRL(1, aon_gpio_15_pad_ctrl, 2);
	AON_PADCTRL(1, aon_gpio_16_pad_ctrl, 2);
	AON_PADCTRL(1, aon_gpio_17_pad_ctrl, 2);
	AON_PADCTRL(1, aon_gpio_18_pad_ctrl, 2);
	AON_PADCTRL(1, aon_gpio_19_pad_ctrl, 2);
	AON_PADCTRL(1, aon_gpio_20_pad_ctrl, 2);

#elif defined(CONFIG_BCM7425)

	/* Bootloader indicates the availability of SDIO_0 in SCRATCH reg */
	if ((BDEV_RD(BCHP_SDIO_0_CFG_SCRATCH) & 0x01) == 0) {
		PINMUX(14, gpio_072, 2);
		PINMUX(14, gpio_073, 2);
		PINMUX(14, gpio_074, 2);
		PINMUX(14, gpio_075, 2);
		PINMUX(15, gpio_076, 2);
		PINMUX(15, gpio_077, 2);
		PINMUX(15, gpio_078, 2);
		PINMUX(15, gpio_079, 2);
		PINMUX(15, gpio_080, 2);
		PINMUX(15, gpio_081, 2);

		/* enable internal pullups */
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_9,
			gpio_072_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
			gpio_073_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
			gpio_074_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
			gpio_075_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
			gpio_076_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
			gpio_077_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
			gpio_078_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
			gpio_079_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
			gpio_080_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_10,
			gpio_081_pad_ctrl, 2);

		/* always use 3.3V (SDIO0_LOW_V_SEL_N=1) */
		/* V00 boards or A0 parts */
		BDEV_UNSET(BCHP_GIO_AON_IODIR_LO, 1 << 4);
		BDEV_SET(BCHP_GIO_AON_DATA_LO, 1 << 4);
		/* V10 boards with B0 parts use SDIO0_VOLT signal */
		AON_PINMUX(2, gpio_100, 5);
	}

	PINMUX(18, sgpio_00, 1);	/* MoCA I2C */
	PINMUX(19, sgpio_01, 1);
#if defined(CONFIG_BCM7425B0)
	brcm_moca_i2c_base = BPHYSADDR(BCHP_BSCC_REG_START);
#else
	brcm_moca_i2c_base = BPHYSADDR(BCHP_BSCA_REG_START);
#endif

#elif defined(CONFIG_BCM7429)

	PINMUX(11, gpio_094, 1);	/* UARTB TX */
	PINMUX(11, gpio_095, 1);	/* UARTB RX */
	PINMUX(12, gpio_096, 1);	/* UARTC TX */
	PINMUX(12, gpio_097, 1);	/* UARTC RX */

	AON_PINMUX(1, aon_gpio_11, 7);	/* MoCA LEDs */
	AON_PINMUX(2, aon_gpio_14, 5);

	PINMUX(18, sgpio_00, 1);	/* MoCA I2C */
	PINMUX(18, sgpio_01, 1);
	brcm_moca_i2c_base = BPHYSADDR(BCHP_BSCD_REG_START);

	/* Bootloader indicates the availability of SDIO_0 in SCRATCH reg */
	if ((BDEV_RD(BCHP_SDIO_0_CFG_SCRATCH) & 0x01) == 0) {
		/*
		 * 7241 uses GPIO_092 for UART_TX0 instead of SDIO0_VCTL so
		 * leave it alone. We don't need SDIO0_VCTL because the board
		 * is 3.3V only and doesn't use it.
		 */
		if (BRCM_PROD_ID() != 0x7241)
			PINMUX(11, gpio_092, 5);
		PINMUX(14, gpio_122, 1);
		PINMUX(14, gpio_123, 1);
		PINMUX(14, gpio_124, 1);
		PINMUX(15, gpio_125, 1);
		PINMUX(15, gpio_126, 1);
		PINMUX(15, gpio_127, 1);
		PINMUX(15, gpio_128, 1);
		PINMUX(15, gpio_129, 1);
		PINMUX(15, gpio_131, 1);
		/*
		 * 7428a0 board uses GPIO_93 for SDIO0_PRES
		 * 7429a0 board uses GPIO_130 for SDIO0_PRES
		 */
		if (BRCM_PROD_ID() == 0x7428) {
			PINMUX(11, gpio_093, 5);
			BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_6,
						 gpio_093_pad_ctrl, 0);
		} else
			PINMUX(15, gpio_130, 1);

		/* enable internal pullups */
		if (BRCM_PROD_ID() != 0x7241)
			BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_6,
						 gpio_092_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_8,
			gpio_122_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_8,
			gpio_123_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_8,
			gpio_124_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_8,
			gpio_125_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_8,
			gpio_126_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_8,
			gpio_127_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_8,
			gpio_128_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_8,
			gpio_129_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_9,
			gpio_130_pad_ctrl, 2);
		BDEV_WR_F_RB(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_9,
			gpio_131_pad_ctrl, 2);
	}

#endif /* chip type */
#endif /* !defined(CONFIG_BRCM_IKOS) */
}

/***********************************************************************
 * FLASH configuration
 ***********************************************************************/

#if defined(CONFIG_BRCM_FIXED_MTD_PARTITIONS)

static struct mtd_partition fixed_partition_map[] = {
	{
		.name = "entire_device",
		.size = MTDPART_SIZ_FULL,
		.offset = 0x00000000
	},
};

/*
 * Use the partition map defined at compile time
 */
int __init board_get_partition_map(struct mtd_partition **p)
{
	*p = fixed_partition_map;
	return ARRAY_SIZE(fixed_partition_map);
}

#else /* defined(CONFIG_BRCM_FIXED_MTD_PARTITIONS) */

/*
 * Construct the partition map for the primary flash device, based on
 * CFE environment variables that were read from prom.c
 *
 * Note that this does NOT have all of the partitions that CFE recognizes
 * (e.g. macadr, nvram).  It only has the rootfs, entire_device, and
 * optionally the kernel image partition.
 */
int __init board_get_partition_map(struct mtd_partition **p)
{
	struct mtd_partition *ret;
	int nr_parts;

	if (brcm_mtd_rootfs_len == 0)
		return -ENODEV;

	nr_parts = 2;
	if (brcm_mtd_kernel_len != 0)
		nr_parts++;

	ret = kzalloc(nr_parts * sizeof(struct mtd_partition), GFP_KERNEL);
	if (!ret)
		return -ENOMEM;

	ret[0].offset = brcm_mtd_rootfs_start;
	ret[0].size = brcm_mtd_rootfs_len;
	ret[0].name = "rootfs";

	ret[1].offset = 0;
	ret[1].size = MTDPART_SIZ_FULL;
	ret[1].name = "entire_device";

	if (brcm_mtd_kernel_len != 0) {
		ret[2].offset = brcm_mtd_kernel_start;
		ret[2].size = brcm_mtd_kernel_len;
		ret[2].name = "kernel";
	}

	*p = ret;
	return nr_parts;
}
#endif /* defined(CONFIG_BRCM_FIXED_MTD_PARTITIONS) */

void brcm_get_ocap_info(struct brcm_ocap_info *info)
{
	info->ocap_part_start = brcm_mtd_ocap_start;
	info->ocap_part_len = brcm_mtd_ocap_len;
	info->docsis_platform = brcm_docsis_platform;
}
EXPORT_SYMBOL(brcm_get_ocap_info);
