// SPDX-License-Identifier: BSD-4-Clause
/*
 * Copyright 2024 Josua Mayer <josua@solid-run.com>
 */

#define TLV_CODE_PRODUCT_NAME   0x21
#define TLV_CODE_PART_NUMBER    0x22
#define TLV_CODE_SERIAL_NUMBER  0x23
#define TLV_CODE_MAC_BASE       0x24
#define TLV_CODE_MANUF_DATE     0x25
#define TLV_CODE_DEVICE_VERSION 0x26
#define TLV_CODE_PLATFORM_NAME  0x28
#define TLV_CODE_MAC_SIZE       0x2A
#define TLV_CODE_MANUF_NAME     0x2B
#define TLV_CODE_MANUF_COUNTRY  0x2C
#define TLV_CODE_VENDOR_NAME    0x2D
#define TLV_CODE_VENDOR_EXT     0xFD
#define TLV_CODE_CRC_32	 	0xFE

bool riic_tlv_read_eeprom(uint8_t chip, uint8_t offset);
uint16_t riic_tlv_find(uint8_t code, uint8_t *output, uint16_t *offset_out);
void riic_tlv_print_eeprom(void);
