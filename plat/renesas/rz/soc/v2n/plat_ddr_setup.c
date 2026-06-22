/*
 * Copyright (c) 2024, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <arch.h>
#include <arch_features.h>
#include <arch_helpers.h>
#include <common/bl_common.h>
#include <common/debug.h>
#include <drivers/auth/auth_mod.h>
#include <drivers/io/io_storage.h>
#include <lib/utils.h>
#include <lib/xlat_tables/xlat_tables_defs.h>
#include <plat/common/platform.h>
#include <sys.h>
#include <ddr.h>
#include <plat_tbbr_img_def.h>

uint32_t ddr_csr_table[RET_CSR_SIZE] __attribute__ ((aligned(8)));

/*
 * DDR calibration sanity test. Kept in-tree for bring-up debugging but
 * disabled by default now that DDR calibration is complete — it adds
 * tens of seconds to boot and floods the console.
 *
 * To re-enable, build BL2 with RZ_V2N_DDR_PROBE=1 on the make command
 * line (e.g. `make ... RZ_V2N_DDR_PROBE=1`).
 */
#ifndef RZ_V2N_DDR_PROBE
#define RZ_V2N_DDR_PROBE 0
#endif

#if RZ_V2N_DDR_PROBE
/*
 * BL2 DDR sanity test. Four phases, each running across the usable
 * range (0x48000000..0x140000000):
 *
 *   1. 16-MiB dense sweep — quick smoke, write-read at one offset per
 *      16 MiB. Catches gross addressing failures.
 *   2. Full-range Stuck-Address — write each cache line (64 B stride)
 *      with its own physical address, then read back and verify.
 *      Equivalent to memtester's "Stuck Address" test; flags row/column
 *      miswires and address-line aliasing.
 *   3. Full-range pattern stress — write 0xAAAA…/0x5555…/0xFFFF… across
 *      the full range, three iterations each before verify. Sustained
 *      back-to-back DDR traffic (~tens of seconds total). Exposes
 *      refresh / VREF / thermal marginality that single-shot tests miss.
 *   4. Walking-ones / walking-zeros at one mid-DDR cache line. Tests
 *      data-line integrity bit-by-bit.
 *
 * If the chip and PHY are clean, all four phases pass. A pass here that
 * still fails Linux-side memtester means the corruption is exposed only
 * by Linux access patterns (cache eviction, controller QoS) and is a
 * stronger PHY/QoS-tuning case for Renesas.
 */
static void ddr_probe_chip(void)
{
	const uintptr_t start = 0x48000000ULL;
	const uintptr_t end   = 0x140000000ULL;

	/* ---------- Phase 1: 16-MiB dense sweep ---------- */
	{
		const uintptr_t step = 0x01000000ULL;
		uint64_t errors = 0, samples = 0;
		uintptr_t first_err = 0;

		NOTICE("DDR probe phase 1: 16-MiB sweep 0x%lx..0x%lx\n",
			(unsigned long)start, (unsigned long)end);

		for (uintptr_t a = start; a < end; a += step) {
			volatile uint64_t *p = (volatile uint64_t *)a;
			uint64_t pat = 0xDEADBEEF00000000ULL | (uint64_t)a;
			uint64_t rb;

			*p = pat;
			__asm__ volatile ("dsb sy" ::: "memory");
			rb = *p;
			__asm__ volatile ("dsb sy" ::: "memory");

			samples++;
			if (rb != pat) {
				if (errors == 0)
					first_err = a;
				errors++;
				NOTICE("  MISMATCH @ 0x%lx wrote 0x%llx read 0x%llx\n",
					(unsigned long)a,
					(unsigned long long)pat,
					(unsigned long long)rb);
			}
		}
		NOTICE("DDR probe phase 1: %llu samples, %llu errors%s\n",
			(unsigned long long)samples,
			(unsigned long long)errors,
			errors ? "" : " — clean");
		if (errors)
			NOTICE("  first error at 0x%lx\n",
				(unsigned long)first_err);
	}

	/* ---------- Phase 2: full-range Stuck-Address @ 64 B stride ---------- */
	{
		const uintptr_t step = 64;	/* one entry per cache line */
		uint64_t errors = 0, scanned = 0;
		uintptr_t first_err = 0;

		NOTICE("DDR probe phase 2: stuck-address @64B across %llu MiB\n",
			(unsigned long long)((end - start) >> 20));

		for (uintptr_t a = start; a < end; a += step)
			*(volatile uint64_t *)a = (uint64_t)a;
		__asm__ volatile ("dsb sy" ::: "memory");

		for (uintptr_t a = start; a < end; a += step) {
			uint64_t rb = *(volatile uint64_t *)a;

			scanned++;
			if (rb != (uint64_t)a) {
				if (errors == 0)
					first_err = a;
				errors++;
				if (errors <= 4) {
					NOTICE("  MISMATCH @ 0x%lx expected 0x%lx read 0x%llx\n",
						(unsigned long)a,
						(unsigned long)a,
						(unsigned long long)rb);
				}
			}
		}
		NOTICE("DDR probe phase 2: %llu scanned, %llu errors%s\n",
			(unsigned long long)scanned,
			(unsigned long long)errors,
			errors ? "" : " — clean");
		if (errors)
			NOTICE("  first error at 0x%lx\n",
				(unsigned long)first_err);
	}

	/* ---------- Phase 3: full-range sustained pattern stress ---------- */
	{
		static const uint64_t patterns[] = {
			0xAAAAAAAAAAAAAAAAULL,
			0x5555555555555555ULL,
			0xFFFFFFFFFFFFFFFFULL,
		};
		const int sustain_iters = 3;	/* repeat write phase to keep DDR slammed */
		uint64_t total_errors = 0;

		NOTICE("DDR probe phase 3: pattern stress full range, "
			"3 patterns x %d sustained passes\n", sustain_iters);

		for (size_t k = 0; k < sizeof(patterns) / sizeof(patterns[0]); k++) {
			uint64_t pat = patterns[k];
			uint64_t serr = 0;

			for (int it = 0; it < sustain_iters; it++) {
				for (uintptr_t a = start; a < end;
				     a += sizeof(uint64_t))
					*(volatile uint64_t *)a = pat;
			}
			__asm__ volatile ("dsb sy" ::: "memory");

			for (uintptr_t a = start; a < end;
			     a += sizeof(uint64_t)) {
				if (*(volatile uint64_t *)a != pat) {
					if (serr < 4) {
						NOTICE("  MISMATCH @ 0x%lx pat=0x%llx\n",
							(unsigned long)a,
							(unsigned long long)pat);
					}
					serr++;
				}
			}
			NOTICE("  pattern 0x%llx: %llu errors%s\n",
				(unsigned long long)pat,
				(unsigned long long)serr,
				serr ? "" : " — clean");
			total_errors += serr;
		}
		NOTICE("DDR probe phase 3: %llu total errors%s\n",
			(unsigned long long)total_errors,
			total_errors ? "" : " — clean");
	}

	/* ---------- Phase 4: walking ones / zeros ---------- */
	{
		const uintptr_t base = 0xC0000000ULL;	/* mid-DDR */
		volatile uint64_t *p = (volatile uint64_t *)base;
		uint64_t errors = 0;

		NOTICE("DDR probe phase 4: walking ones/zeros at 0x%lx\n",
			(unsigned long)base);

		for (int b = 0; b < 64; b++) {
			uint64_t pat = 1ULL << b;
			uint64_t rb;

			*p = pat;
			__asm__ volatile ("dsb sy" ::: "memory");
			rb = *p;
			if (rb != pat) {
				NOTICE("  bit %d: wrote 0x%llx read 0x%llx\n",
					b, (unsigned long long)pat,
					(unsigned long long)rb);
				errors++;
			}

			pat = ~(1ULL << b);
			*p = pat;
			__asm__ volatile ("dsb sy" ::: "memory");
			rb = *p;
			if (rb != pat) {
				NOTICE("  ~bit %d: wrote 0x%llx read 0x%llx\n",
					b, (unsigned long long)pat,
					(unsigned long long)rb);
				errors++;
			}
		}
		NOTICE("DDR probe phase 4: %llu errors%s\n",
			(unsigned long long)errors,
			errors ? "" : " — clean");
	}

	NOTICE("DDR probe: all phases complete\n");
}
#else  /* !RZ_V2N_DDR_PROBE */
static inline void ddr_probe_chip(void) { }
#endif /* RZ_V2N_DDR_PROBE */

/*
 * Default DDR size hook. Boards that can detect the population at runtime
 * (e.g. v2n_sr_som via TLV EEPROM) override this.
 */
enum ddr_size __attribute__((weak)) board_get_ddr_size(void)
{
	return DDR_8GB;
}

static uint64_t plat_ddr_total_bytes;

static uint64_t ddr_code_to_bytes(enum ddr_size code)
{
	switch (code) {
	case DDR_4GB: return (uint64_t)4 << 30;
	case DDR_3GB: return (uint64_t)3 << 30;
	case DDR_2GB: return (uint64_t)2 << 30;
	case DDR_1GB: return (uint64_t)1 << 30;
	case DDR_8GB:
	default:      return (uint64_t)8 << 30;
	}
}

uint64_t plat_ddr_get_total_bytes(void)
{
	return plat_ddr_total_bytes;
}

static void plat_ddr_apply_size(void)
{
	enum ddr_size code = board_get_ddr_size();

	plat_ddr_total_bytes = ddr_code_to_bytes(code);
	ddr_select_params(code);
}

#if PLAT_SYSTEM_SUSPEND
image_info_t ddr_config_info = {
	.h.type = (uint8_t)PARAM_IMAGE_BINARY,
	.h.version = (uint8_t)VERSION_2,
	.h.size = (uint16_t)sizeof(image_info_t),
	.h.attr = 0,
	.image_max_size = sizeof(ddr_csr_table),
	.image_base = (uintptr_t)&ddr_csr_table
};

int save_ddr_config(unsigned int image_id, image_info_t *image_data)
{
	uintptr_t dev_handle;
	uintptr_t image_handle;
	uintptr_t image_spec;
	uintptr_t image_base;
	size_t image_size;
	size_t bytes_write;
	int io_result;

	assert(image_data != NULL);
	assert(image_data->h.version >= VERSION_2);

	image_base = image_data->image_base;

	io_result = plat_get_image_source(image_id, &dev_handle, &image_spec);
	if (io_result != 0) {
		INFO("Failed to obtain reference to image id=%u (%i)\n",
			image_id, io_result);
		return io_result;
	}

	io_result = io_open(dev_handle, image_spec, &image_handle);
	if (io_result != 0) {
		INFO("Failed to access image id=%u (%i)\n",
			image_id, io_result);
		return io_result;
	}

	INFO("Saving DDR retention info.\n");

	io_result = io_size(image_handle, &image_size);
	if ((io_result != 0) || (image_size == 0U)) {
		WARN("Failed to determine the size of the image id=%u (%i)\n",
			image_id, io_result);
		if ((io_result == 0) && (image_size == 0U))
			io_result = -EIO;
		goto exit;
	}

	if (image_size > image_data->image_max_size) {
		WARN("Image id=%u size out of bounds\n", image_id);
		io_result = -EFBIG;
		goto exit;
	}

	image_data->image_size = (uint32_t)image_size;

	io_result = io_write(image_handle, image_base, image_size, &bytes_write);
	if ((io_result != 0) || (bytes_write < image_size)) {
		WARN("Failed to save image id=%u (%i)\n", image_id, io_result);
		goto exit;
	}

	INFO("DDR Retention Info saved.\n");

exit:
	(void)io_close(image_handle);

	(void)io_dev_close(dev_handle);

	return io_result;
}

void plat_ddr_setup(void)
{
	if (!sys_is_resume()) {
		plat_ddr_apply_size();
		ddr_setup();
		ddr_probe_chip();

		if (save_ddr_config(V2N_DDR_CONFIG_ID, &ddr_config_info) != 0) {
			ERROR("Failed to save DDR retention info.\n");
			panic();
		}
	} else {
		plat_ddr_apply_size();
		INFO("Restoring DDR retention info.\n");
		if (load_auth_image(V2N_DDR_CONFIG_ID, &ddr_config_info) != 0) {
			ERROR("Failed to load DDR retention info.\n");
			panic();
		}

		ddr_retention_exit();
	}
}
#else
void plat_ddr_setup(void)
{
	plat_ddr_apply_size();
	ddr_setup();
	ddr_probe_chip();
}
#endif /* PLAT_SYSTEM_SUSPEND */
