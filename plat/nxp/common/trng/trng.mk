#
# Copyright 2018-2020 NXP
#
# SPDX-License-Identifier: BSD-3-Clause
#
#
#------------------------------------------------------------------------------
#
# Select the TRNG files
#
# -----------------------------------------------------------------------------

ifeq (${ADD_TRNG},)

ADD_TRNG	:= 1

PLAT_TRNG_PATH	:= $(PLAT_COMMON_PATH)/trng

TRNG_SOURCES	:= ${PLAT_TRNG_PATH}/caam_trng.c

BL31_SOURCES	+= ${TRNG_SOURCES}
endif

# -----------------------------------------------------------------------------
