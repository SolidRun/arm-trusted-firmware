/*
 * Copyright (c) 2024, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <arch_helpers.h>
#include <assert.h>
#include <common/bl_common.h>
#include <lib/xlat_tables/xlat_tables_compat.h>
#include <plat/common/common_def.h>

#include <scifa.h>
#include <plat_tzc_def.h>
#include <rz_private.h>
#include <rz_soc_def.h>
#include <pwrc.h>
#include <drivers/generic_delay_timer.h>
#include <syc.h>

static console_t rzv2n_bl31_console;
static bl2_to_bl31_params_mem_t from_bl2;

static void board_gpv_drp_setting(void);

#ifdef PLAT_EXTRA_LD_SCRIPT
IMPORT_SYM(uintptr_t, __BL31_PMUSRAM_START__, BL31_PMUSRAM_START);
IMPORT_SYM(uintptr_t, __BL31_PMUSRAM_END__, BL31_PMUSRAM_END);
IMPORT_SYM(uintptr_t, __BL31_PMUSRAM_BASE__, BL31_PMUSRAM_BASE);
#endif

void plat_copy_code_to_system_ram(void)
{
#ifdef PLAT_EXTRA_LD_SCRIPT
	int ret __attribute__ ((unused));
	uint32_t attr;
	const uintptr_t pmu_code_load = BL31_PMUSRAM_BASE;
	const uintptr_t pmu_code_image = BL31_PMUSRAM_START;
	size_t pmu_code_size = BL31_PMUSRAM_END - BL31_PMUSRAM_START;

	attr = MT_MEMORY | MT_RW | MT_SECURE | MT_EXECUTE_NEVER;
	ret = xlat_change_mem_attributes(pmu_code_image, pmu_code_size, attr);
	assert(ret == 0);

	memcpy((void *)pmu_code_image, (void *)pmu_code_load, pmu_code_size);
	flush_dcache_range(pmu_code_image, pmu_code_size);

	attr = MT_MEMORY | MT_RO | MT_SECURE | MT_EXECUTE;
	ret = xlat_change_mem_attributes(pmu_code_image, pmu_code_size, attr);
	assert(ret == 0);

	/* Invalidate instruction cache */
	plat_invalidate_icache();
	dsb();
	isb();
#endif /* PLAT_EXTRA_LD_SCRIPT && PLAT_SYSTEM_SUSPEND */
}

void bl31_early_platform_setup2(u_register_t arg0,
								u_register_t arg1,
								u_register_t arg2,
								u_register_t arg3)
{
	int ret;

	/* initialize console driver */
	ret = console_rz_register(
							RZV2N_SCIF_BASE,
							RZV2N_UART_INCK_HZ,
							RZV2N_UART_BAUDRATE,
							&rzv2n_bl31_console);
	if (!ret)
		panic();

	console_set_scope(&rzv2n_bl31_console,
			CONSOLE_FLAG_BOOT | CONSOLE_FLAG_RUNTIME | CONSOLE_FLAG_CRASH);

	syc_init(RZV2N_SYC_INCK_HZ);

		/* initialize Timer */
	generic_delay_timer_init();


	/* copy bl2_to_bl31_params_mem_t*/
	memcpy(&from_bl2, (void *)PARAMS_BASE, sizeof(from_bl2));
}

void bl31_plat_arch_setup(void)
{
	const mmap_region_t bl31_regions[] = {
		MAP_REGION_FLAT(BL31_START, BL31_END - BL31_START,
						MT_MEMORY | MT_RW | MT_SECURE),
		MAP_REGION_FLAT(BL_CODE_BASE, BL_CODE_END - BL_CODE_BASE,
						MT_CODE | MT_SECURE),
		MAP_REGION_FLAT(BL_RO_DATA_BASE, BL_RO_DATA_END - BL_RO_DATA_BASE,
						MT_RO_DATA | MT_SECURE),
		{0}
	};

	const mmap_region_t rzv2n_mmap[] = {
		MAP_REGION_FLAT(RZV2N_SRAM_BASE, RZV2N_SRAM_TOTAL_SIZE,
				MT_MEMORY | MT_RW | MT_SECURE),
		MAP_REGION_FLAT(RZV2N_DEVICE_BASE, RZV2N_DEVICE_SIZE,
				MT_DEVICE | MT_RW | MT_SECURE),
		MAP_REGION_FLAT(RZV2N_DDR_BASE, RZV2N_DDR_SIZE,
				MT_MEMORY | MT_RW | MT_SECURE),
		{0}
	};

	setup_page_tables(bl31_regions, rzv2n_mmap);
	enable_mmu_el3(0);
	plat_copy_code_to_system_ram();
}

void bl31_platform_setup(void)
{
	plat_security_setup();

	/* initialize GIC-600 */
	plat_gic_driver_init();
	plat_gic_init();

	pwrc_setup();

    board_gpv_drp_setting();
}

entry_point_info_t *bl31_plat_get_next_image_ep_info(uint32_t type)
{
	entry_point_info_t *next_image_info = NULL;

	next_image_info = (type == NON_SECURE)
			? &from_bl2.bl33_ep_info : &from_bl2.bl32_ep_info;

	if (next_image_info->pc)
		return next_image_info;
	else
		return NULL;
}

static void board_gpv_drp_setting(void)
{
//MAC_M*0
	(*(volatile uint32_t *)(0x1904410C)) = 0x00000063;
	(*(volatile uint32_t *)(0x19044110)) = 0x20002000;
	(*(volatile uint32_t *)(0x19044118)) = 0x00000000;
	(*(volatile uint32_t *)(0x1904411C)) = 0x00000001;
	(*(volatile uint32_t *)(0x19044120)) = 0x02000000;   // test point
	(*(volatile uint32_t *)(0x19044124)) = 0x00000000;
	(*(volatile uint32_t *)(0x19044128)) = 0x00000001;
	(*(volatile uint32_t *)(0x1904412C)) = 0x02000000;   // test point

//MAC_M*1
	(*(volatile uint32_t *)(0x1904510C)) = 0x00000063;
	(*(volatile uint32_t *)(0x19045110)) = 0x20002000;
	(*(volatile uint32_t *)(0x19045118)) = 0x00000000;
	(*(volatile uint32_t *)(0x1904511C)) = 0x00000001;
	(*(volatile uint32_t *)(0x19045120)) = 0x10000000;
	(*(volatile uint32_t *)(0x19045124)) = 0x00000000;
	(*(volatile uint32_t *)(0x19045128)) = 0x00000001;
	(*(volatile uint32_t *)(0x1904512C)) = 0x10000000;

//MACWT_M*0
	(*(volatile uint32_t *)(0x1904610C)) = 0x00000063;
	(*(volatile uint32_t *)(0x19046110)) = 0x20002000;
	(*(volatile uint32_t *)(0x19046118)) = 0x00000000;
	(*(volatile uint32_t *)(0x1904611C)) = 0x00000001;
	(*(volatile uint32_t *)(0x19046120)) = 0x10000000;
	(*(volatile uint32_t *)(0x19046124)) = 0x00000000;
	(*(volatile uint32_t *)(0x19046128)) = 0x00000001;
	(*(volatile uint32_t *)(0x1904612C)) = 0x10000000;

//MACWT_M*01
	(*(volatile uint32_t *)(0x1904710C)) = 0x00000063;
	(*(volatile uint32_t *)(0x19047110)) = 0x20002000;
	(*(volatile uint32_t *)(0x19047118)) = 0x00000000;
	(*(volatile uint32_t *)(0x1904711C)) = 0x00000001;
	(*(volatile uint32_t *)(0x19047120)) = 0x10000000;
	(*(volatile uint32_t *)(0x19047124)) = 0x00000000;
	(*(volatile uint32_t *)(0x19047128)) = 0x00000001;
	(*(volatile uint32_t *)(0x1904712C)) = 0x10000000;

//GPV ACPU
	(*(volatile uint32_t *)(0x14a44124)) = 0x00000000;
	(*(volatile uint32_t *)(0x14a44128)) = 0x00000001;
	(*(volatile uint32_t *)(0x14a4412c)) = 0x11000000;
	(*(volatile uint32_t *)(0x14a4410c)) = 0x00000002;

//GPV_VIDEO1
	(*(volatile uint32_t *)(0x1654210C)) = 0x00000018;
	(*(volatile uint32_t *)(0x16542130)) = 0x000A000A;
	(*(volatile uint32_t *)(0x16542134)) = 0x00000000;
	(*(volatile uint32_t *)(0x16542138)) = 0x06040604;

	(*(volatile uint32_t *)(0x1654310C)) = 0x00000018;
	(*(volatile uint32_t *)(0x16543130)) = 0x00640064;
	(*(volatile uint32_t *)(0x16543134)) = 0x00000202;
	(*(volatile uint32_t *)(0x16543138)) = 0x05040504;

	(*(volatile uint32_t *)(0x1654410C)) = 0x00000018;
	(*(volatile uint32_t *)(0x16544130)) = 0x00640064;
	(*(volatile uint32_t *)(0x16544134)) = 0x00000202;
	(*(volatile uint32_t *)(0x16544138)) = 0x05040504;

	(*(volatile uint32_t *)(0x1654510C)) = 0x00000018;
	(*(volatile uint32_t *)(0x16545130)) = 0x00640064;
	(*(volatile uint32_t *)(0x16545134)) = 0x00000202;
	(*(volatile uint32_t *)(0x16545138)) = 0x05040504;

//GPV_VIDEO0
	(*(volatile uint32_t *)(0x1614a124)) = 0x00000000;
	(*(volatile uint32_t *)(0x1614a128)) = 0x00000001;
	(*(volatile uint32_t *)(0x1614a12c)) = 0x04f00000;
	(*(volatile uint32_t *)(0x1614a10c)) = 0x00000002;
	return;
}
