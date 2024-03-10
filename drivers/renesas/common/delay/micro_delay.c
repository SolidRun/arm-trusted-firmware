/*
 * Copyright (c) 2018-2020, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <arch_helpers.h>

#include "micro_delay.h"

#define RCAR_CONV_MICROSEC		1000000U

void
#if IMAGE_BL31
	__attribute__ ((section(".system_ram")))
#endif
	rcar_micro_delay(uint64_t micro_sec)
{
	uint64_t base_count;

	for (base_count = 10000000; base_count > 1; base_count--);
}
