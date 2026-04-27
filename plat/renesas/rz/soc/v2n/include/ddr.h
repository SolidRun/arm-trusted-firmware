/*
 * Copyright (c) 2024, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __PLAT_DDR_H__
#define __PLAT_DDR_H__

#define RET_CSR_SIZE		(0x400)
extern uint32_t ddr_csr_table[RET_CSR_SIZE];

enum ddr_size {
	DDR_8GB = 0,	/* default */
	DDR_4GB,
	DDR_3GB,
	DDR_2GB,
	DDR_1GB,
};

void ddr_setup(void);
void ddr_retention_entry(void);
void ddr_retention_exit(void);

/* Bind size-dependent DDR parameter tables. Must run before ddr_setup(). */
void ddr_select_params(enum ddr_size code);

/*
 * Board-level DDR size hook. Default returns DDR_8GB; boards (e.g. SR SoM)
 * may override to read the size from EEPROM/SKU at boot.
 */
enum ddr_size board_get_ddr_size(void);

#endif	/* __PLAT_DDR_H__ */
