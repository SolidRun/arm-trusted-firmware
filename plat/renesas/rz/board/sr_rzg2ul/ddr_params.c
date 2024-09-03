// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2024 Josua Mayer <josua@solid-run.com>
 */

#include <lib/utils_def.h>
#include <stdint.h>
#include <string.h>

#include <cpg.h>
#include <common/debug.h>
#include <common/desc_image_load.h>
#include <ddr_internal.h>
#include <ddr_mc_regs.h>
#include <libfdt.h>
#include <pfc.h>
#include <plat/common/common_def.h>
#include <riic.h>
#include <riic_tlv.h>
#include <rz_private.h>

/*
 * Import T3bc C-011_D4-02-3 as baseline configurations.
 * Override const keyword to allow runtime patching ...
 */
#define const
#define ddr_an_version C_011_D4_02_3_ddr_an_version
#include "param_mc_C-011_D4-02-3.c"
#include "param_swizzle_T3bc.c"
#undef ddr_an_version
#undef const

/* declare ddr tool version string outside of parameters to reserve extra space */
char ddr_an_version[12] = {0};

/*
 * Define differential patch functions for each supported memory
 * configuration, relative to the baseline.
 */

/*
 * Patch a 2d array where first element is a register address and second element its value.
 * The patch and the baseline must both be sorted by register address.
 */
static inline void apply_regtable_patch(uint32_t base[MC_INIT_NUM][2], const uint32_t patch[][2], size_t patch_size)
{
	size_t i, j;

	/* for each patch entry update register value in base */
	for (i = 0, j = 0; i < MC_INIT_NUM  && j < patch_size; i++)
		if (base[i][0] == patch[j][0])
			base[i][1] = patch[j++][1];
}

static inline void apply_C_011_D4_02_3(void)
{
	memcpy(ddr_an_version, C_011_D4_02_3_ddr_an_version, strlen(C_011_D4_02_3_ddr_an_version) + 1);
}

/*
 * patch for C-011_D4-02-2 (based on C-011_D4-02-3)
 * diff -au ./plat/renesas/rz/soc/g2ul/drivers/ddr/param_mc_C-011_D4-02-3.c ./plat/renesas/rz/soc/g2ul/drivers/ddr/param_mc_C-011_D4-02-2.c
 */
static const uint32_t C_011_D4_02_2_mc_init_tbl[][2] = {
	{ DENALI_CTL_30, 0x00000118 },
	{ DENALI_CTL_34, 0x01200255 },
	{ DENALI_CTL_35, 0x00000120 },
	{ DENALI_CTL_122, 0x01010100 },
	{ DENALI_CTL_123, 0x00010101 },
	{ DENALI_CTL_124, 0x0FFF0000 },
	{ DENALI_CTL_125, 0x0001FF00 },
};

const char C_011_D4_02_2_ddr_an_version[] = "v3.0.1";

static inline void apply_C_011_D4_02_2(void)
{
	apply_regtable_patch(mc_init_tbl, C_011_D4_02_2_mc_init_tbl, ARRAY_SIZE(C_011_D4_02_2_mc_init_tbl));
	memcpy(ddr_an_version, C_011_D4_02_2_ddr_an_version, strlen(C_011_D4_02_2_ddr_an_version) + 1);
}

static inline void apply_T3bc(void)
{
}

#if (defined(BL33_ARG1_FDTBLOB) && BL33_ARG1_FDTBLOB) || (defined(BL33_ARG23_DRAM_INFO) && BL33_ARG23_DRAM_INFO)
static uint64_t dram_size;
#endif

void ddr_param_setup(void)
{
	bool status;
	char sku[32] = {0};
	uint16_t sku_len;

	/* Read SoM ID EEPROM on i2c0 at 0x50 */
	cpg_i2c_setup((1 << 0));
	pfc_i2c01_setup(1, 0);
	riic_setup(RZG2L_RIIC0_BASE);
	riic_flush(RZG2L_RIIC0_BASE);
	status = riic_tlv_read_eeprom(0x50, 0x00);
	pfc_i2c01_setup(0, 0);
	cpg_i2c_setup(0);
	if (!status) {
		ERROR("Failed to read SoM EEPROM on i2c0 @ 0x50!\n");
		goto mem_default;
	}

	/*
	 *find som part number, e.g.:
	 * - SRG2ULSW00D512E000V11C0
	 * - SRG2ULSW00D01GE064V12I0
	 */
	sku_len = riic_tlv_find(TLV_CODE_PART_NUMBER, NULL, NULL);
	if (!sku_len || sku_len >= sizeof(sku)) {
		ERROR("SoM SKU length on EEPROM is invalid: %d\n", sku_len);
		goto mem_default;
	}
	sku_len = riic_tlv_find(TLV_CODE_PART_NUMBER, (uint8_t *)sku, NULL);

	/* choose memory config by size digits [13:11] */
	if (!strncmp(&sku[11], "512", 3))
		goto mem_512m;
	if (!strncmp(&sku[11], "01G", 3))
		goto mem_1g;

	ERROR("EEPROM specifies unsupported memory size: '%c%c%c'\n", sku[11], sku[12], sku[13]);
	goto mem_default;

mem_default:
mem_512m:
#if (defined(BL33_ARG1_FDTBLOB) && BL33_ARG1_FDTBLOB) || (defined(BL33_ARG23_DRAM_INFO) && BL33_ARG23_DRAM_INFO)
	dram_size = SZ_512M;
#endif
	apply_T3bc();
	apply_C_011_D4_02_3();
	NOTICE("memory settings: %s %s\n", "T3bc", "C-011_D4-02-3");
	return;
mem_1g:
#if (defined(BL33_ARG1_FDTBLOB) && BL33_ARG1_FDTBLOB) || (defined(BL33_ARG23_DRAM_INFO) && BL33_ARG23_DRAM_INFO)
	dram_size = SZ_1G;
#endif
	apply_T3bc();
	apply_C_011_D4_02_2();
	NOTICE("memory settings: %s %s\n", "T3bc", "C-011_D4-02-2");
	return;
}

#if defined(BL33_ARG1_FDTBLOB) && BL33_ARG1_FDTBLOB
int bl2_fdtblob_setup_dram(void *fdt)
{
	int node;
	int ret = 0;

	ret = fdt_setprop_string(fdt, 0, "compatible", "solidrun,rzg2l-sr-som");
	if (ret)
		NOTICE("setprop failed: %d\n", ret);


	node = ret = fdt_add_subnode(fdt, 0, "memory@48000000");
	if (ret < 0)
		return ret;

	ret = fdt_setprop_string(fdt, node, "device_type", "memory");
	if (ret < 0)
		return ret;

	/* dram memory starts at 1G, but first 128MB are reserved */
	ret = fdt_setprop_u64(fdt, node, "reg", SZ_1G + SZ_128M);
	if (ret < 0)
		return ret;

	/* expose only non-reserved size */
	ret = fdt_appendprop_u64(fdt, node, "reg", dram_size - SZ_128M);
	if (ret < 0)
		return ret;

	return ret;
}
#endif

#if defined(BL33_ARG23_DRAM_INFO) && BL33_ARG23_DRAM_INFO
int bl3_params_setup(unsigned int image_id, bl2_to_bl31_params_mem_t *params)
{
	if (image_id == BL33_IMAGE_ID) {
		/* expose only non-reserved area */
		params->bl33_ep_info.args.arg2 = SZ_1G + SZ_128M;
		params->bl33_ep_info.args.arg3 = dram_size - SZ_128M;
	}

	return 0;
}
#endif
