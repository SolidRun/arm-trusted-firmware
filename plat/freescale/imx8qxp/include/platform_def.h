/*
 * Copyright (c) 2015, ARM Limited and Contributors. All rights reserved.
 * Copyright 2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#define PLATFORM_LINKER_FORMAT		"elf64-littleaarch64"
#define PLATFORM_LINKER_ARCH		aarch64

#define PLATFORM_STACK_SIZE		0x400
/* Fix Me! A35 */
#define CACHE_WRITEBACK_GRANULE		64

#define PLAT_PRIMARY_CPU		0x0
#define PLATFORM_MAX_CPU_PER_CLUSTER	4
#define PLATFORM_CLUSTER_COUNT		1
#define PLATFORM_CORE_COUNT		4

#define IMX_PWR_LVL0		MPIDR_AFFLVL0
#define IMX_PWR_LVL1		MPIDR_AFFLVL1

#define PWR_DOMAIN_AT_MAX_LVL		1
#define PLAT_MAX_PWR_LVL		2
#define PLAT_MAX_OFF_STATE		2
#define PLAT_MAX_RET_STATE		1

#define BL31_BASE			0x80000000
#define BL31_LIMIT			0x80020000

/* non-secure uboot base */
#define PLAT_NS_IMAGE_OFFSET		0x80020000

/* GICv3 base address */
#define PLAT_GICD_BASE			0x51a00000
#define PLAT_GICR_BASE			0x51b00000

#define PLAT_FSL_ADDR_SPACE_SIZE	(1ull << 32)
#define PLAT_VIRT_ADDR_SPACE_SIZE	(1ull << 32)
#define PLAT_PHY_ADDR_SPACE_SIZE	(1ull << 32)

#define MAX_XLAT_TABLES			4
#define MAX_MMAP_REGIONS		8

#define IMX_BOOT_UART_BASE		0x5a060000
#define IMX_BOOT_UART_BAUDRATE		115200
#define IMX_BOOT_UART_CLK_IN_HZ		24000000
#define PLAT_CRASH_UART_BASE		IMX_BOOT_UART_BASE
#define PLAT__CRASH_UART_CLK_IN_HZ	24000000
#define IMX_CONSOLE_BAUDRATE		115200

#define COUNTER_FREQUENCY		8000000 /* 8MHz */

#define DEBUG_CONSOLE			0
#define DEBUG_CONSOLE_A35		0
#define PLAT_IMX8QXP			1
