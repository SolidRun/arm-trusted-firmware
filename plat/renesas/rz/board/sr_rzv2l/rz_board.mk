# SPDX-License-Identifier: BSD-3-Clause
#
# Copyright 2024 Josua Mayer <josua@solid-run.com>
#

BL2_SOURCES += \
	plat/renesas/rz/common/drivers/riic.c \
	plat/renesas/rz/common/drivers/riic_tlv.c

DDR_SOURCES += \
	plat/renesas/rz/board/sr_rzv2l/ddr_params.c

PLAT_INCLUDES += -Iplat/renesas/rz/soc/v2l/drivers/ddr
PLAT_INCLUDES += -Iplat/renesas/rz/common/drivers/ddr

DDR_PARAM_SETUP := 1
$(eval $(call add_define,DDR_PARAM_SETUP))

DDR_PLL4    := 1600
$(eval $(call add_define,DDR_PLL4))

BL33_ARG1_FDTBLOB := 0
$(eval $(call add_define,BL33_ARG1_FDTBLOB))
include lib/libfdt/libfdt.mk

BL33_ARG23_DRAM_INFO := 1
$(eval $(call add_define,BL33_ARG23_DRAM_INFO))
include lib/libfdt/libfdt.mk
