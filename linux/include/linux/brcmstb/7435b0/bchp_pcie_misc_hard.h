/***************************************************************************
 *     Copyright (c) 1999-2012, Broadcom Corporation
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
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Jul 27 03:16:52 2012
 *                 MD5 Checksum         d41d8cd98f00b204e9800998ecf8427e
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: $
 *
 ***************************************************************************/

#ifndef BCHP_PCIE_MISC_HARD_H__
#define BCHP_PCIE_MISC_HARD_H__

/***************************************************************************
 *PCIE_MISC_HARD - PCI-E Miscellaneous Registers (Hard reset)
 ***************************************************************************/
#define BCHP_PCIE_MISC_HARD_ECO_CTRL_HARD        0x00414200 /* ECO Hard Reset Control Register */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG      0x00414204 /* PCIE Hard Debug Register */

/***************************************************************************
 *ECO_CTRL_HARD - ECO Hard Reset Control Register
 ***************************************************************************/
/* PCIE_MISC_HARD :: ECO_CTRL_HARD :: reserved0 [31:16] */
#define BCHP_PCIE_MISC_HARD_ECO_CTRL_HARD_reserved0_MASK           0xffff0000
#define BCHP_PCIE_MISC_HARD_ECO_CTRL_HARD_reserved0_SHIFT          16

/* PCIE_MISC_HARD :: ECO_CTRL_HARD :: ECO_HARD [15:00] */
#define BCHP_PCIE_MISC_HARD_ECO_CTRL_HARD_ECO_HARD_MASK            0x0000ffff
#define BCHP_PCIE_MISC_HARD_ECO_CTRL_HARD_ECO_HARD_SHIFT           0
#define BCHP_PCIE_MISC_HARD_ECO_CTRL_HARD_ECO_HARD_DEFAULT         0x00000000

/***************************************************************************
 *PCIE_HARD_DEBUG - PCIE Hard Debug Register
 ***************************************************************************/
/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: reserved0 [31:24] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_reserved0_MASK         0xff000000
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_reserved0_SHIFT        24

/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: SERDES_IDDQ [23:23] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_SERDES_IDDQ_MASK       0x00800000
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_SERDES_IDDQ_SHIFT      23
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_SERDES_IDDQ_DEFAULT    0x00000000

/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: SERDES_RESET [22:22] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_SERDES_RESET_MASK      0x00400000
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_SERDES_RESET_SHIFT     22
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_SERDES_RESET_DEFAULT   0x00000000

/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: SERDES_MDIO_RESET [21:21] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_SERDES_MDIO_RESET_MASK 0x00200000
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_SERDES_MDIO_RESET_SHIFT 21
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_SERDES_MDIO_RESET_DEFAULT 0x00000000

/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: SERDES_TEST_MODE [20:20] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_SERDES_TEST_MODE_MASK  0x00100000
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_SERDES_TEST_MODE_SHIFT 20
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_SERDES_TEST_MODE_DEFAULT 0x00000000

/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: reserved1 [19:19] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_reserved1_MASK         0x00080000
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_reserved1_SHIFT        19

/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: REFCLK_OVERRIDE_OUT [18:18] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_REFCLK_OVERRIDE_OUT_MASK 0x00040000
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_REFCLK_OVERRIDE_OUT_SHIFT 18
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_REFCLK_OVERRIDE_OUT_DEFAULT 0x00000000

/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: REFCLK_OVERRIDE_IN [17:17] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_REFCLK_OVERRIDE_IN_MASK 0x00020000
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_REFCLK_OVERRIDE_IN_SHIFT 17
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_REFCLK_OVERRIDE_IN_DEFAULT 0x00000000

/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: REFCLK_OVERRIDE [16:16] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_REFCLK_OVERRIDE_MASK   0x00010000
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_REFCLK_OVERRIDE_SHIFT  16
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_REFCLK_OVERRIDE_DEFAULT 0x00000000

/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: PCIE_TMUX_SEL [15:08] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_PCIE_TMUX_SEL_MASK     0x0000ff00
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_PCIE_TMUX_SEL_SHIFT    8
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_PCIE_TMUX_SEL_DEFAULT  0x00000000

/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: WATCHDOG_PERST_CTRL [07:07] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_WATCHDOG_PERST_CTRL_MASK 0x00000080
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_WATCHDOG_PERST_CTRL_SHIFT 7
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_WATCHDOG_PERST_CTRL_DEFAULT 0x00000000

/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: LOCAL_PERST_CTRL [06:06] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_LOCAL_PERST_CTRL_MASK  0x00000040
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_LOCAL_PERST_CTRL_SHIFT 6
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_LOCAL_PERST_CTRL_DEFAULT 0x00000000

/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: LOCAL_BRIDGE_RESET_CTRL [05:05] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_LOCAL_BRIDGE_RESET_CTRL_MASK 0x00000020
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_LOCAL_BRIDGE_RESET_CTRL_SHIFT 5
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_LOCAL_BRIDGE_RESET_CTRL_DEFAULT 0x00000000

/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: AUXCLK_ENABLE [04:04] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_AUXCLK_ENABLE_MASK     0x00000010
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_AUXCLK_ENABLE_SHIFT    4
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_AUXCLK_ENABLE_DEFAULT  0x00000000

/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: DOWNSTREAM_PERST_DISABLE [03:03] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_DOWNSTREAM_PERST_DISABLE_MASK 0x00000008
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_DOWNSTREAM_PERST_DISABLE_SHIFT 3
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_DOWNSTREAM_PERST_DISABLE_DEFAULT 0x00000000

/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: PCIECORE_ADDR_MODE [02:02] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_PCIECORE_ADDR_MODE_MASK 0x00000004
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_PCIECORE_ADDR_MODE_SHIFT 2
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_PCIECORE_ADDR_MODE_DEFAULT 0x00000000

/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: CLKREQ_DEBUG_ENABLE [01:01] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_CLKREQ_DEBUG_ENABLE_MASK 0x00000002
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_CLKREQ_DEBUG_ENABLE_SHIFT 1
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_CLKREQ_DEBUG_ENABLE_DEFAULT 0x00000000

/* PCIE_MISC_HARD :: PCIE_HARD_DEBUG :: HOTRESET_DISABLE [00:00] */
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_HOTRESET_DISABLE_MASK  0x00000001
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_HOTRESET_DISABLE_SHIFT 0
#define BCHP_PCIE_MISC_HARD_PCIE_HARD_DEBUG_HOTRESET_DISABLE_DEFAULT 0x00000000

#endif /* #ifndef BCHP_PCIE_MISC_HARD_H__ */

/* End of File */
