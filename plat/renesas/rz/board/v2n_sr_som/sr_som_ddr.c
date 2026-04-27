/*
 * Copyright (c) 2026, SolidRun Ltd. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Reads the SoM TLV EEPROM (m24c02 @ 0x50 on I2C8) in BL2 to determine the
 * populated DDR size, and overrides the SoC default board_get_ddr_size().
 *
 * Runs before DDR is up; uses the same RIIC controller already brought up
 * by the PMIC code in pwrc_board.c.
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <common/debug.h>
#include <riic.h>
#include <ddr.h>

#define SOM_EEPROM_ADDR		0x50
#define SOM_EEPROM_SIZE		256		/* m24c02 */

#define TLV_HDR_LEN		11
#define TLV_HDR_VERSION		0x01
#define TLV_CODE_PART_NUM	0x22

/*
 * SKU layout (0-based):
 *   sku[10] == 'D'
 *   sku[11..13] = "08G" / "04G" / "03G" / "02G" / "01G"
 */
#define SKU_DDR_OFFSET		10
#define SKU_MIN_LEN		14

static int eeprom_read(uint8_t offset, uint8_t *buf, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		if (riic_read(SOM_EEPROM_ADDR, offset + i, &buf[i]) < 0)
			return -1;
	}
	return 0;
}

enum ddr_size board_get_ddr_size(void)
{
	uint8_t hdr[TLV_HDR_LEN];
	uint8_t pn[32];
	uint16_t totallen;
	size_t off, pn_len = 0;

	riic_setup();

	if (eeprom_read(0, hdr, TLV_HDR_LEN) < 0)
		goto fallback;

	if (memcmp(hdr, "TlvInfo", 8) != 0 || hdr[8] != TLV_HDR_VERSION)
		goto fallback;

	totallen = ((uint16_t)hdr[9] << 8) | hdr[10];
	if (totallen > SOM_EEPROM_SIZE - TLV_HDR_LEN)
		totallen = SOM_EEPROM_SIZE - TLV_HDR_LEN;

	off = 0;
	while (off + 2 <= totallen) {
		uint8_t type, len;

		if (riic_read(SOM_EEPROM_ADDR, TLV_HDR_LEN + off, &type) < 0)
			goto fallback;
		if (riic_read(SOM_EEPROM_ADDR, TLV_HDR_LEN + off + 1, &len) < 0)
			goto fallback;

		if (type == TLV_CODE_PART_NUM) {
			pn_len = (len > sizeof(pn)) ? sizeof(pn) : len;
			if (eeprom_read(TLV_HDR_LEN + off + 2, pn, pn_len) < 0)
				goto fallback;
			break;
		}
		off += 2 + len;
	}

	if (pn_len < SKU_MIN_LEN ||
	    pn[SKU_DDR_OFFSET] != 'D' ||
	    pn[SKU_DDR_OFFSET + 3] != 'G') {
		WARN("RZ/V2N SoM: SKU missing DDR field, defaulting to 8GB\n");
		goto fallback;
	}

	if (memcmp(&pn[SKU_DDR_OFFSET], "D08G", 4) == 0) {
		INFO("RZ/V2N SoM: DDR = 8GB\n");
		return DDR_8GB;
	}
	if (memcmp(&pn[SKU_DDR_OFFSET], "D04G", 4) == 0) {
		INFO("RZ/V2N SoM: DDR = 4GB\n");
		return DDR_4GB;
	}
	if (memcmp(&pn[SKU_DDR_OFFSET], "D03G", 4) == 0) {
		INFO("RZ/V2N SoM: DDR = 3GB\n");
		return DDR_3GB;
	}
	if (memcmp(&pn[SKU_DDR_OFFSET], "D02G", 4) == 0) {
		INFO("RZ/V2N SoM: DDR = 2GB\n");
		return DDR_2GB;
	}
	if (memcmp(&pn[SKU_DDR_OFFSET], "D01G", 4) == 0) {
		INFO("RZ/V2N SoM: DDR = 1GB\n");
		return DDR_1GB;
	}

	WARN("RZ/V2N SoM: unsupported DDR code, defaulting to 8GB\n");

fallback:
	return DDR_8GB;
}
