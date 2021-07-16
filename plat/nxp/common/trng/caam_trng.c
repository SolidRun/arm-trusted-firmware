/*
 * Copyright (c) 2017-2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <string.h>

#include <caam.h>
#include <common/runtime_svc.h>
#include <dcfg.h>
#include <lib/mmio.h>
#include <tools_share/uuid.h>

#include <lib/smccc.h>
#include <services/trng_svc.h>
#include <smccc_helpers.h>

#include <plat_common.h>

DEFINE_SVC_UUID2(_plat_trng_uuid,
	0x23523c58, 0x7448, 0x4083, 0x9d, 0x16,
	0xe3, 0xfa, 0xb9, 0xf1, 0x73, 0xbc
);
uuid_t plat_trng_uuid;

/*
 * Uses the hardware rng peripheral from the caam driver to return 8 bytes of
 * entropy. Returns 'true' when done successfully, 'false' otherwise.
 */
bool plat_get_entropy(uint64_t *out)
{
	uint64_t ret;

	assert(out);
	assert(!check_uptr_overflow((uintptr_t)out, sizeof(*out)));

	ret = get_random(1);

	if (ret == 0)
		return false;

	*out = ret;

	return true;
}

void plat_entropy_setup(void)
{
	plat_trng_uuid = _plat_trng_uuid;

	/* Initialise the entropy source */
	hw_rng_instantiate();
}
