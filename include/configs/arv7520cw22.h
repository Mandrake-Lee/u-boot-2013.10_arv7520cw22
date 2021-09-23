/*
 * Copyright (C) 2013 Luka Perkov <luka@openwrt.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_MACH_TYPE		"arv7520cw22"
#define CONFIG_IDENT_STRING	" "CONFIG_MACH_TYPE
#define CONFIG_BOARD_NAME	"Arcadyan ARV7520CW22"

/* Configure SoC */
#define CONFIG_LTQ_SUPPORT_UART			/* Enable ASC and UART */

#define CONFIG_LTQ_SUPPORT_ETHERNET		/* Enable ethernet */

#define CONFIG_LTQ_SUPPORT_NAND_FLASH		/* Have a MX30LF1G08AA 128Mb NAND flash */

#define CONFIG_LTQ_SUPPORT_SPL_NAND_FLASH	/* Build NAND flash SPL */
#define CONFIG_LTQ_SPL_COMP_LZO			/* Compress SPL with LZO */
#define CONFIG_LTQ_SPL_CONSOLE			/* Enable SPL console */
#define CONFIG_LTQ_SPL_DEBUG				/* Give some verbosity */

#define CONFIG_SYS_NAND_PAGE_COUNT	64
#define CONFIG_SYS_NAND_PAGE_SIZE	2048
#define CONFIG_SYS_NAND_OOBSIZE		64
#define CONFIG_SYS_NAND_BLOCK_SIZE	(128 * 1024)
#define CONFIG_SYS_NAND_BAD_BLOCK_POS	NAND_LARGE_BADBLOCK_POS
#define CONFIG_SYS_NAND_5_ADDR_CYCLE

/* SPL/TPL boot parameter. Might need some tuning depending on arch, compiler... */
#define CONFIG_SYS_NAND_U_BOOT_OFFS	0x8000
#define CONFIG_SYS_NAND_TPL_OFFS 		0x4000


#define CONFIG_SYS_BOOTM_LEN          0x1000000       /* 16 MB */

/* Environment */
#if defined(CONFIG_SYS_BOOT_NANDSPL)
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OVERWRITE
/* Because flash is so large, we will book 1Mb for the whole Uboot partition */
/* Environment will occupy last 25% (256kb*/
#define CONFIG_ENV_OFFSET		(3*256 * 1024)
#define CONFIG_ENV_SECT_SIZE		(1*256 * 1024)
#else
#define CONFIG_ENV_IS_NOWHERE
#endif

#define CONFIG_ENV_SIZE			(3* 8 * 1024)
#define CONFIG_LOADADDR			CONFIG_SYS_LOAD_ADDR

/* Console */
#define CONFIG_LTQ_ADVANCED_CONSOLE
#define CONFIG_BAUDRATE			115200
#define CONFIG_CONSOLE_ASC		1
#define CONFIG_CONSOLE_DEV		"ttyLTQ1"

/* Pull in default board configs for Lantiq XWAY VRX200 */
#include <asm/lantiq/config.h>
#include <asm/arch/config.h>

/* Pull in default OpenWrt configs for Lantiq SoC */
#include "openwrt-lantiq-common.h"

/* MTD devices */
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTD
#define CONFIG_CMD_MTDPARTS
#define CONFIG_CMD_UBI
#define CONFIG_RBTREE
/* Define the same partitions as set in the openwrt device tree */
#define CONFIG_MTDIDS_DEFAULT		"nand0=nand.0"
#define CONFIG_MTDPARTS_DEFAULT	"mtdparts=nand.0:768k(u-boot),256k(u-boot-env),10m(kernel),-(ubifs)"
#define CONFIG_ENV_MTD_SUPPORT	"mtdparts="CONFIG_MTDPARTS_DEFAULT"\0" \
							"mtdids="CONFIG_MTDIDS_DEFAULT"\0"
/* Other commands */
#define CONFIG_CMD_MISC
#define CONFIG_CMD_ECHO

#define CONFIG_ENV_UPDATE_UBOOT_NAND					\
	"update-uboot-nand=run load-uboot-nandspl-lzo write-uboot-nand\0"

#define CONFIG_EXTRA_ENV_SETTINGS	\
	CONFIG_ENV_LANTIQ_DEFAULTS	\
	CONFIG_ENV_UPDATE_UBOOT_NAND	\
	CONFIG_ENV_MTD_SUPPORT

#endif /* __CONFIG_H */
