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

#include <arch_helpers.h>
#include <assert.h>
#include <bl_common.h>
#include <console.h>
#include <context.h>
#include <context_mgmt.h>
#include <debug.h>
#include <mmio.h>
#include <platform.h>
#include <platform_def.h>
#include <plat_imx8.h>
#include <xlat_tables.h>
#include <soc.h>
#include <tzc380.h>
#include <imx_csu.h>
#include <imx_rdc.h>

/* linker defined symbols */
extern unsigned long __RO_START__;
extern unsigned long __RO_END__;
extern unsigned long __BL31_END__;

#if USE_COHERENT_MEM
extern unsigned long __COHERENT_RAM_START__;
extern unsigned long __COHERENT_RAM_END__;

#define BL31_COHERENT_RAM_BASE (unsigned long)(&__COHERENT_RAM_START__)
#define BL31_COHERENT_RAM_LIMIT (unsigned long)(&__COHERENT_RAM_END__)
#endif

#define BL31_RO_BASE	(unsigned long)(&__RO_START__)
#define BL31_RO_LIMIT	(unsigned long)(&__RO_END__)
#define BL31_END	(unsigned long)(&__BL31_END__)

#define CAAM_BASE       (0x30900000) /* HW address*/

#define JR0_BASE        (CAAM_BASE + 0x1000)

#define CAAM_JR0MID		(0x30900010)
#define CAAM_JR1MID		(0x30900018)
#define CAAM_JR2MID		(0x30900020)
#define CAAM_NS_MID		(0x1)

#define SM_P0_PERM      (JR0_BASE + 0xa04)
#define SM_P0_SMAG2     (JR0_BASE + 0xa08)
#define SM_P0_SMAG1     (JR0_BASE + 0xa0c)
#define SM_CMD          (JR0_BASE + 0xbe4)

/* secure memory command */
#define SMC_PAGE_SHIFT	16
#define SMC_PART_SHIFT	8

#define SMC_CMD_ALLOC_PAGE	0x01	/* allocate page to this partition */
#define SMC_CMD_DEALLOC_PART	0x03	/* deallocate partition */

static entry_point_info_t bl32_image_ep_info;
static entry_point_info_t bl33_image_ep_info;

/* get SPSR for BL33 entry */
static uint32_t get_spsr_for_bl33_entry(void)
{
	unsigned long el_status;
	unsigned long mode;
	uint32_t spsr;

	/* figure out what mode we enter the non-secure world */
	el_status = read_id_aa64pfr0_el1() >> ID_AA64PFR0_EL2_SHIFT;
	el_status &= ID_AA64PFR0_ELX_MASK;

	mode = (el_status) ? MODE_EL2 : MODE_EL1;

	spsr = SPSR_64(mode, MODE_SP_ELX, DISABLE_ALL_EXCEPTIONS);
	return spsr;
}

#define SCTR_BASE_ADDR	0x306c0000
#define CNTFID0_OFF	0x20
#define CNTFID1_OFF	0x24

#define SC_CNTCR_ENABLE         (1 << 0)
#define SC_CNTCR_HDBG           (1 << 1)
#define SC_CNTCR_FREQ0          (1 << 8)
#define SC_CNTCR_FREQ1          (1 << 9)

#define GPR_TZASC_EN		(1 << 0)
#define GPR_TZASC_EN_LOCK	(1 << 16)
unsigned int freq;

void system_counter_init(void)
{
	int val;

        /* Update with accurate clock frequency */
	freq = mmio_read_32(SCTR_BASE_ADDR + CNTFID0_OFF);

        val = mmio_read_32(SCTR_BASE_ADDR + CNTCR_OFF);
        val &= ~(SC_CNTCR_FREQ0 | SC_CNTCR_FREQ1);
        val |= SC_CNTCR_FREQ0 | SC_CNTCR_ENABLE | SC_CNTCR_HDBG;
        mmio_write_32(SCTR_BASE_ADDR + CNTCR_OFF, val);
}

void bl31_tzc380_setup(void)
{
	unsigned int val;

	val = mmio_read_32(IMX_IOMUX_GPR_BASE + 0x28);
	if ((val & GPR_TZASC_EN) != GPR_TZASC_EN)
		return;

	NOTICE("Configureing TZASC380\n");

	tzc380_init(IMX_TZASC_BASE);

	/*
	 * Need to substact offset 0x40000000 from CPU address when
	 * programming tzasc region for i.mx8mq.
	 */

	/* Enable 1G-5G S/NS RW */
	tzc380_configure_region(0, 0x00000000, TZC_ATTR_REGION_SIZE(TZC_REGION_SIZE_4G) | TZC_ATTR_REGION_EN_MASK | TZC_ATTR_SP_ALL);

	tzc380_dump_state();
}

void bl31_early_platform_setup(bl31_params_t *from_bl2,
				void *plat_params_from_bl2)
{
	uint32_t sm_cmd;
#if !defined (CSU_RDC_TEST)
	int i;
	/* enable CSU NS access permission */
	for (i = 0; i < 64; i++) {
		mmio_write_32(0x303e0000 + i * 4, 0xffffffff);
	}
#endif

	/* Dealloc part 0 and 2 with current DID */
	sm_cmd = (0 << SMC_PART_SHIFT | SMC_CMD_DEALLOC_PART);
	mmio_write_32(SM_CMD, sm_cmd);

	sm_cmd = (2 << SMC_PART_SHIFT | SMC_CMD_DEALLOC_PART);
	mmio_write_32(SM_CMD, sm_cmd);

	/* config CAAM JRaMID set MID to Cortex A */
	mmio_write_32(CAAM_JR0MID, CAAM_NS_MID);
	mmio_write_32(CAAM_JR1MID, CAAM_NS_MID);
	mmio_write_32(CAAM_JR2MID, CAAM_NS_MID);

	/* config the AIPSTZ1 */
	mmio_write_32(0x301f0000, 0x77777777);
	mmio_write_32(0x301f0004, 0x77777777);
	mmio_write_32(0x301f0040, 0x0);
	mmio_write_32(0x301f0044, 0x0);
	mmio_write_32(0x301f0048, 0x0);
	mmio_write_32(0x301f004c, 0x0);
	mmio_write_32(0x301f0050, 0x0);

	/* config the AIPSTZ2 */
	mmio_write_32(0x305f0000, 0x77777777);
	mmio_write_32(0x305f0004, 0x77777777);
	mmio_write_32(0x305f0040, 0x0);
	mmio_write_32(0x305f0044, 0x0);
	mmio_write_32(0x305f0048, 0x0);
	mmio_write_32(0x305f004c, 0x0);
	mmio_write_32(0x305f0050, 0x0);

	/* config the AIPSTZ3 */
	mmio_write_32(0x309f0000, 0x77777777);
	mmio_write_32(0x309f0004, 0x77777777);
	mmio_write_32(0x309f0040, 0x0);
	mmio_write_32(0x309f0044, 0x0);
	mmio_write_32(0x309f0048, 0x0);
	mmio_write_32(0x309f004c, 0x0);
	mmio_write_32(0x309f0050, 0x0);

	/* config the AIPSTZ4 */
	mmio_write_32(0x32df0000, 0x77777777);
	mmio_write_32(0x32df0004, 0x77777777);
	mmio_write_32(0x32df0040, 0x0);
	mmio_write_32(0x32df0044, 0x0);
	mmio_write_32(0x32df0048, 0x0);
	mmio_write_32(0x32df004c, 0x0);
	mmio_write_32(0x32df0050, 0x0);

#if DEBUG_CONSOLE
	console_init(IMX_BOOT_UART_BASE, IMX_BOOT_UART_CLK_IN_HZ,
		     IMX_CONSOLE_BAUDRATE);
#endif
	/* enable the system counter */
	system_counter_init();

	/* Alloc partition 0 writing SMPO and SMAGs */
	mmio_write_32(SM_P0_PERM, 0xff);
	mmio_write_32(SM_P0_SMAG2, 0xffffffff);
	mmio_write_32(SM_P0_SMAG1, 0xffffffff);

	/* Allocate page 0 and 1 to partition 0 with DID set */
	sm_cmd = (0 << SMC_PAGE_SHIFT
					| 0 << SMC_PART_SHIFT
					| SMC_CMD_ALLOC_PAGE);
	mmio_write_32(SM_CMD, sm_cmd);

	sm_cmd = (1 << SMC_PAGE_SHIFT
					| 0 << SMC_PART_SHIFT
					| SMC_CMD_ALLOC_PAGE);
	mmio_write_32(SM_CMD, sm_cmd);

	/*
	 * tell BL3-1 where the non-secure software image is located
	 * and the entry state information.
	 */
	bl33_image_ep_info.pc = PLAT_NS_IMAGE_OFFSET;
	bl33_image_ep_info.spsr = get_spsr_for_bl33_entry();
	SET_SECURITY_STATE(bl33_image_ep_info.h.attr, NON_SECURE);

#ifdef TEE_IMX8
	/* Populate entry point information for BL32 */
	SET_PARAM_HEAD(&bl32_image_ep_info, PARAM_EP, VERSION_1, 0);
	SET_SECURITY_STATE(bl32_image_ep_info.h.attr, SECURE);
	bl32_image_ep_info.pc = BL32_BASE;
	bl32_image_ep_info.spsr = 0;
	/* Pass TEE base and size to uboot */
	bl33_image_ep_info.args.arg1 = 0xFE000000;
	bl33_image_ep_info.args.arg2 = 0x2000000;
#endif
	bl31_tzc380_setup();

#if defined (CSU_RDC_TEST)
	csu_test();
	rdc_test();
#endif
}

void bl31_plat_arch_setup(void)
{
	/* add the mmap */
	mmap_add_region(0x900000, 0x900000, 0x20000,
			MT_MEMORY | MT_RW);
	mmap_add_region(0x100000, 0x100000, 0x10000,
			MT_MEMORY | MT_RW);

	mmap_add_region(0x180000, 0x180000, 0x8000,
			MT_MEMORY | MT_RW);

	mmap_add_region(0x40000000, 0x40000000, 0xc0000000,
			MT_MEMORY | MT_RW | MT_NS);

	mmap_add_region(BL31_BASE, BL31_BASE, BL31_RO_LIMIT - BL31_RO_BASE,
			MT_MEMORY | MT_RO);
	mmap_add_region(IMX_ROM_BASE, IMX_ROM_BASE,
			0x20000, MT_MEMORY | MT_RO);
	/* Map GPV */
	mmap_add_region(0x32000000, 0x32000000, 0x800000, MT_DEVICE | MT_RW);
	/* Map AIPS */
	mmap_add_region(IMX_AIPS_BASE, IMX_AIPS_BASE, IMX_AIPS_SIZE, MT_DEVICE | MT_RW);

	/* Map GIC */
	mmap_add_region(0x38800000, 0x38800000, 0x200000,
			MT_DEVICE | MT_RW);

	/* Map DDRC/PHY/PERF */
	mmap_add_region(0x3c000000, 0x3c000000, 0xC000000, MT_DEVICE | MT_RW);

#if USE_COHERENT_MEM
	mmap_add_region(BL31_COHERENT_RAM_BASE, BL31_COHERENT_RAM_BASE,
		BL31_COHERENT_RAM_LIMIT - BL31_COHERENT_RAM_BASE,
		MT_DEVICE | MT_RW);
#endif

	/* setup xlat table */
	init_xlat_tables();

	/* enable the MMU */
	enable_mmu_el3(0);
}

void bl31_platform_setup(void)
{
	/* init the GICv3 cpu and distributor interface */
	plat_gic_driver_init();
	plat_gic_init();

	/* gpc init */
	imx_gpc_init();

	/* switch DDR frequency to 3200 mts */
	lpddr4_switch_to_3200();
}

entry_point_info_t *bl31_plat_get_next_image_ep_info(unsigned int type)
{
	if (type == NON_SECURE)
		return &bl33_image_ep_info;
	if (type == SECURE)
		return &bl32_image_ep_info;

	return NULL;
}

unsigned int plat_get_syscnt_freq2(void)
{
	return freq;
}

void bl31_plat_runtime_setup(void)
{
	return;
}
