/*
 * Copyright (c) 2015, ARM Limited and Contributors. All rights reserved.
 * Copyright 2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <arch.h>
#include <arch_helpers.h>
#include <debug.h>
#include <ddrc.h>
#include <mmio.h>
#include <platform_def.h>
#include <spinlock.h>
#include <soc.h>

struct ddrphy_trained_csr {
	uint32_t offset;
	uint32_t val;
};
static struct ddrphy_trained_csr lpddr4_saved_trained_csr[] = {
    { .offset = 0x200b2,  .val = 0 },
    { .offset = 0x1200b2, .val = 0 },
    { .offset = 0x2200b2, .val = 0 },
    { .offset = 0x3200b2, .val = 0 },
    { .offset = 0x200cb,  .val = 0 },
    { .offset = 0x1004a,  .val = 0 },
    { .offset = 0x1004c,  .val = 0 },
    { .offset = 0x11004c, .val = 0 },
    { .offset = 0x21004c, .val = 0 },
    { .offset = 0x31004c, .val = 0 },
    { .offset = 0x10043,  .val = 0 },
    { .offset = 0x110043, .val = 0 },
    { .offset = 0x210043, .val = 0 },
    { .offset = 0x310043, .val = 0 },
    { .offset = 0x10143,  .val = 0 },
    { .offset = 0x110143, .val = 0 },
    { .offset = 0x210143, .val = 0 },
    { .offset = 0x310143, .val = 0 },
    { .offset = 0x1104a,  .val = 0 },
    { .offset = 0x1104c,  .val = 0 },
    { .offset = 0x11104c, .val = 0 },
    { .offset = 0x21104c, .val = 0 },
    { .offset = 0x31104c, .val = 0 },
    { .offset = 0x11043,  .val = 0 },
    { .offset = 0x111043, .val = 0 },
    { .offset = 0x211043, .val = 0 },
    { .offset = 0x311043, .val = 0 },
    { .offset = 0x11143,  .val = 0 },
    { .offset = 0x111143, .val = 0 },
    { .offset = 0x211143, .val = 0 },
    { .offset = 0x311143, .val = 0 },
    { .offset = 0x1204a,  .val = 0 },
    { .offset = 0x1204c,  .val = 0 },
    { .offset = 0x11204c, .val = 0 },
    { .offset = 0x21204c, .val = 0 },
    { .offset = 0x31204c, .val = 0 },
    { .offset = 0x12043,  .val = 0 },
    { .offset = 0x112043, .val = 0 },
    { .offset = 0x212043, .val = 0 },
    { .offset = 0x312043, .val = 0 },
    { .offset = 0x12143,  .val = 0 },
    { .offset = 0x112143, .val = 0 },
    { .offset = 0x212143, .val = 0 },
    { .offset = 0x312143, .val = 0 },
    { .offset = 0x1304a,  .val = 0 },
    { .offset = 0x1304c,  .val = 0 },
    { .offset = 0x11304c, .val = 0 },
    { .offset = 0x21304c, .val = 0 },
    { .offset = 0x31304c, .val = 0 },
    { .offset = 0x13043,  .val = 0 },
    { .offset = 0x113043, .val = 0 },
    { .offset = 0x213043, .val = 0 },
    { .offset = 0x313043, .val = 0 },
    { .offset = 0x13143,  .val = 0 },
    { .offset = 0x113143, .val = 0 },
    { .offset = 0x213143, .val = 0 },
    { .offset = 0x313143, .val = 0 },
    { .offset = 0x80,     .val = 0 },
    { .offset = 0x100080, .val = 0 },
    { .offset = 0x200080, .val = 0 },
    { .offset = 0x300080, .val = 0 },
    { .offset = 0x1080,   .val = 0 },
    { .offset = 0x101080, .val = 0 },
    { .offset = 0x201080, .val = 0 },
    { .offset = 0x301080, .val = 0 },
    { .offset = 0x2080,   .val = 0 },
    { .offset = 0x102080, .val = 0 },
    { .offset = 0x202080, .val = 0 },
    { .offset = 0x302080, .val = 0 },
    { .offset = 0x3080,   .val = 0 },
    { .offset = 0x103080, .val = 0 },
    { .offset = 0x203080, .val = 0 },
    { .offset = 0x303080, .val = 0 },
    { .offset = 0x4080,   .val = 0 },
    { .offset = 0x104080, .val = 0 },
    { .offset = 0x204080, .val = 0 },
    { .offset = 0x304080, .val = 0 },
    { .offset = 0x5080,   .val = 0 },
    { .offset = 0x105080, .val = 0 },
    { .offset = 0x205080, .val = 0 },
    { .offset = 0x305080, .val = 0 },
    { .offset = 0x6080,   .val = 0 },
    { .offset = 0x106080, .val = 0 },
    { .offset = 0x206080, .val = 0 },
    { .offset = 0x306080, .val = 0 },
    { .offset = 0x7080,   .val = 0 },
    { .offset = 0x107080, .val = 0 },
    { .offset = 0x207080, .val = 0 },
    { .offset = 0x307080, .val = 0 },
    { .offset = 0x8080,   .val = 0 },
    { .offset = 0x108080, .val = 0 },
    { .offset = 0x208080, .val = 0 },
    { .offset = 0x308080, .val = 0 },
    { .offset = 0x9080,   .val = 0 },
    { .offset = 0x109080, .val = 0 },
    { .offset = 0x209080, .val = 0 },
    { .offset = 0x309080, .val = 0 },
    { .offset = 0x10080,  .val = 0 },
    { .offset = 0x110080, .val = 0 },
    { .offset = 0x210080, .val = 0 },
    { .offset = 0x310080, .val = 0 },
    { .offset = 0x10180,  .val = 0 },
    { .offset = 0x110180, .val = 0 },
    { .offset = 0x210180, .val = 0 },
    { .offset = 0x310180, .val = 0 },
    { .offset = 0x10081,  .val = 0 },
    { .offset = 0x110081, .val = 0 },
    { .offset = 0x210081, .val = 0 },
    { .offset = 0x310081, .val = 0 },
    { .offset = 0x10181,  .val = 0 },
    { .offset = 0x110181, .val = 0 },
    { .offset = 0x210181, .val = 0 },
    { .offset = 0x310181, .val = 0 },
    { .offset = 0x10082,  .val = 0 },
    { .offset = 0x110082, .val = 0 },
    { .offset = 0x210082, .val = 0 },
    { .offset = 0x310082, .val = 0 },
    { .offset = 0x10182,  .val = 0 },
    { .offset = 0x110182, .val = 0 },
    { .offset = 0x210182, .val = 0 },
    { .offset = 0x310182, .val = 0 },
    { .offset = 0x10083,  .val = 0 },
    { .offset = 0x110083, .val = 0 },
    { .offset = 0x210083, .val = 0 },
    { .offset = 0x310083, .val = 0 },
    { .offset = 0x10183,  .val = 0 },
    { .offset = 0x110183, .val = 0 },
    { .offset = 0x210183, .val = 0 },
    { .offset = 0x310183, .val = 0 },
    { .offset = 0x11080,  .val = 0 },
    { .offset = 0x111080, .val = 0 },
    { .offset = 0x211080, .val = 0 },
    { .offset = 0x311080, .val = 0 },
    { .offset = 0x11180,  .val = 0 },
    { .offset = 0x111180, .val = 0 },
    { .offset = 0x211180, .val = 0 },
    { .offset = 0x311180, .val = 0 },
    { .offset = 0x11081,  .val = 0 },
    { .offset = 0x111081, .val = 0 },
    { .offset = 0x211081, .val = 0 },
    { .offset = 0x311081, .val = 0 },
    { .offset = 0x11181,  .val = 0 },
    { .offset = 0x111181, .val = 0 },
    { .offset = 0x211181, .val = 0 },
    { .offset = 0x311181, .val = 0 },
    { .offset = 0x11082,  .val = 0 },
    { .offset = 0x111082, .val = 0 },
    { .offset = 0x211082, .val = 0 },
    { .offset = 0x311082, .val = 0 },
    { .offset = 0x11182,  .val = 0 },
    { .offset = 0x111182, .val = 0 },
    { .offset = 0x211182, .val = 0 },
    { .offset = 0x311182, .val = 0 },
    { .offset = 0x11083,  .val = 0 },
    { .offset = 0x111083, .val = 0 },
    { .offset = 0x211083, .val = 0 },
    { .offset = 0x311083, .val = 0 },
    { .offset = 0x11183,  .val = 0 },
    { .offset = 0x111183, .val = 0 },
    { .offset = 0x211183, .val = 0 },
    { .offset = 0x311183, .val = 0 },
    { .offset = 0x12080,  .val = 0 },
    { .offset = 0x112080, .val = 0 },
    { .offset = 0x212080, .val = 0 },
    { .offset = 0x312080, .val = 0 },
    { .offset = 0x12180,  .val = 0 },
    { .offset = 0x112180, .val = 0 },
    { .offset = 0x212180, .val = 0 },
    { .offset = 0x312180, .val = 0 },
    { .offset = 0x12081,  .val = 0 },
    { .offset = 0x112081, .val = 0 },
    { .offset = 0x212081, .val = 0 },
    { .offset = 0x312081, .val = 0 },
    { .offset = 0x12181,  .val = 0 },
    { .offset = 0x112181, .val = 0 },
    { .offset = 0x212181, .val = 0 },
    { .offset = 0x312181, .val = 0 },
    { .offset = 0x12082,  .val = 0 },
    { .offset = 0x112082, .val = 0 },
    { .offset = 0x212082, .val = 0 },
    { .offset = 0x312082, .val = 0 },
    { .offset = 0x12182,  .val = 0 },
    { .offset = 0x112182, .val = 0 },
    { .offset = 0x212182, .val = 0 },
    { .offset = 0x312182, .val = 0 },
    { .offset = 0x12083,  .val = 0 },
    { .offset = 0x112083, .val = 0 },
    { .offset = 0x212083, .val = 0 },
    { .offset = 0x312083, .val = 0 },
    { .offset = 0x12183,  .val = 0 },
    { .offset = 0x112183, .val = 0 },
    { .offset = 0x212183, .val = 0 },
    { .offset = 0x312183, .val = 0 },
    { .offset = 0x13080,  .val = 0 },
    { .offset = 0x113080, .val = 0 },
    { .offset = 0x213080, .val = 0 },
    { .offset = 0x313080, .val = 0 },
    { .offset = 0x13180,  .val = 0 },
    { .offset = 0x113180, .val = 0 },
    { .offset = 0x213180, .val = 0 },
    { .offset = 0x313180, .val = 0 },
    { .offset = 0x13081,  .val = 0 },
    { .offset = 0x113081, .val = 0 },
    { .offset = 0x213081, .val = 0 },
    { .offset = 0x313081, .val = 0 },
    { .offset = 0x13181,  .val = 0 },
    { .offset = 0x113181, .val = 0 },
    { .offset = 0x213181, .val = 0 },
    { .offset = 0x313181, .val = 0 },
    { .offset = 0x13082,  .val = 0 },
    { .offset = 0x113082, .val = 0 },
    { .offset = 0x213082, .val = 0 },
    { .offset = 0x313082, .val = 0 },
    { .offset = 0x13182,  .val = 0 },
    { .offset = 0x113182, .val = 0 },
    { .offset = 0x213182, .val = 0 },
    { .offset = 0x313182, .val = 0 },
    { .offset = 0x13083,  .val = 0 },
    { .offset = 0x113083, .val = 0 },
    { .offset = 0x213083, .val = 0 },
    { .offset = 0x313083, .val = 0 },
    { .offset = 0x13183,  .val = 0 },
    { .offset = 0x113183, .val = 0 },
    { .offset = 0x213183, .val = 0 },
    { .offset = 0x313183, .val = 0 },
    { .offset = 0x100d0,  .val = 0 },
    { .offset = 0x1100d0, .val = 0 },
    { .offset = 0x2100d0, .val = 0 },
    { .offset = 0x3100d0, .val = 0 },
    { .offset = 0x101d0,  .val = 0 },
    { .offset = 0x1101d0, .val = 0 },
    { .offset = 0x2101d0, .val = 0 },
    { .offset = 0x3101d0, .val = 0 },
    { .offset = 0x100d1,  .val = 0 },
    { .offset = 0x1100d1, .val = 0 },
    { .offset = 0x2100d1, .val = 0 },
    { .offset = 0x3100d1, .val = 0 },
    { .offset = 0x101d1,  .val = 0 },
    { .offset = 0x1101d1, .val = 0 },
    { .offset = 0x2101d1, .val = 0 },
    { .offset = 0x3101d1, .val = 0 },
    { .offset = 0x100d2,  .val = 0 },
    { .offset = 0x1100d2, .val = 0 },
    { .offset = 0x2100d2, .val = 0 },
    { .offset = 0x3100d2, .val = 0 },
    { .offset = 0x101d2,  .val = 0 },
    { .offset = 0x1101d2, .val = 0 },
    { .offset = 0x2101d2, .val = 0 },
    { .offset = 0x3101d2, .val = 0 },
    { .offset = 0x100d3,  .val = 0 },
    { .offset = 0x1100d3, .val = 0 },
    { .offset = 0x2100d3, .val = 0 },
    { .offset = 0x3100d3, .val = 0 },
    { .offset = 0x101d3,  .val = 0 },
    { .offset = 0x1101d3, .val = 0 },
    { .offset = 0x2101d3, .val = 0 },
    { .offset = 0x3101d3, .val = 0 },
    { .offset = 0x110d0,  .val = 0 },
    { .offset = 0x1110d0, .val = 0 },
    { .offset = 0x2110d0, .val = 0 },
    { .offset = 0x3110d0, .val = 0 },
    { .offset = 0x111d0,  .val = 0 },
    { .offset = 0x1111d0, .val = 0 },
    { .offset = 0x2111d0, .val = 0 },
    { .offset = 0x3111d0, .val = 0 },
    { .offset = 0x110d1,  .val = 0 },
    { .offset = 0x1110d1, .val = 0 },
    { .offset = 0x2110d1, .val = 0 },
    { .offset = 0x3110d1, .val = 0 },
    { .offset = 0x111d1,  .val = 0 },
    { .offset = 0x1111d1, .val = 0 },
    { .offset = 0x2111d1, .val = 0 },
    { .offset = 0x3111d1, .val = 0 },
    { .offset = 0x110d2,  .val = 0 },
    { .offset = 0x1110d2, .val = 0 },
    { .offset = 0x2110d2, .val = 0 },
    { .offset = 0x3110d2, .val = 0 },
    { .offset = 0x111d2,  .val = 0 },
    { .offset = 0x1111d2, .val = 0 },
    { .offset = 0x2111d2, .val = 0 },
    { .offset = 0x3111d2, .val = 0 },
    { .offset = 0x110d3,  .val = 0 },
    { .offset = 0x1110d3, .val = 0 },
    { .offset = 0x2110d3, .val = 0 },
    { .offset = 0x3110d3, .val = 0 },
    { .offset = 0x111d3,  .val = 0 },
    { .offset = 0x1111d3, .val = 0 },
    { .offset = 0x2111d3, .val = 0 },
    { .offset = 0x3111d3, .val = 0 },
    { .offset = 0x120d0,  .val = 0 },
    { .offset = 0x1120d0, .val = 0 },
    { .offset = 0x2120d0, .val = 0 },
    { .offset = 0x3120d0, .val = 0 },
    { .offset = 0x121d0,  .val = 0 },
    { .offset = 0x1121d0, .val = 0 },
    { .offset = 0x2121d0, .val = 0 },
    { .offset = 0x3121d0, .val = 0 },
    { .offset = 0x120d1,  .val = 0 },
    { .offset = 0x1120d1, .val = 0 },
    { .offset = 0x2120d1, .val = 0 },
    { .offset = 0x3120d1, .val = 0 },
    { .offset = 0x121d1,  .val = 0 },
    { .offset = 0x1121d1, .val = 0 },
    { .offset = 0x2121d1, .val = 0 },
    { .offset = 0x3121d1, .val = 0 },
    { .offset = 0x120d2,  .val = 0 },
    { .offset = 0x1120d2, .val = 0 },
    { .offset = 0x2120d2, .val = 0 },
    { .offset = 0x3120d2, .val = 0 },
    { .offset = 0x121d2,  .val = 0 },
    { .offset = 0x1121d2, .val = 0 },
    { .offset = 0x2121d2, .val = 0 },
    { .offset = 0x3121d2, .val = 0 },
    { .offset = 0x120d3,  .val = 0 },
    { .offset = 0x1120d3, .val = 0 },
    { .offset = 0x2120d3, .val = 0 },
    { .offset = 0x3120d3, .val = 0 },
    { .offset = 0x121d3,  .val = 0 },
    { .offset = 0x1121d3, .val = 0 },
    { .offset = 0x2121d3, .val = 0 },
    { .offset = 0x3121d3, .val = 0 },
    { .offset = 0x130d0,  .val = 0 },
    { .offset = 0x1130d0, .val = 0 },
    { .offset = 0x2130d0, .val = 0 },
    { .offset = 0x3130d0, .val = 0 },
    { .offset = 0x131d0,  .val = 0 },
    { .offset = 0x1131d0, .val = 0 },
    { .offset = 0x2131d0, .val = 0 },
    { .offset = 0x3131d0, .val = 0 },
    { .offset = 0x130d1,  .val = 0 },
    { .offset = 0x1130d1, .val = 0 },
    { .offset = 0x2130d1, .val = 0 },
    { .offset = 0x3130d1, .val = 0 },
    { .offset = 0x131d1,  .val = 0 },
    { .offset = 0x1131d1, .val = 0 },
    { .offset = 0x2131d1, .val = 0 },
    { .offset = 0x3131d1, .val = 0 },
    { .offset = 0x130d2,  .val = 0 },
    { .offset = 0x1130d2, .val = 0 },
    { .offset = 0x2130d2, .val = 0 },
    { .offset = 0x3130d2, .val = 0 },
    { .offset = 0x131d2,  .val = 0 },
    { .offset = 0x1131d2, .val = 0 },
    { .offset = 0x2131d2, .val = 0 },
    { .offset = 0x3131d2, .val = 0 },
    { .offset = 0x130d3,  .val = 0 },
    { .offset = 0x1130d3, .val = 0 },
    { .offset = 0x2130d3, .val = 0 },
    { .offset = 0x3130d3, .val = 0 },
    { .offset = 0x131d3,  .val = 0 },
    { .offset = 0x1131d3, .val = 0 },
    { .offset = 0x2131d3, .val = 0 },
    { .offset = 0x3131d3, .val = 0 },
    { .offset = 0x10068,  .val = 0 },
    { .offset = 0x10168,  .val = 0 },
    { .offset = 0x10268,  .val = 0 },
    { .offset = 0x10368,  .val = 0 },
    { .offset = 0x10468,  .val = 0 },
    { .offset = 0x10568,  .val = 0 },
    { .offset = 0x10668,  .val = 0 },
    { .offset = 0x10768,  .val = 0 },
    { .offset = 0x10868,  .val = 0 },
    { .offset = 0x10069,  .val = 0 },
    { .offset = 0x10169,  .val = 0 },
    { .offset = 0x10269,  .val = 0 },
    { .offset = 0x10369,  .val = 0 },
    { .offset = 0x10469,  .val = 0 },
    { .offset = 0x10569,  .val = 0 },
    { .offset = 0x10669,  .val = 0 },
    { .offset = 0x10769,  .val = 0 },
    { .offset = 0x10869,  .val = 0 },
    { .offset = 0x1006a,  .val = 0 },
    { .offset = 0x1016a,  .val = 0 },
    { .offset = 0x1026a,  .val = 0 },
    { .offset = 0x1036a,  .val = 0 },
    { .offset = 0x1046a,  .val = 0 },
    { .offset = 0x1056a,  .val = 0 },
    { .offset = 0x1066a,  .val = 0 },
    { .offset = 0x1076a,  .val = 0 },
    { .offset = 0x1086a,  .val = 0 },
    { .offset = 0x1006b,  .val = 0 },
    { .offset = 0x1016b,  .val = 0 },
    { .offset = 0x1026b,  .val = 0 },
    { .offset = 0x1036b,  .val = 0 },
    { .offset = 0x1046b,  .val = 0 },
    { .offset = 0x1056b,  .val = 0 },
    { .offset = 0x1066b,  .val = 0 },
    { .offset = 0x1076b,  .val = 0 },
    { .offset = 0x1086b,  .val = 0 },
    { .offset = 0x11068,  .val = 0 },
    { .offset = 0x11168,  .val = 0 },
    { .offset = 0x11268,  .val = 0 },
    { .offset = 0x11368,  .val = 0 },
    { .offset = 0x11468,  .val = 0 },
    { .offset = 0x11568,  .val = 0 },
    { .offset = 0x11668,  .val = 0 },
    { .offset = 0x11768,  .val = 0 },
    { .offset = 0x11868,  .val = 0 },
    { .offset = 0x11069,  .val = 0 },
    { .offset = 0x11169,  .val = 0 },
    { .offset = 0x11269,  .val = 0 },
    { .offset = 0x11369,  .val = 0 },
    { .offset = 0x11469,  .val = 0 },
    { .offset = 0x11569,  .val = 0 },
    { .offset = 0x11669,  .val = 0 },
    { .offset = 0x11769,  .val = 0 },
    { .offset = 0x11869,  .val = 0 },
    { .offset = 0x1106a,  .val = 0 },
    { .offset = 0x1116a,  .val = 0 },
    { .offset = 0x1126a,  .val = 0 },
    { .offset = 0x1136a,  .val = 0 },
    { .offset = 0x1146a,  .val = 0 },
    { .offset = 0x1156a,  .val = 0 },
    { .offset = 0x1166a,  .val = 0 },
    { .offset = 0x1176a,  .val = 0 },
    { .offset = 0x1186a,  .val = 0 },
    { .offset = 0x1106b,  .val = 0 },
    { .offset = 0x1116b,  .val = 0 },
    { .offset = 0x1126b,  .val = 0 },
    { .offset = 0x1136b,  .val = 0 },
    { .offset = 0x1146b,  .val = 0 },
    { .offset = 0x1156b,  .val = 0 },
    { .offset = 0x1166b,  .val = 0 },
    { .offset = 0x1176b,  .val = 0 },
    { .offset = 0x1186b,  .val = 0 },
    { .offset = 0x12068,  .val = 0 },
    { .offset = 0x12168,  .val = 0 },
    { .offset = 0x12268,  .val = 0 },
    { .offset = 0x12368,  .val = 0 },
    { .offset = 0x12468,  .val = 0 },
    { .offset = 0x12568,  .val = 0 },
    { .offset = 0x12668,  .val = 0 },
    { .offset = 0x12768,  .val = 0 },
    { .offset = 0x12868,  .val = 0 },
    { .offset = 0x12069,  .val = 0 },
    { .offset = 0x12169,  .val = 0 },
    { .offset = 0x12269,  .val = 0 },
    { .offset = 0x12369,  .val = 0 },
    { .offset = 0x12469,  .val = 0 },
    { .offset = 0x12569,  .val = 0 },
    { .offset = 0x12669,  .val = 0 },
    { .offset = 0x12769,  .val = 0 },
    { .offset = 0x12869,  .val = 0 },
    { .offset = 0x1206a,  .val = 0 },
    { .offset = 0x1216a,  .val = 0 },
    { .offset = 0x1226a,  .val = 0 },
    { .offset = 0x1236a,  .val = 0 },
    { .offset = 0x1246a,  .val = 0 },
    { .offset = 0x1256a,  .val = 0 },
    { .offset = 0x1266a,  .val = 0 },
    { .offset = 0x1276a,  .val = 0 },
    { .offset = 0x1286a,  .val = 0 },
    { .offset = 0x1206b,  .val = 0 },
    { .offset = 0x1216b,  .val = 0 },
    { .offset = 0x1226b,  .val = 0 },
    { .offset = 0x1236b,  .val = 0 },
    { .offset = 0x1246b,  .val = 0 },
    { .offset = 0x1256b,  .val = 0 },
    { .offset = 0x1266b,  .val = 0 },
    { .offset = 0x1276b,  .val = 0 },
    { .offset = 0x1286b,  .val = 0 },
    { .offset = 0x13068,  .val = 0 },
    { .offset = 0x13168,  .val = 0 },
    { .offset = 0x13268,  .val = 0 },
    { .offset = 0x13368,  .val = 0 },
    { .offset = 0x13468,  .val = 0 },
    { .offset = 0x13568,  .val = 0 },
    { .offset = 0x13668,  .val = 0 },
    { .offset = 0x13768,  .val = 0 },
    { .offset = 0x13868,  .val = 0 },
    { .offset = 0x13069,  .val = 0 },
    { .offset = 0x13169,  .val = 0 },
    { .offset = 0x13269,  .val = 0 },
    { .offset = 0x13369,  .val = 0 },
    { .offset = 0x13469,  .val = 0 },
    { .offset = 0x13569,  .val = 0 },
    { .offset = 0x13669,  .val = 0 },
    { .offset = 0x13769,  .val = 0 },
    { .offset = 0x13869,  .val = 0 },
    { .offset = 0x1306a,  .val = 0 },
    { .offset = 0x1316a,  .val = 0 },
    { .offset = 0x1326a,  .val = 0 },
    { .offset = 0x1336a,  .val = 0 },
    { .offset = 0x1346a,  .val = 0 },
    { .offset = 0x1356a,  .val = 0 },
    { .offset = 0x1366a,  .val = 0 },
    { .offset = 0x1376a,  .val = 0 },
    { .offset = 0x1386a,  .val = 0 },
    { .offset = 0x1306b,  .val = 0 },
    { .offset = 0x1316b,  .val = 0 },
    { .offset = 0x1326b,  .val = 0 },
    { .offset = 0x1336b,  .val = 0 },
    { .offset = 0x1346b,  .val = 0 },
    { .offset = 0x1356b,  .val = 0 },
    { .offset = 0x1366b,  .val = 0 },
    { .offset = 0x1376b,  .val = 0 },
    { .offset = 0x1386b,  .val = 0 },
    { .offset = 0x1008c,  .val = 0 },
    { .offset = 0x11008c, .val = 0 },
    { .offset = 0x21008c, .val = 0 },
    { .offset = 0x31008c, .val = 0 },
    { .offset = 0x1018c,  .val = 0 },
    { .offset = 0x11018c, .val = 0 },
    { .offset = 0x21018c, .val = 0 },
    { .offset = 0x31018c, .val = 0 },
    { .offset = 0x1008d,  .val = 0 },
    { .offset = 0x11008d, .val = 0 },
    { .offset = 0x21008d, .val = 0 },
    { .offset = 0x31008d, .val = 0 },
    { .offset = 0x1018d,  .val = 0 },
    { .offset = 0x11018d, .val = 0 },
    { .offset = 0x21018d, .val = 0 },
    { .offset = 0x31018d, .val = 0 },
    { .offset = 0x1008e,  .val = 0 },
    { .offset = 0x11008e, .val = 0 },
    { .offset = 0x21008e, .val = 0 },
    { .offset = 0x31008e, .val = 0 },
    { .offset = 0x1018e,  .val = 0 },
    { .offset = 0x11018e, .val = 0 },
    { .offset = 0x21018e, .val = 0 },
    { .offset = 0x31018e, .val = 0 },
    { .offset = 0x1008f,  .val = 0 },
    { .offset = 0x11008f, .val = 0 },
    { .offset = 0x21008f, .val = 0 },
    { .offset = 0x31008f, .val = 0 },
    { .offset = 0x1018f,  .val = 0 },
    { .offset = 0x11018f, .val = 0 },
    { .offset = 0x21018f, .val = 0 },
    { .offset = 0x31018f, .val = 0 },
    { .offset = 0x1108c,  .val = 0 },
    { .offset = 0x11108c, .val = 0 },
    { .offset = 0x21108c, .val = 0 },
    { .offset = 0x31108c, .val = 0 },
    { .offset = 0x1118c,  .val = 0 },
    { .offset = 0x11118c, .val = 0 },
    { .offset = 0x21118c, .val = 0 },
    { .offset = 0x31118c, .val = 0 },
    { .offset = 0x1108d,  .val = 0 },
    { .offset = 0x11108d, .val = 0 },
    { .offset = 0x21108d, .val = 0 },
    { .offset = 0x31108d, .val = 0 },
    { .offset = 0x1118d,  .val = 0 },
    { .offset = 0x11118d, .val = 0 },
    { .offset = 0x21118d, .val = 0 },
    { .offset = 0x31118d, .val = 0 },
    { .offset = 0x1108e,  .val = 0 },
    { .offset = 0x11108e, .val = 0 },
    { .offset = 0x21108e, .val = 0 },
    { .offset = 0x31108e, .val = 0 },
    { .offset = 0x1118e,  .val = 0 },
    { .offset = 0x11118e, .val = 0 },
    { .offset = 0x21118e, .val = 0 },
    { .offset = 0x31118e, .val = 0 },
    { .offset = 0x1108f,  .val = 0 },
    { .offset = 0x11108f, .val = 0 },
    { .offset = 0x21108f, .val = 0 },
    { .offset = 0x31108f, .val = 0 },
    { .offset = 0x1118f,  .val = 0 },
    { .offset = 0x11118f, .val = 0 },
    { .offset = 0x21118f, .val = 0 },
    { .offset = 0x31118f, .val = 0 },
    { .offset = 0x1208c,  .val = 0 },
    { .offset = 0x11208c, .val = 0 },
    { .offset = 0x21208c, .val = 0 },
    { .offset = 0x31208c, .val = 0 },
    { .offset = 0x1218c,  .val = 0 },
    { .offset = 0x11218c, .val = 0 },
    { .offset = 0x21218c, .val = 0 },
    { .offset = 0x31218c, .val = 0 },
    { .offset = 0x1208d,  .val = 0 },
    { .offset = 0x11208d, .val = 0 },
    { .offset = 0x21208d, .val = 0 },
    { .offset = 0x31208d, .val = 0 },
    { .offset = 0x1218d,  .val = 0 },
    { .offset = 0x11218d, .val = 0 },
    { .offset = 0x21218d, .val = 0 },
    { .offset = 0x31218d, .val = 0 },
    { .offset = 0x1208e,  .val = 0 },
    { .offset = 0x11208e, .val = 0 },
    { .offset = 0x21208e, .val = 0 },
    { .offset = 0x31208e, .val = 0 },
    { .offset = 0x1218e,  .val = 0 },
    { .offset = 0x11218e, .val = 0 },
    { .offset = 0x21218e, .val = 0 },
    { .offset = 0x31218e, .val = 0 },
    { .offset = 0x1208f,  .val = 0 },
    { .offset = 0x11208f, .val = 0 },
    { .offset = 0x21208f, .val = 0 },
    { .offset = 0x31208f, .val = 0 },
    { .offset = 0x1218f,  .val = 0 },
    { .offset = 0x11218f, .val = 0 },
    { .offset = 0x21218f, .val = 0 },
    { .offset = 0x31218f, .val = 0 },
    { .offset = 0x1308c,  .val = 0 },
    { .offset = 0x11308c, .val = 0 },
    { .offset = 0x21308c, .val = 0 },
    { .offset = 0x31308c, .val = 0 },
    { .offset = 0x1318c,  .val = 0 },
    { .offset = 0x11318c, .val = 0 },
    { .offset = 0x21318c, .val = 0 },
    { .offset = 0x31318c, .val = 0 },
    { .offset = 0x1308d,  .val = 0 },
    { .offset = 0x11308d, .val = 0 },
    { .offset = 0x21308d, .val = 0 },
    { .offset = 0x31308d, .val = 0 },
    { .offset = 0x1318d,  .val = 0 },
    { .offset = 0x11318d, .val = 0 },
    { .offset = 0x21318d, .val = 0 },
    { .offset = 0x31318d, .val = 0 },
    { .offset = 0x1308e,  .val = 0 },
    { .offset = 0x11308e, .val = 0 },
    { .offset = 0x21308e, .val = 0 },
    { .offset = 0x31308e, .val = 0 },
    { .offset = 0x1318e,  .val = 0 },
    { .offset = 0x11318e, .val = 0 },
    { .offset = 0x21318e, .val = 0 },
    { .offset = 0x31318e, .val = 0 },
    { .offset = 0x1308f,  .val = 0 },
    { .offset = 0x11308f, .val = 0 },
    { .offset = 0x21308f, .val = 0 },
    { .offset = 0x31308f, .val = 0 },
    { .offset = 0x1318f,  .val = 0 },
    { .offset = 0x11318f, .val = 0 },
    { .offset = 0x21318f, .val = 0 },
    { .offset = 0x31318f, .val = 0 },
    { .offset = 0x100c0,  .val = 0 },
    { .offset = 0x1100c0, .val = 0 },
    { .offset = 0x2100c0, .val = 0 },
    { .offset = 0x3100c0, .val = 0 },
    { .offset = 0x101c0,  .val = 0 },
    { .offset = 0x1101c0, .val = 0 },
    { .offset = 0x2101c0, .val = 0 },
    { .offset = 0x3101c0, .val = 0 },
    { .offset = 0x102c0,  .val = 0 },
    { .offset = 0x1102c0, .val = 0 },
    { .offset = 0x2102c0, .val = 0 },
    { .offset = 0x3102c0, .val = 0 },
    { .offset = 0x103c0,  .val = 0 },
    { .offset = 0x1103c0, .val = 0 },
    { .offset = 0x2103c0, .val = 0 },
    { .offset = 0x3103c0, .val = 0 },
    { .offset = 0x104c0,  .val = 0 },
    { .offset = 0x1104c0, .val = 0 },
    { .offset = 0x2104c0, .val = 0 },
    { .offset = 0x3104c0, .val = 0 },
    { .offset = 0x105c0,  .val = 0 },
    { .offset = 0x1105c0, .val = 0 },
    { .offset = 0x2105c0, .val = 0 },
    { .offset = 0x3105c0, .val = 0 },
    { .offset = 0x106c0,  .val = 0 },
    { .offset = 0x1106c0, .val = 0 },
    { .offset = 0x2106c0, .val = 0 },
    { .offset = 0x3106c0, .val = 0 },
    { .offset = 0x107c0,  .val = 0 },
    { .offset = 0x1107c0, .val = 0 },
    { .offset = 0x2107c0, .val = 0 },
    { .offset = 0x3107c0, .val = 0 },
    { .offset = 0x108c0,  .val = 0 },
    { .offset = 0x1108c0, .val = 0 },
    { .offset = 0x2108c0, .val = 0 },
    { .offset = 0x3108c0, .val = 0 },
    { .offset = 0x100c1,  .val = 0 },
    { .offset = 0x1100c1, .val = 0 },
    { .offset = 0x2100c1, .val = 0 },
    { .offset = 0x3100c1, .val = 0 },
    { .offset = 0x101c1,  .val = 0 },
    { .offset = 0x1101c1, .val = 0 },
    { .offset = 0x2101c1, .val = 0 },
    { .offset = 0x3101c1, .val = 0 },
    { .offset = 0x102c1,  .val = 0 },
    { .offset = 0x1102c1, .val = 0 },
    { .offset = 0x2102c1, .val = 0 },
    { .offset = 0x3102c1, .val = 0 },
    { .offset = 0x103c1,  .val = 0 },
    { .offset = 0x1103c1, .val = 0 },
    { .offset = 0x2103c1, .val = 0 },
    { .offset = 0x3103c1, .val = 0 },
    { .offset = 0x104c1,  .val = 0 },
    { .offset = 0x1104c1, .val = 0 },
    { .offset = 0x2104c1, .val = 0 },
    { .offset = 0x3104c1, .val = 0 },
    { .offset = 0x105c1,  .val = 0 },
    { .offset = 0x1105c1, .val = 0 },
    { .offset = 0x2105c1, .val = 0 },
    { .offset = 0x3105c1, .val = 0 },
    { .offset = 0x106c1,  .val = 0 },
    { .offset = 0x1106c1, .val = 0 },
    { .offset = 0x2106c1, .val = 0 },
    { .offset = 0x3106c1, .val = 0 },
    { .offset = 0x107c1,  .val = 0 },
    { .offset = 0x1107c1, .val = 0 },
    { .offset = 0x2107c1, .val = 0 },
    { .offset = 0x3107c1, .val = 0 },
    { .offset = 0x108c1,  .val = 0 },
    { .offset = 0x1108c1, .val = 0 },
    { .offset = 0x2108c1, .val = 0 },
    { .offset = 0x3108c1, .val = 0 },
    { .offset = 0x100c2,  .val = 0 },
    { .offset = 0x1100c2, .val = 0 },
    { .offset = 0x2100c2, .val = 0 },
    { .offset = 0x3100c2, .val = 0 },
    { .offset = 0x101c2,  .val = 0 },
    { .offset = 0x1101c2, .val = 0 },
    { .offset = 0x2101c2, .val = 0 },
    { .offset = 0x3101c2, .val = 0 },
    { .offset = 0x102c2,  .val = 0 },
    { .offset = 0x1102c2, .val = 0 },
    { .offset = 0x2102c2, .val = 0 },
    { .offset = 0x3102c2, .val = 0 },
    { .offset = 0x103c2,  .val = 0 },
    { .offset = 0x1103c2, .val = 0 },
    { .offset = 0x2103c2, .val = 0 },
    { .offset = 0x3103c2, .val = 0 },
    { .offset = 0x104c2,  .val = 0 },
    { .offset = 0x1104c2, .val = 0 },
    { .offset = 0x2104c2, .val = 0 },
    { .offset = 0x3104c2, .val = 0 },
    { .offset = 0x105c2,  .val = 0 },
    { .offset = 0x1105c2, .val = 0 },
    { .offset = 0x2105c2, .val = 0 },
    { .offset = 0x3105c2, .val = 0 },
    { .offset = 0x106c2,  .val = 0 },
    { .offset = 0x1106c2, .val = 0 },
    { .offset = 0x2106c2, .val = 0 },
    { .offset = 0x3106c2, .val = 0 },
    { .offset = 0x107c2,  .val = 0 },
    { .offset = 0x1107c2, .val = 0 },
    { .offset = 0x2107c2, .val = 0 },
    { .offset = 0x3107c2, .val = 0 },
    { .offset = 0x108c2,  .val = 0 },
    { .offset = 0x1108c2, .val = 0 },
    { .offset = 0x2108c2, .val = 0 },
    { .offset = 0x3108c2, .val = 0 },
    { .offset = 0x100c3,  .val = 0 },
    { .offset = 0x1100c3, .val = 0 },
    { .offset = 0x2100c3, .val = 0 },
    { .offset = 0x3100c3, .val = 0 },
    { .offset = 0x101c3,  .val = 0 },
    { .offset = 0x1101c3, .val = 0 },
    { .offset = 0x2101c3, .val = 0 },
    { .offset = 0x3101c3, .val = 0 },
    { .offset = 0x102c3,  .val = 0 },
    { .offset = 0x1102c3, .val = 0 },
    { .offset = 0x2102c3, .val = 0 },
    { .offset = 0x3102c3, .val = 0 },
    { .offset = 0x103c3,  .val = 0 },
    { .offset = 0x1103c3, .val = 0 },
    { .offset = 0x2103c3, .val = 0 },
    { .offset = 0x3103c3, .val = 0 },
    { .offset = 0x104c3,  .val = 0 },
    { .offset = 0x1104c3, .val = 0 },
    { .offset = 0x2104c3, .val = 0 },
    { .offset = 0x3104c3, .val = 0 },
    { .offset = 0x105c3,  .val = 0 },
    { .offset = 0x1105c3, .val = 0 },
    { .offset = 0x2105c3, .val = 0 },
    { .offset = 0x3105c3, .val = 0 },
    { .offset = 0x106c3,  .val = 0 },
    { .offset = 0x1106c3, .val = 0 },
    { .offset = 0x2106c3, .val = 0 },
    { .offset = 0x3106c3, .val = 0 },
    { .offset = 0x107c3,  .val = 0 },
    { .offset = 0x1107c3, .val = 0 },
    { .offset = 0x2107c3, .val = 0 },
    { .offset = 0x3107c3, .val = 0 },
    { .offset = 0x108c3,  .val = 0 },
    { .offset = 0x1108c3, .val = 0 },
    { .offset = 0x2108c3, .val = 0 },
    { .offset = 0x3108c3, .val = 0 },
    { .offset = 0x110c0,  .val = 0 },
    { .offset = 0x1110c0, .val = 0 },
    { .offset = 0x2110c0, .val = 0 },
    { .offset = 0x3110c0, .val = 0 },
    { .offset = 0x111c0,  .val = 0 },
    { .offset = 0x1111c0, .val = 0 },
    { .offset = 0x2111c0, .val = 0 },
    { .offset = 0x3111c0, .val = 0 },
    { .offset = 0x112c0,  .val = 0 },
    { .offset = 0x1112c0, .val = 0 },
    { .offset = 0x2112c0, .val = 0 },
    { .offset = 0x3112c0, .val = 0 },
    { .offset = 0x113c0,  .val = 0 },
    { .offset = 0x1113c0, .val = 0 },
    { .offset = 0x2113c0, .val = 0 },
    { .offset = 0x3113c0, .val = 0 },
    { .offset = 0x114c0,  .val = 0 },
    { .offset = 0x1114c0, .val = 0 },
    { .offset = 0x2114c0, .val = 0 },
    { .offset = 0x3114c0, .val = 0 },
    { .offset = 0x115c0,  .val = 0 },
    { .offset = 0x1115c0, .val = 0 },
    { .offset = 0x2115c0, .val = 0 },
    { .offset = 0x3115c0, .val = 0 },
    { .offset = 0x116c0,  .val = 0 },
    { .offset = 0x1116c0, .val = 0 },
    { .offset = 0x2116c0, .val = 0 },
    { .offset = 0x3116c0, .val = 0 },
    { .offset = 0x117c0,  .val = 0 },
    { .offset = 0x1117c0, .val = 0 },
    { .offset = 0x2117c0, .val = 0 },
    { .offset = 0x3117c0, .val = 0 },
    { .offset = 0x118c0,  .val = 0 },
    { .offset = 0x1118c0, .val = 0 },
    { .offset = 0x2118c0, .val = 0 },
    { .offset = 0x3118c0, .val = 0 },
    { .offset = 0x110c1,  .val = 0 },
    { .offset = 0x1110c1, .val = 0 },
    { .offset = 0x2110c1, .val = 0 },
    { .offset = 0x3110c1, .val = 0 },
    { .offset = 0x111c1,  .val = 0 },
    { .offset = 0x1111c1, .val = 0 },
    { .offset = 0x2111c1, .val = 0 },
    { .offset = 0x3111c1, .val = 0 },
    { .offset = 0x112c1,  .val = 0 },
    { .offset = 0x1112c1, .val = 0 },
    { .offset = 0x2112c1, .val = 0 },
    { .offset = 0x3112c1, .val = 0 },
    { .offset = 0x113c1,  .val = 0 },
    { .offset = 0x1113c1, .val = 0 },
    { .offset = 0x2113c1, .val = 0 },
    { .offset = 0x3113c1, .val = 0 },
    { .offset = 0x114c1,  .val = 0 },
    { .offset = 0x1114c1, .val = 0 },
    { .offset = 0x2114c1, .val = 0 },
    { .offset = 0x3114c1, .val = 0 },
    { .offset = 0x115c1,  .val = 0 },
    { .offset = 0x1115c1, .val = 0 },
    { .offset = 0x2115c1, .val = 0 },
    { .offset = 0x3115c1, .val = 0 },
    { .offset = 0x116c1,  .val = 0 },
    { .offset = 0x1116c1, .val = 0 },
    { .offset = 0x2116c1, .val = 0 },
    { .offset = 0x3116c1, .val = 0 },
    { .offset = 0x117c1,  .val = 0 },
    { .offset = 0x1117c1, .val = 0 },
    { .offset = 0x2117c1, .val = 0 },
    { .offset = 0x3117c1, .val = 0 },
    { .offset = 0x118c1,  .val = 0 },
    { .offset = 0x1118c1, .val = 0 },
    { .offset = 0x2118c1, .val = 0 },
    { .offset = 0x3118c1, .val = 0 },
    { .offset = 0x110c2,  .val = 0 },
    { .offset = 0x1110c2, .val = 0 },
    { .offset = 0x2110c2, .val = 0 },
    { .offset = 0x3110c2, .val = 0 },
    { .offset = 0x111c2,  .val = 0 },
    { .offset = 0x1111c2, .val = 0 },
    { .offset = 0x2111c2, .val = 0 },
    { .offset = 0x3111c2, .val = 0 },
    { .offset = 0x112c2,  .val = 0 },
    { .offset = 0x1112c2, .val = 0 },
    { .offset = 0x2112c2, .val = 0 },
    { .offset = 0x3112c2, .val = 0 },
    { .offset = 0x113c2,  .val = 0 },
    { .offset = 0x1113c2, .val = 0 },
    { .offset = 0x2113c2, .val = 0 },
    { .offset = 0x3113c2, .val = 0 },
    { .offset = 0x114c2,  .val = 0 },
    { .offset = 0x1114c2, .val = 0 },
    { .offset = 0x2114c2, .val = 0 },
    { .offset = 0x3114c2, .val = 0 },
    { .offset = 0x115c2,  .val = 0 },
    { .offset = 0x1115c2, .val = 0 },
    { .offset = 0x2115c2, .val = 0 },
    { .offset = 0x3115c2, .val = 0 },
    { .offset = 0x116c2,  .val = 0 },
    { .offset = 0x1116c2, .val = 0 },
    { .offset = 0x2116c2, .val = 0 },
    { .offset = 0x3116c2, .val = 0 },
    { .offset = 0x117c2,  .val = 0 },
    { .offset = 0x1117c2, .val = 0 },
    { .offset = 0x2117c2, .val = 0 },
    { .offset = 0x3117c2, .val = 0 },
    { .offset = 0x118c2,  .val = 0 },
    { .offset = 0x1118c2, .val = 0 },
    { .offset = 0x2118c2, .val = 0 },
    { .offset = 0x3118c2, .val = 0 },
    { .offset = 0x110c3,  .val = 0 },
    { .offset = 0x1110c3, .val = 0 },
    { .offset = 0x2110c3, .val = 0 },
    { .offset = 0x3110c3, .val = 0 },
    { .offset = 0x111c3,  .val = 0 },
    { .offset = 0x1111c3, .val = 0 },
    { .offset = 0x2111c3, .val = 0 },
    { .offset = 0x3111c3, .val = 0 },
    { .offset = 0x112c3,  .val = 0 },
    { .offset = 0x1112c3, .val = 0 },
    { .offset = 0x2112c3, .val = 0 },
    { .offset = 0x3112c3, .val = 0 },
    { .offset = 0x113c3,  .val = 0 },
    { .offset = 0x1113c3, .val = 0 },
    { .offset = 0x2113c3, .val = 0 },
    { .offset = 0x3113c3, .val = 0 },
    { .offset = 0x114c3,  .val = 0 },
    { .offset = 0x1114c3, .val = 0 },
    { .offset = 0x2114c3, .val = 0 },
    { .offset = 0x3114c3, .val = 0 },
    { .offset = 0x115c3,  .val = 0 },
    { .offset = 0x1115c3, .val = 0 },
    { .offset = 0x2115c3, .val = 0 },
    { .offset = 0x3115c3, .val = 0 },
    { .offset = 0x116c3,  .val = 0 },
    { .offset = 0x1116c3, .val = 0 },
    { .offset = 0x2116c3, .val = 0 },
    { .offset = 0x3116c3, .val = 0 },
    { .offset = 0x117c3,  .val = 0 },
    { .offset = 0x1117c3, .val = 0 },
    { .offset = 0x2117c3, .val = 0 },
    { .offset = 0x3117c3, .val = 0 },
    { .offset = 0x118c3,  .val = 0 },
    { .offset = 0x1118c3, .val = 0 },
    { .offset = 0x2118c3, .val = 0 },
    { .offset = 0x3118c3, .val = 0 },
    { .offset = 0x120c0,  .val = 0 },
    { .offset = 0x1120c0, .val = 0 },
    { .offset = 0x2120c0, .val = 0 },
    { .offset = 0x3120c0, .val = 0 },
    { .offset = 0x121c0,  .val = 0 },
    { .offset = 0x1121c0, .val = 0 },
    { .offset = 0x2121c0, .val = 0 },
    { .offset = 0x3121c0, .val = 0 },
    { .offset = 0x122c0,  .val = 0 },
    { .offset = 0x1122c0, .val = 0 },
    { .offset = 0x2122c0, .val = 0 },
    { .offset = 0x3122c0, .val = 0 },
    { .offset = 0x123c0,  .val = 0 },
    { .offset = 0x1123c0, .val = 0 },
    { .offset = 0x2123c0, .val = 0 },
    { .offset = 0x3123c0, .val = 0 },
    { .offset = 0x124c0,  .val = 0 },
    { .offset = 0x1124c0, .val = 0 },
    { .offset = 0x2124c0, .val = 0 },
    { .offset = 0x3124c0, .val = 0 },
    { .offset = 0x125c0,  .val = 0 },
    { .offset = 0x1125c0, .val = 0 },
    { .offset = 0x2125c0, .val = 0 },
    { .offset = 0x3125c0, .val = 0 },
    { .offset = 0x126c0,  .val = 0 },
    { .offset = 0x1126c0, .val = 0 },
    { .offset = 0x2126c0, .val = 0 },
    { .offset = 0x3126c0, .val = 0 },
    { .offset = 0x127c0,  .val = 0 },
    { .offset = 0x1127c0, .val = 0 },
    { .offset = 0x2127c0, .val = 0 },
    { .offset = 0x3127c0, .val = 0 },
    { .offset = 0x128c0,  .val = 0 },
    { .offset = 0x1128c0, .val = 0 },
    { .offset = 0x2128c0, .val = 0 },
    { .offset = 0x3128c0, .val = 0 },
    { .offset = 0x120c1,  .val = 0 },
    { .offset = 0x1120c1, .val = 0 },
    { .offset = 0x2120c1, .val = 0 },
    { .offset = 0x3120c1, .val = 0 },
    { .offset = 0x121c1,  .val = 0 },
    { .offset = 0x1121c1, .val = 0 },
    { .offset = 0x2121c1, .val = 0 },
    { .offset = 0x3121c1, .val = 0 },
    { .offset = 0x122c1,  .val = 0 },
    { .offset = 0x1122c1, .val = 0 },
    { .offset = 0x2122c1, .val = 0 },
    { .offset = 0x3122c1, .val = 0 },
    { .offset = 0x123c1,  .val = 0 },
    { .offset = 0x1123c1, .val = 0 },
    { .offset = 0x2123c1, .val = 0 },
    { .offset = 0x3123c1, .val = 0 },
    { .offset = 0x124c1,  .val = 0 },
    { .offset = 0x1124c1, .val = 0 },
    { .offset = 0x2124c1, .val = 0 },
    { .offset = 0x3124c1, .val = 0 },
    { .offset = 0x125c1,  .val = 0 },
    { .offset = 0x1125c1, .val = 0 },
    { .offset = 0x2125c1, .val = 0 },
    { .offset = 0x3125c1, .val = 0 },
    { .offset = 0x126c1,  .val = 0 },
    { .offset = 0x1126c1, .val = 0 },
    { .offset = 0x2126c1, .val = 0 },
    { .offset = 0x3126c1, .val = 0 },
    { .offset = 0x127c1,  .val = 0 },
    { .offset = 0x1127c1, .val = 0 },
    { .offset = 0x2127c1, .val = 0 },
    { .offset = 0x3127c1, .val = 0 },
    { .offset = 0x128c1,  .val = 0 },
    { .offset = 0x1128c1, .val = 0 },
    { .offset = 0x2128c1, .val = 0 },
    { .offset = 0x3128c1, .val = 0 },
    { .offset = 0x120c2,  .val = 0 },
    { .offset = 0x1120c2, .val = 0 },
    { .offset = 0x2120c2, .val = 0 },
    { .offset = 0x3120c2, .val = 0 },
    { .offset = 0x121c2,  .val = 0 },
    { .offset = 0x1121c2, .val = 0 },
    { .offset = 0x2121c2, .val = 0 },
    { .offset = 0x3121c2, .val = 0 },
    { .offset = 0x122c2,  .val = 0 },
    { .offset = 0x1122c2, .val = 0 },
    { .offset = 0x2122c2, .val = 0 },
    { .offset = 0x3122c2, .val = 0 },
    { .offset = 0x123c2,  .val = 0 },
    { .offset = 0x1123c2, .val = 0 },
    { .offset = 0x2123c2, .val = 0 },
    { .offset = 0x3123c2, .val = 0 },
    { .offset = 0x124c2,  .val = 0 },
    { .offset = 0x1124c2, .val = 0 },
    { .offset = 0x2124c2, .val = 0 },
    { .offset = 0x3124c2, .val = 0 },
    { .offset = 0x125c2,  .val = 0 },
    { .offset = 0x1125c2, .val = 0 },
    { .offset = 0x2125c2, .val = 0 },
    { .offset = 0x3125c2, .val = 0 },
    { .offset = 0x126c2,  .val = 0 },
    { .offset = 0x1126c2, .val = 0 },
    { .offset = 0x2126c2, .val = 0 },
    { .offset = 0x3126c2, .val = 0 },
    { .offset = 0x127c2,  .val = 0 },
    { .offset = 0x1127c2, .val = 0 },
    { .offset = 0x2127c2, .val = 0 },
    { .offset = 0x3127c2, .val = 0 },
    { .offset = 0x128c2,  .val = 0 },
    { .offset = 0x1128c2, .val = 0 },
    { .offset = 0x2128c2, .val = 0 },
    { .offset = 0x3128c2, .val = 0 },
    { .offset = 0x120c3,  .val = 0 },
    { .offset = 0x1120c3, .val = 0 },
    { .offset = 0x2120c3, .val = 0 },
    { .offset = 0x3120c3, .val = 0 },
    { .offset = 0x121c3,  .val = 0 },
    { .offset = 0x1121c3, .val = 0 },
    { .offset = 0x2121c3, .val = 0 },
    { .offset = 0x3121c3, .val = 0 },
    { .offset = 0x122c3,  .val = 0 },
    { .offset = 0x1122c3, .val = 0 },
    { .offset = 0x2122c3, .val = 0 },
    { .offset = 0x3122c3, .val = 0 },
    { .offset = 0x123c3,  .val = 0 },
    { .offset = 0x1123c3, .val = 0 },
    { .offset = 0x2123c3, .val = 0 },
    { .offset = 0x3123c3, .val = 0 },
    { .offset = 0x124c3,  .val = 0 },
    { .offset = 0x1124c3, .val = 0 },
    { .offset = 0x2124c3, .val = 0 },
    { .offset = 0x3124c3, .val = 0 },
    { .offset = 0x125c3,  .val = 0 },
    { .offset = 0x1125c3, .val = 0 },
    { .offset = 0x2125c3, .val = 0 },
    { .offset = 0x3125c3, .val = 0 },
    { .offset = 0x126c3,  .val = 0 },
    { .offset = 0x1126c3, .val = 0 },
    { .offset = 0x2126c3, .val = 0 },
    { .offset = 0x3126c3, .val = 0 },
    { .offset = 0x127c3,  .val = 0 },
    { .offset = 0x1127c3, .val = 0 },
    { .offset = 0x2127c3, .val = 0 },
    { .offset = 0x3127c3, .val = 0 },
    { .offset = 0x128c3,  .val = 0 },
    { .offset = 0x1128c3, .val = 0 },
    { .offset = 0x2128c3, .val = 0 },
    { .offset = 0x3128c3, .val = 0 },
    { .offset = 0x130c0,  .val = 0 },
    { .offset = 0x1130c0, .val = 0 },
    { .offset = 0x2130c0, .val = 0 },
    { .offset = 0x3130c0, .val = 0 },
    { .offset = 0x131c0,  .val = 0 },
    { .offset = 0x1131c0, .val = 0 },
    { .offset = 0x2131c0, .val = 0 },
    { .offset = 0x3131c0, .val = 0 },
    { .offset = 0x132c0,  .val = 0 },
    { .offset = 0x1132c0, .val = 0 },
    { .offset = 0x2132c0, .val = 0 },
    { .offset = 0x3132c0, .val = 0 },
    { .offset = 0x133c0,  .val = 0 },
    { .offset = 0x1133c0, .val = 0 },
    { .offset = 0x2133c0, .val = 0 },
    { .offset = 0x3133c0, .val = 0 },
    { .offset = 0x134c0,  .val = 0 },
    { .offset = 0x1134c0, .val = 0 },
    { .offset = 0x2134c0, .val = 0 },
    { .offset = 0x3134c0, .val = 0 },
    { .offset = 0x135c0,  .val = 0 },
    { .offset = 0x1135c0, .val = 0 },
    { .offset = 0x2135c0, .val = 0 },
    { .offset = 0x3135c0, .val = 0 },
    { .offset = 0x136c0,  .val = 0 },
    { .offset = 0x1136c0, .val = 0 },
    { .offset = 0x2136c0, .val = 0 },
    { .offset = 0x3136c0, .val = 0 },
    { .offset = 0x137c0,  .val = 0 },
    { .offset = 0x1137c0, .val = 0 },
    { .offset = 0x2137c0, .val = 0 },
    { .offset = 0x3137c0, .val = 0 },
    { .offset = 0x138c0,  .val = 0 },
    { .offset = 0x1138c0, .val = 0 },
    { .offset = 0x2138c0, .val = 0 },
    { .offset = 0x3138c0, .val = 0 },
    { .offset = 0x130c1,  .val = 0 },
    { .offset = 0x1130c1, .val = 0 },
    { .offset = 0x2130c1, .val = 0 },
    { .offset = 0x3130c1, .val = 0 },
    { .offset = 0x131c1,  .val = 0 },
    { .offset = 0x1131c1, .val = 0 },
    { .offset = 0x2131c1, .val = 0 },
    { .offset = 0x3131c1, .val = 0 },
    { .offset = 0x132c1,  .val = 0 },
    { .offset = 0x1132c1, .val = 0 },
    { .offset = 0x2132c1, .val = 0 },
    { .offset = 0x3132c1, .val = 0 },
    { .offset = 0x133c1,  .val = 0 },
    { .offset = 0x1133c1, .val = 0 },
    { .offset = 0x2133c1, .val = 0 },
    { .offset = 0x3133c1, .val = 0 },
    { .offset = 0x134c1,  .val = 0 },
    { .offset = 0x1134c1, .val = 0 },
    { .offset = 0x2134c1, .val = 0 },
    { .offset = 0x3134c1, .val = 0 },
    { .offset = 0x135c1,  .val = 0 },
    { .offset = 0x1135c1, .val = 0 },
    { .offset = 0x2135c1, .val = 0 },
    { .offset = 0x3135c1, .val = 0 },
    { .offset = 0x136c1,  .val = 0 },
    { .offset = 0x1136c1, .val = 0 },
    { .offset = 0x2136c1, .val = 0 },
    { .offset = 0x3136c1, .val = 0 },
    { .offset = 0x137c1,  .val = 0 },
    { .offset = 0x1137c1, .val = 0 },
    { .offset = 0x2137c1, .val = 0 },
    { .offset = 0x3137c1, .val = 0 },
    { .offset = 0x138c1,  .val = 0 },
    { .offset = 0x1138c1, .val = 0 },
    { .offset = 0x2138c1, .val = 0 },
    { .offset = 0x3138c1, .val = 0 },
    { .offset = 0x130c2,  .val = 0 },
    { .offset = 0x1130c2, .val = 0 },
    { .offset = 0x2130c2, .val = 0 },
    { .offset = 0x3130c2, .val = 0 },
    { .offset = 0x131c2,  .val = 0 },
    { .offset = 0x1131c2, .val = 0 },
    { .offset = 0x2131c2, .val = 0 },
    { .offset = 0x3131c2, .val = 0 },
    { .offset = 0x132c2,  .val = 0 },
    { .offset = 0x1132c2, .val = 0 },
    { .offset = 0x2132c2, .val = 0 },
    { .offset = 0x3132c2, .val = 0 },
    { .offset = 0x133c2,  .val = 0 },
    { .offset = 0x1133c2, .val = 0 },
    { .offset = 0x2133c2, .val = 0 },
    { .offset = 0x3133c2, .val = 0 },
    { .offset = 0x134c2,  .val = 0 },
    { .offset = 0x1134c2, .val = 0 },
    { .offset = 0x2134c2, .val = 0 },
    { .offset = 0x3134c2, .val = 0 },
    { .offset = 0x135c2,  .val = 0 },
    { .offset = 0x1135c2, .val = 0 },
    { .offset = 0x2135c2, .val = 0 },
    { .offset = 0x3135c2, .val = 0 },
    { .offset = 0x136c2,  .val = 0 },
    { .offset = 0x1136c2, .val = 0 },
    { .offset = 0x2136c2, .val = 0 },
    { .offset = 0x3136c2, .val = 0 },
    { .offset = 0x137c2,  .val = 0 },
    { .offset = 0x1137c2, .val = 0 },
    { .offset = 0x2137c2, .val = 0 },
    { .offset = 0x3137c2, .val = 0 },
    { .offset = 0x138c2,  .val = 0 },
    { .offset = 0x1138c2, .val = 0 },
    { .offset = 0x2138c2, .val = 0 },
    { .offset = 0x3138c2, .val = 0 },
    { .offset = 0x130c3,  .val = 0 },
    { .offset = 0x1130c3, .val = 0 },
    { .offset = 0x2130c3, .val = 0 },
    { .offset = 0x3130c3, .val = 0 },
    { .offset = 0x131c3,  .val = 0 },
    { .offset = 0x1131c3, .val = 0 },
    { .offset = 0x2131c3, .val = 0 },
    { .offset = 0x3131c3, .val = 0 },
    { .offset = 0x132c3,  .val = 0 },
    { .offset = 0x1132c3, .val = 0 },
    { .offset = 0x2132c3, .val = 0 },
    { .offset = 0x3132c3, .val = 0 },
    { .offset = 0x133c3,  .val = 0 },
    { .offset = 0x1133c3, .val = 0 },
    { .offset = 0x2133c3, .val = 0 },
    { .offset = 0x3133c3, .val = 0 },
    { .offset = 0x134c3,  .val = 0 },
    { .offset = 0x1134c3, .val = 0 },
    { .offset = 0x2134c3, .val = 0 },
    { .offset = 0x3134c3, .val = 0 },
    { .offset = 0x135c3,  .val = 0 },
    { .offset = 0x1135c3, .val = 0 },
    { .offset = 0x2135c3, .val = 0 },
    { .offset = 0x3135c3, .val = 0 },
    { .offset = 0x136c3,  .val = 0 },
    { .offset = 0x1136c3, .val = 0 },
    { .offset = 0x2136c3, .val = 0 },
    { .offset = 0x3136c3, .val = 0 },
    { .offset = 0x137c3,  .val = 0 },
    { .offset = 0x1137c3, .val = 0 },
    { .offset = 0x2137c3, .val = 0 },
    { .offset = 0x3137c3, .val = 0 },
    { .offset = 0x138c3,  .val = 0 },
    { .offset = 0x1138c3, .val = 0 },
    { .offset = 0x2138c3, .val = 0 },
    { .offset = 0x3138c3, .val = 0 },
    { .offset = 0x10020,  .val = 0 },
    { .offset = 0x110020, .val = 0 },
    { .offset = 0x210020, .val = 0 },
    { .offset = 0x310020, .val = 0 },
    { .offset = 0x11020,  .val = 0 },
    { .offset = 0x111020, .val = 0 },
    { .offset = 0x211020, .val = 0 },
    { .offset = 0x311020, .val = 0 },
    { .offset = 0x12020,  .val = 0 },
    { .offset = 0x112020, .val = 0 },
    { .offset = 0x212020, .val = 0 },
    { .offset = 0x312020, .val = 0 },
    { .offset = 0x13020,  .val = 0 },
    { .offset = 0x113020, .val = 0 },
    { .offset = 0x213020, .val = 0 },
    { .offset = 0x313020, .val = 0 },
    { .offset = 0x20072,  .val = 0 },
    { .offset = 0x20073,  .val = 0 },
    { .offset = 0x20074,  .val = 0 },
    { .offset = 0x100aa,  .val = 0 },
    { .offset = 0x110aa,  .val = 0 },
    { .offset = 0x120aa,  .val = 0 },
    { .offset = 0x130aa,  .val = 0 },
    { .offset = 0x100ae,  .val = 0 },
    { .offset = 0x1100ae, .val = 0 },
    { .offset = 0x2100ae, .val = 0 },
    { .offset = 0x3100ae, .val = 0 },
    { .offset = 0x100af,  .val = 0 },
    { .offset = 0x1100af, .val = 0 },
    { .offset = 0x2100af, .val = 0 },
    { .offset = 0x3100af, .val = 0 },
    { .offset = 0x110ae,  .val = 0 },
    { .offset = 0x1110ae, .val = 0 },
    { .offset = 0x2110ae, .val = 0 },
    { .offset = 0x3110ae, .val = 0 },
    { .offset = 0x110af,  .val = 0 },
    { .offset = 0x1110af, .val = 0 },
    { .offset = 0x2110af, .val = 0 },
    { .offset = 0x3110af, .val = 0 },
    { .offset = 0x120ae,  .val = 0 },
    { .offset = 0x1120ae, .val = 0 },
    { .offset = 0x2120ae, .val = 0 },
    { .offset = 0x3120ae, .val = 0 },
    { .offset = 0x120af,  .val = 0 },
    { .offset = 0x1120af, .val = 0 },
    { .offset = 0x2120af, .val = 0 },
    { .offset = 0x3120af, .val = 0 },
    { .offset = 0x130ae,  .val = 0 },
    { .offset = 0x1130ae, .val = 0 },
    { .offset = 0x2130ae, .val = 0 },
    { .offset = 0x3130ae, .val = 0 },
    { .offset = 0x130af,  .val = 0 },
    { .offset = 0x1130af, .val = 0 },
    { .offset = 0x2130af, .val = 0 },
    { .offset = 0x3130af, .val = 0 },
    { .offset = 0x20020,  .val = 0 },
    { .offset = 0x120020, .val = 0 },
    { .offset = 0x220020, .val = 0 },
    { .offset = 0x320020, .val = 0 },
    { .offset = 0x100a0,  .val = 0 },
    { .offset = 0x100a1,  .val = 0 },
    { .offset = 0x100a2,  .val = 0 },
    { .offset = 0x100a3,  .val = 0 },
    { .offset = 0x100a4,  .val = 0 },
    { .offset = 0x100a5,  .val = 0 },
    { .offset = 0x100a6,  .val = 0 },
    { .offset = 0x100a7,  .val = 0 },
    { .offset = 0x110a0,  .val = 0 },
    { .offset = 0x110a1,  .val = 0 },
    { .offset = 0x110a2,  .val = 0 },
    { .offset = 0x110a3,  .val = 0 },
    { .offset = 0x110a4,  .val = 0 },
    { .offset = 0x110a5,  .val = 0 },
    { .offset = 0x110a6,  .val = 0 },
    { .offset = 0x110a7,  .val = 0 },
    { .offset = 0x120a0,  .val = 0 },
    { .offset = 0x120a1,  .val = 0 },
    { .offset = 0x120a2,  .val = 0 },
    { .offset = 0x120a3,  .val = 0 },
    { .offset = 0x120a4,  .val = 0 },
    { .offset = 0x120a5,  .val = 0 },
    { .offset = 0x120a6,  .val = 0 },
    { .offset = 0x120a7,  .val = 0 },
    { .offset = 0x130a0,  .val = 0 },
    { .offset = 0x130a1,  .val = 0 },
    { .offset = 0x130a2,  .val = 0 },
    { .offset = 0x130a3,  .val = 0 },
    { .offset = 0x130a4,  .val = 0 },
    { .offset = 0x130a5,  .val = 0 },
    { .offset = 0x130a6,  .val = 0 },
    { .offset = 0x130a7,  .val = 0 },
    { .offset = 0x2007c,  .val = 0 },
    { .offset = 0x12007c, .val = 0 },
    { .offset = 0x22007c, .val = 0 },
    { .offset = 0x32007c, .val = 0 },
    { .offset = 0x2007d,  .val = 0 },
    { .offset = 0x12007d, .val = 0 },
    { .offset = 0x22007d, .val = 0 },
    { .offset = 0x32007d, .val = 0 },
    { .offset = 0x400fd,  .val = 0 },
    { .offset = 0x400c0,  .val = 0 },
    { .offset = 0x90201,  .val = 0 },
    { .offset = 0x190201, .val = 0 },
    { .offset = 0x290201, .val = 0 },
    { .offset = 0x390201, .val = 0 },
    { .offset = 0x90202,  .val = 0 },
    { .offset = 0x190202, .val = 0 },
    { .offset = 0x290202, .val = 0 },
    { .offset = 0x390202, .val = 0 },
    { .offset = 0x90203,  .val = 0 },
    { .offset = 0x190203, .val = 0 },
    { .offset = 0x290203, .val = 0 },
    { .offset = 0x390203, .val = 0 },
    { .offset = 0x90204,  .val = 0 },
    { .offset = 0x190204, .val = 0 },
    { .offset = 0x290204, .val = 0 },
    { .offset = 0x390204, .val = 0 },
    { .offset = 0x90205,  .val = 0 },
    { .offset = 0x190205, .val = 0 },
    { .offset = 0x290205, .val = 0 },
    { .offset = 0x390205, .val = 0 },
    { .offset = 0x90206,  .val = 0 },
    { .offset = 0x190206, .val = 0 },
    { .offset = 0x290206, .val = 0 },
    { .offset = 0x390206, .val = 0 },
    { .offset = 0x90207,  .val = 0 },
    { .offset = 0x190207, .val = 0 },
    { .offset = 0x290207, .val = 0 },
    { .offset = 0x390207, .val = 0 },
    { .offset = 0x90208,  .val = 0 },
    { .offset = 0x190208, .val = 0 },
    { .offset = 0x290208, .val = 0 },
    { .offset = 0x390208, .val = 0 },
    { .offset = 0x10062,  .val = 0 },
    { .offset = 0x10162,  .val = 0 },
    { .offset = 0x10262,  .val = 0 },
    { .offset = 0x10362,  .val = 0 },
    { .offset = 0x10462,  .val = 0 },
    { .offset = 0x10562,  .val = 0 },
    { .offset = 0x10662,  .val = 0 },
    { .offset = 0x10762,  .val = 0 },
    { .offset = 0x10862,  .val = 0 },
    { .offset = 0x11062,  .val = 0 },
    { .offset = 0x11162,  .val = 0 },
    { .offset = 0x11262,  .val = 0 },
    { .offset = 0x11362,  .val = 0 },
    { .offset = 0x11462,  .val = 0 },
    { .offset = 0x11562,  .val = 0 },
    { .offset = 0x11662,  .val = 0 },
    { .offset = 0x11762,  .val = 0 },
    { .offset = 0x11862,  .val = 0 },
    { .offset = 0x12062,  .val = 0 },
    { .offset = 0x12162,  .val = 0 },
    { .offset = 0x12262,  .val = 0 },
    { .offset = 0x12362,  .val = 0 },
    { .offset = 0x12462,  .val = 0 },
    { .offset = 0x12562,  .val = 0 },
    { .offset = 0x12662,  .val = 0 },
    { .offset = 0x12762,  .val = 0 },
    { .offset = 0x12862,  .val = 0 },
    { .offset = 0x13062,  .val = 0 },
    { .offset = 0x13162,  .val = 0 },
    { .offset = 0x13262,  .val = 0 },
    { .offset = 0x13362,  .val = 0 },
    { .offset = 0x13462,  .val = 0 },
    { .offset = 0x13562,  .val = 0 },
    { .offset = 0x13662,  .val = 0 },
    { .offset = 0x13762,  .val = 0 },
    { .offset = 0x13862,  .val = 0 },
    { .offset = 0x20077,  .val = 0 },
    { .offset = 0x10001,  .val = 0 },
    { .offset = 0x10011,  .val = 0 },
    { .offset = 0x10012,  .val = 0 },
    { .offset = 0x10013,  .val = 0 },
    { .offset = 0x10014,  .val = 0 },
    { .offset = 0x10018,  .val = 0 },
    { .offset = 0x11001,  .val = 0 },
    { .offset = 0x11011,  .val = 0 },
    { .offset = 0x11012,  .val = 0 },
    { .offset = 0x11013,  .val = 0 },
    { .offset = 0x11014,  .val = 0 },
    { .offset = 0x11018,  .val = 0 },
    { .offset = 0x12001,  .val = 0 },
    { .offset = 0x12011,  .val = 0 },
    { .offset = 0x12012,  .val = 0 },
    { .offset = 0x12013,  .val = 0 },
    { .offset = 0x12014,  .val = 0 },
    { .offset = 0x12018,  .val = 0 },
    { .offset = 0x13001,  .val = 0 },
    { .offset = 0x13011,  .val = 0 },
    { .offset = 0x13012,  .val = 0 },
    { .offset = 0x13013,  .val = 0 },
    { .offset = 0x13014,  .val = 0 },
    { .offset = 0x13018,  .val = 0 },
    { .offset = 0x1008c,  .val = 0 },
    { .offset = 0x11008c, .val = 0 },
    { .offset = 0x21008c, .val = 0 },
    { .offset = 0x31008c, .val = 0 },
    { .offset = 0x1018c,  .val = 0 },
    { .offset = 0x11018c, .val = 0 },
    { .offset = 0x21018c, .val = 0 },
    { .offset = 0x31018c, .val = 0 },
    { .offset = 0x1008d,  .val = 0 },
    { .offset = 0x11008d, .val = 0 },
    { .offset = 0x21008d, .val = 0 },
    { .offset = 0x31008d, .val = 0 },
    { .offset = 0x1018d,  .val = 0 },
    { .offset = 0x11018d, .val = 0 },
    { .offset = 0x21018d, .val = 0 },
    { .offset = 0x31018d, .val = 0 },
    { .offset = 0x1008e,  .val = 0 },
    { .offset = 0x11008e, .val = 0 },
    { .offset = 0x21008e, .val = 0 },
    { .offset = 0x31008e, .val = 0 },
    { .offset = 0x1018e,  .val = 0 },
    { .offset = 0x11018e, .val = 0 },
    { .offset = 0x21018e, .val = 0 },
    { .offset = 0x31018e, .val = 0 },
    { .offset = 0x1008f,  .val = 0 },
    { .offset = 0x11008f, .val = 0 },
    { .offset = 0x21008f, .val = 0 },
    { .offset = 0x31008f, .val = 0 },
    { .offset = 0x1018f,  .val = 0 },
    { .offset = 0x11018f, .val = 0 },
    { .offset = 0x21018f, .val = 0 },
    { .offset = 0x31018f, .val = 0 },
    { .offset = 0x1108c,  .val = 0 },
    { .offset = 0x11108c, .val = 0 },
    { .offset = 0x21108c, .val = 0 },
    { .offset = 0x31108c, .val = 0 },
    { .offset = 0x1118c,  .val = 0 },
    { .offset = 0x11118c, .val = 0 },
    { .offset = 0x21118c, .val = 0 },
    { .offset = 0x31118c, .val = 0 },
    { .offset = 0x1108d,  .val = 0 },
    { .offset = 0x11108d, .val = 0 },
    { .offset = 0x21108d, .val = 0 },
    { .offset = 0x31108d, .val = 0 },
    { .offset = 0x1118d,  .val = 0 },
    { .offset = 0x11118d, .val = 0 },
    { .offset = 0x21118d, .val = 0 },
    { .offset = 0x31118d, .val = 0 },
    { .offset = 0x1108e,  .val = 0 },
    { .offset = 0x11108e, .val = 0 },
    { .offset = 0x21108e, .val = 0 },
    { .offset = 0x31108e, .val = 0 },
    { .offset = 0x1118e,  .val = 0 },
    { .offset = 0x11118e, .val = 0 },
    { .offset = 0x21118e, .val = 0 },
    { .offset = 0x31118e, .val = 0 },
    { .offset = 0x1108f,  .val = 0 },
    { .offset = 0x11108f, .val = 0 },
    { .offset = 0x21108f, .val = 0 },
    { .offset = 0x31108f, .val = 0 },
    { .offset = 0x1118f,  .val = 0 },
    { .offset = 0x11118f, .val = 0 },
    { .offset = 0x21118f, .val = 0 },
    { .offset = 0x31118f, .val = 0 },
    { .offset = 0x1208c,  .val = 0 },
    { .offset = 0x11208c, .val = 0 },
    { .offset = 0x21208c, .val = 0 },
    { .offset = 0x31208c, .val = 0 },
    { .offset = 0x1218c,  .val = 0 },
    { .offset = 0x11218c, .val = 0 },
    { .offset = 0x21218c, .val = 0 },
    { .offset = 0x31218c, .val = 0 },
    { .offset = 0x1208d,  .val = 0 },
    { .offset = 0x11208d, .val = 0 },
    { .offset = 0x21208d, .val = 0 },
    { .offset = 0x31208d, .val = 0 },
    { .offset = 0x1218d,  .val = 0 },
    { .offset = 0x11218d, .val = 0 },
    { .offset = 0x21218d, .val = 0 },
    { .offset = 0x31218d, .val = 0 },
    { .offset = 0x1208e,  .val = 0 },
    { .offset = 0x11208e, .val = 0 },
    { .offset = 0x21208e, .val = 0 },
    { .offset = 0x31208e, .val = 0 },
    { .offset = 0x1218e,  .val = 0 },
    { .offset = 0x11218e, .val = 0 },
    { .offset = 0x21218e, .val = 0 },
    { .offset = 0x31218e, .val = 0 },
    { .offset = 0x1208f,  .val = 0 },
    { .offset = 0x11208f, .val = 0 },
    { .offset = 0x21208f, .val = 0 },
    { .offset = 0x31208f, .val = 0 },
    { .offset = 0x1218f,  .val = 0 },
    { .offset = 0x11218f, .val = 0 },
    { .offset = 0x21218f, .val = 0 },
    { .offset = 0x31218f, .val = 0 },
    { .offset = 0x1308c,  .val = 0 },
    { .offset = 0x11308c, .val = 0 },
    { .offset = 0x21308c, .val = 0 },
    { .offset = 0x31308c, .val = 0 },
    { .offset = 0x1318c,  .val = 0 },
    { .offset = 0x11318c, .val = 0 },
    { .offset = 0x21318c, .val = 0 },
    { .offset = 0x31318c, .val = 0 },
    { .offset = 0x1308d,  .val = 0 },
    { .offset = 0x11308d, .val = 0 },
    { .offset = 0x21308d, .val = 0 },
    { .offset = 0x31308d, .val = 0 },
    { .offset = 0x1318d,  .val = 0 },
    { .offset = 0x11318d, .val = 0 },
    { .offset = 0x21318d, .val = 0 },
    { .offset = 0x31318d, .val = 0 },
    { .offset = 0x1308e,  .val = 0 },
    { .offset = 0x11308e, .val = 0 },
    { .offset = 0x21308e, .val = 0 },
    { .offset = 0x31308e, .val = 0 },
    { .offset = 0x1318e,  .val = 0 },
    { .offset = 0x11318e, .val = 0 },
    { .offset = 0x21318e, .val = 0 },
    { .offset = 0x31318e, .val = 0 },
    { .offset = 0x1308f,  .val = 0 },
    { .offset = 0x11308f, .val = 0 },
    { .offset = 0x21308f, .val = 0 },
    { .offset = 0x31308f, .val = 0 },
    { .offset = 0x1318f,  .val = 0 },
    { .offset = 0x11318f, .val = 0 },
    { .offset = 0x21318f, .val = 0 },
    { .offset = 0x31318f, .val = 0 },
    { .offset = 0x10040,  .val = 0 },
    { .offset = 0x10140,  .val = 0 },
    { .offset = 0x10240,  .val = 0 },
    { .offset = 0x10340,  .val = 0 },
    { .offset = 0x10440,  .val = 0 },
    { .offset = 0x10540,  .val = 0 },
    { .offset = 0x10640,  .val = 0 },
    { .offset = 0x10740,  .val = 0 },
    { .offset = 0x10840,  .val = 0 },
    { .offset = 0x10030,  .val = 0 },
    { .offset = 0x10130,  .val = 0 },
    { .offset = 0x10230,  .val = 0 },
    { .offset = 0x10330,  .val = 0 },
    { .offset = 0x10430,  .val = 0 },
    { .offset = 0x10530,  .val = 0 },
    { .offset = 0x10630,  .val = 0 },
    { .offset = 0x10730,  .val = 0 },
    { .offset = 0x10830,  .val = 0 },
    { .offset = 0x11040,  .val = 0 },
    { .offset = 0x11140,  .val = 0 },
    { .offset = 0x11240,  .val = 0 },
    { .offset = 0x11340,  .val = 0 },
    { .offset = 0x11440,  .val = 0 },
    { .offset = 0x11540,  .val = 0 },
    { .offset = 0x11640,  .val = 0 },
    { .offset = 0x11740,  .val = 0 },
    { .offset = 0x11840,  .val = 0 },
    { .offset = 0x11030,  .val = 0 },
    { .offset = 0x11130,  .val = 0 },
    { .offset = 0x11230,  .val = 0 },
    { .offset = 0x11330,  .val = 0 },
    { .offset = 0x11430,  .val = 0 },
    { .offset = 0x11530,  .val = 0 },
    { .offset = 0x11630,  .val = 0 },
    { .offset = 0x11730,  .val = 0 },
    { .offset = 0x11830,  .val = 0 },
    { .offset = 0x12040,  .val = 0 },
    { .offset = 0x12140,  .val = 0 },
    { .offset = 0x12240,  .val = 0 },
    { .offset = 0x12340,  .val = 0 },
    { .offset = 0x12440,  .val = 0 },
    { .offset = 0x12540,  .val = 0 },
    { .offset = 0x12640,  .val = 0 },
    { .offset = 0x12740,  .val = 0 },
    { .offset = 0x12840,  .val = 0 },
    { .offset = 0x12030,  .val = 0 },
    { .offset = 0x12130,  .val = 0 },
    { .offset = 0x12230,  .val = 0 },
    { .offset = 0x12330,  .val = 0 },
    { .offset = 0x12430,  .val = 0 },
    { .offset = 0x12530,  .val = 0 },
    { .offset = 0x12630,  .val = 0 },
    { .offset = 0x12730,  .val = 0 },
    { .offset = 0x12830,  .val = 0 },
    { .offset = 0x13040,  .val = 0 },
    { .offset = 0x13140,  .val = 0 },
    { .offset = 0x13240,  .val = 0 },
    { .offset = 0x13340,  .val = 0 },
    { .offset = 0x13440,  .val = 0 },
    { .offset = 0x13540,  .val = 0 },
    { .offset = 0x13640,  .val = 0 },
    { .offset = 0x13740,  .val = 0 },
    { .offset = 0x13840,  .val = 0 },
    { .offset = 0x13030,  .val = 0 },
    { .offset = 0x13130,  .val = 0 },
    { .offset = 0x13230,  .val = 0 },
    { .offset = 0x13330,  .val = 0 },
    { .offset = 0x13430,  .val = 0 },
    { .offset = 0x13530,  .val = 0 },
    { .offset = 0x13630,  .val = 0 },
    { .offset = 0x13730,  .val = 0 },
    { .offset = 0x13830,  .val = 0 },
};

static struct ddrphy_trained_csr data_before_restore[] = {
    { .offset =0xd0000, .val=0x0 },
    { .offset =0x20110, .val=0x02  },
    { .offset =0x20111, .val=0x03  },
    { .offset =0x20112, .val=0x04  },
    { .offset =0x20113, .val=0x05  },
    { .offset =0x20114, .val=0x00  },
    { .offset =0x20115, .val=0x01  },
    { .offset =0x1005f, .val=0x1ff },
    { .offset =0x1015f, .val=0x1ff },
    { .offset =0x1105f, .val=0x1ff },
    { .offset =0x1115f, .val=0x1ff },
    { .offset =0x1205f, .val=0x1ff },
    { .offset =0x1215f, .val=0x1ff },
    { .offset =0x1305f, .val=0x1ff },
    { .offset =0x1315f, .val=0x1ff },
    { .offset =0x55, .val=0x1ff },
    { .offset =0x1055, .val=0x1ff },
    { .offset =0x2055, .val=0x1ff },
    { .offset =0x3055, .val=0x1ff },
    { .offset =0x4055, .val=0x1ff },
    { .offset =0x5055, .val=0x1ff },
    { .offset =0x6055, .val=0x1ff },
    { .offset =0x7055, .val=0x1ff },
    { .offset =0x8055, .val=0x1ff },
    { .offset =0x9055, .val=0x1ff },
    { .offset =0x200c5, .val=0x19 },
    { .offset =0x2002e, .val=0x2 },
    { .offset =0x90204, .val=0x0 },
    { .offset =0x20024, .val=0xab },
    { .offset =0x2003a, .val=0x0 },
    { .offset =0x20056, .val=0x3 },
    { .offset =0x1004d, .val=0xe00 },
    { .offset =0x1014d, .val=0xe00 },
    { .offset =0x1104d, .val=0xe00 },
    { .offset =0x1114d, .val=0xe00 },
    { .offset =0x1204d, .val=0xe00 },
    { .offset =0x1214d, .val=0xe00 },
    { .offset =0x1304d, .val=0xe00 },
    { .offset =0x1314d, .val=0xe00 },
    { .offset =0x10049, .val=0xe38 },
    { .offset =0x10149, .val=0xe38 },
    { .offset =0x11049, .val=0xe38 },
    { .offset =0x11149, .val=0xe38 },
    { .offset =0x12049, .val=0xe38 },
    { .offset =0x12149, .val=0xe38 },
    { .offset =0x13049, .val=0xe38 },
    { .offset =0x13149, .val=0xe38 },
    { .offset =0x43, .val=0x21 },
    { .offset =0x1043, .val=0x21 },
    { .offset =0x2043, .val=0x21 },
    { .offset =0x3043, .val=0x21 },
    { .offset =0x4043, .val=0x21 },
    { .offset =0x5043, .val=0x21 },
    { .offset =0x6043, .val=0x21 },
    { .offset =0x7043, .val=0x21 },
    { .offset =0x8043, .val=0x21 },
    { .offset =0x9043, .val=0x21 },
    { .offset =0x20018, .val=0x3 },
    { .offset =0x20075, .val=0x4 },
    { .offset =0x20050, .val=0x0 },
    { .offset =0x20008, .val=0x320 },
    { .offset =0x20088, .val=0x9 },
    { .offset =0x200b2, .val=0x19c },
    { .offset =0x10043, .val=0x5a1 },
    { .offset =0x10143, .val=0x5a1 },
    { .offset =0x11043, .val=0x5a1 },
    { .offset =0x11143, .val=0x5a1 },
    { .offset =0x12043, .val=0x5a1 },
    { .offset =0x12143, .val=0x5a1 },
    { .offset =0x13043, .val=0x5a1 },
    { .offset =0x13143, .val=0x5a1 },
    { .offset =0x200fa, .val=0x1 },
    { .offset =0x20019, .val=0x1 },
    { .offset =0x200f0, .val=0x0 },
    { .offset =0x200f1, .val=0x0 },
    { .offset =0x200f2, .val=0x4444 },
    { .offset =0x200f3, .val=0x8888 },
    { .offset =0x200f4, .val=0x5555 },
    { .offset =0x200f5, .val=0x0 },
    { .offset =0x200f6, .val=0x0 },
    { .offset =0x200f7, .val=0xf000 },
    { .offset =0x2000b, .val=0x65 },
    { .offset =0x2000c, .val=0xc9 },
    { .offset =0x2000d, .val=0x7d1 },
    { .offset =0x2000e, .val=0x2c },
    { .offset =0x20025, .val=0x0 },
    { .offset =0x2002d, .val=0x0 },
};

static struct ddrphy_trained_csr data_after_restore[] = {
    { .offset =0xd0000, .val=0x0 },
    { .offset =0x90000, .val=0x10 },
    { .offset =0x90001, .val=0x400 },
    { .offset =0x90002, .val=0x10e },
    { .offset =0x90003, .val=0x0 },
    { .offset =0x90004, .val=0x0 },
    { .offset =0x90005, .val=0x8 },
    { .offset =0x90029, .val=0xb },
    { .offset =0x9002a, .val=0x480 },
    { .offset =0x9002b, .val=0x109 },
    { .offset =0x9002c, .val=0x8 },
    { .offset =0x9002d, .val=0x448 },
    { .offset =0x9002e, .val=0x139 },
    { .offset =0x9002f, .val=0x8 },
    { .offset =0x90030, .val=0x478 },
    { .offset =0x90031, .val=0x109 },
    { .offset =0x90032, .val=0x0 },
    { .offset =0x90033, .val=0xe8 },
    { .offset =0x90034, .val=0x109 },
    { .offset =0x90035, .val=0x2 },
    { .offset =0x90036, .val=0x10 },
    { .offset =0x90037, .val=0x139 },
    { .offset =0x90038, .val=0xf },
    { .offset =0x90039, .val=0x7c0 },
    { .offset =0x9003a, .val=0x139 },
    { .offset =0x9003b, .val=0x44 },
    { .offset =0x9003c, .val=0x630 },
    { .offset =0x9003d, .val=0x159 },
    { .offset =0x9003e, .val=0x14f },
    { .offset =0x9003f, .val=0x630 },
    { .offset =0x90040, .val=0x159 },
    { .offset =0x90041, .val=0x47 },
    { .offset =0x90042, .val=0x630 },
    { .offset =0x90043, .val=0x149 },
    { .offset =0x90044, .val=0x4f },
    { .offset =0x90045, .val=0x630 },
    { .offset =0x90046, .val=0x179 },
    { .offset =0x90047, .val=0x8 },
    { .offset =0x90048, .val=0xe0 },
    { .offset =0x90049, .val=0x109 },
    { .offset =0x9004a, .val=0x0 },
    { .offset =0x9004b, .val=0x7c8 },
    { .offset =0x9004c, .val=0x109 },
    { .offset =0x9004d, .val=0x0 },
    { .offset =0x9004e, .val=0x1 },
    { .offset =0x9004f, .val=0x8 },
    { .offset =0x90050, .val=0x0 },
    { .offset =0x90051, .val=0x45a },
    { .offset =0x90052, .val=0x9 },
    { .offset =0x90053, .val=0x0 },
    { .offset =0x90054, .val=0x448 },
    { .offset =0x90055, .val=0x109 },
    { .offset =0x90056, .val=0x40 },
    { .offset =0x90057, .val=0x630 },
    { .offset =0x90058, .val=0x179 },
    { .offset =0x90059, .val=0x1 },
    { .offset =0x9005a, .val=0x618 },
    { .offset =0x9005b, .val=0x109 },
    { .offset =0x9005c, .val=0x40c0 },
    { .offset =0x9005d, .val=0x630 },
    { .offset =0x9005e, .val=0x149 },
    { .offset =0x9005f, .val=0x8 },
    { .offset =0x90060, .val=0x4 },
    { .offset =0x90061, .val=0x48 },
    { .offset =0x90062, .val=0x4040 },
    { .offset =0x90063, .val=0x630 },
    { .offset =0x90064, .val=0x149 },
    { .offset =0x90065, .val=0x0 },
    { .offset =0x90066, .val=0x4 },
    { .offset =0x90067, .val=0x48 },
    { .offset =0x90068, .val=0x40 },
    { .offset =0x90069, .val=0x630 },
    { .offset =0x9006a, .val=0x149 },
    { .offset =0x9006b, .val=0x10 },
    { .offset =0x9006c, .val=0x4 },
    { .offset =0x9006d, .val=0x18 },
    { .offset =0x9006e, .val=0x0 },
    { .offset =0x9006f, .val=0x4 },
    { .offset =0x90070, .val=0x78 },
    { .offset =0x90071, .val=0x549 },
    { .offset =0x90072, .val=0x630 },
    { .offset =0x90073, .val=0x159 },
    { .offset =0x90074, .val=0xd49 },
    { .offset =0x90075, .val=0x630 },
    { .offset =0x90076, .val=0x159 },
    { .offset =0x90077, .val=0x94a },
    { .offset =0x90078, .val=0x630 },
    { .offset =0x90079, .val=0x159 },
    { .offset =0x9007a, .val=0x441 },
    { .offset =0x9007b, .val=0x630 },
    { .offset =0x9007c, .val=0x149 },
    { .offset =0x9007d, .val=0x42 },
    { .offset =0x9007e, .val=0x630 },
    { .offset =0x9007f, .val=0x149 },
    { .offset =0x90080, .val=0x1 },
    { .offset =0x90081, .val=0x630 },
    { .offset =0x90082, .val=0x149 },
    { .offset =0x90083, .val=0x0 },
    { .offset =0x90084, .val=0xe0 },
    { .offset =0x90085, .val=0x109 },
    { .offset =0x90086, .val=0xa },
    { .offset =0x90087, .val=0x10 },
    { .offset =0x90088, .val=0x109 },
    { .offset =0x90089, .val=0x9 },
    { .offset =0x9008a, .val=0x3c0 },
    { .offset =0x9008b, .val=0x149 },
    { .offset =0x9008c, .val=0x9 },
    { .offset =0x9008d, .val=0x3c0 },
    { .offset =0x9008e, .val=0x159 },
    { .offset =0x9008f, .val=0x18 },
    { .offset =0x90090, .val=0x10 },
    { .offset =0x90091, .val=0x109 },
    { .offset =0x90092, .val=0x0 },
    { .offset =0x90093, .val=0x3c0 },
    { .offset =0x90094, .val=0x109 },
    { .offset =0x90095, .val=0x18 },
    { .offset =0x90096, .val=0x4 },
    { .offset =0x90097, .val=0x48 },
    { .offset =0x90098, .val=0x18 },
    { .offset =0x90099, .val=0x4 },
    { .offset =0x9009a, .val=0x58 },
    { .offset =0x9009b, .val=0xa },
    { .offset =0x9009c, .val=0x10 },
    { .offset =0x9009d, .val=0x109 },
    { .offset =0x9009e, .val=0x2 },
    { .offset =0x9009f, .val=0x10 },
    { .offset =0x900a0, .val=0x109 },
    { .offset =0x900a1, .val=0x5 },
    { .offset =0x900a2, .val=0x7c0 },
    { .offset =0x900a3, .val=0x109 },
    { .offset =0x900a4, .val=0x10 },
    { .offset =0x900a5, .val=0x10 },
    { .offset =0x900a6, .val=0x109 },
    { .offset =0x40000, .val=0x811 },
    { .offset =0x40020, .val=0x880 },
    { .offset =0x40040, .val=0x0 },
    { .offset =0x40060, .val=0x0 },
    { .offset =0x40001, .val=0x4016 },
    { .offset =0x40021, .val=0x83 },
    { .offset =0x40041, .val=0x4f },
    { .offset =0x40061, .val=0x0 },
    { .offset =0x40002, .val=0x4040 },
    { .offset =0x40022, .val=0x83 },
    { .offset =0x40042, .val=0x51 },
    { .offset =0x40062, .val=0x0 },
    { .offset =0x40003, .val=0x811 },
    { .offset =0x40023, .val=0x880 },
    { .offset =0x40043, .val=0x0 },
    { .offset =0x40063, .val=0x0 },
    { .offset =0x40004, .val=0x720 },
    { .offset =0x40024, .val=0xf },
    { .offset =0x40044, .val=0x1740 },
    { .offset =0x40064, .val=0x0 },
    { .offset =0x40005, .val=0x16 },
    { .offset =0x40025, .val=0x83 },
    { .offset =0x40045, .val=0x4b },
    { .offset =0x40065, .val=0x0 },
    { .offset =0x40006, .val=0x716 },
    { .offset =0x40026, .val=0xf },
    { .offset =0x40046, .val=0x2001 },
    { .offset =0x40066, .val=0x0 },
    { .offset =0x40007, .val=0x716 },
    { .offset =0x40027, .val=0xf },
    { .offset =0x40047, .val=0x2800 },
    { .offset =0x40067, .val=0x0 },
    { .offset =0x40008, .val=0x716 },
    { .offset =0x40028, .val=0xf },
    { .offset =0x40048, .val=0xf00 },
    { .offset =0x40068, .val=0x0 },
    { .offset =0x40009, .val=0x720 },
    { .offset =0x40029, .val=0xf },
    { .offset =0x40049, .val=0x1400 },
    { .offset =0x40069, .val=0x0 },
    { .offset =0x4000a, .val=0xe08 },
    { .offset =0x4002a, .val=0xc15 },
    { .offset =0x4004a, .val=0x0 },
    { .offset =0x4006a, .val=0x0 },
    { .offset =0x4000b, .val=0x623 },
    { .offset =0x4002b, .val=0x15 },
    { .offset =0x4004b, .val=0x0 },
    { .offset =0x4006b, .val=0x0 },
    { .offset =0x4000c, .val=0x4004 },
    { .offset =0x4002c, .val=0x80 },
    { .offset =0x4004c, .val=0x0 },
    { .offset =0x4006c, .val=0x0 },
    { .offset =0x4000d, .val=0xe08 },
    { .offset =0x4002d, .val=0xc1a },
    { .offset =0x4004d, .val=0x0 },
    { .offset =0x4006d, .val=0x0 },
    { .offset =0x4000e, .val=0x623 },
    { .offset =0x4002e, .val=0x1a },
    { .offset =0x4004e, .val=0x0 },
    { .offset =0x4006e, .val=0x0 },
    { .offset =0x4000f, .val=0x4040 },
    { .offset =0x4002f, .val=0x80 },
    { .offset =0x4004f, .val=0x0 },
    { .offset =0x4006f, .val=0x0 },
    { .offset =0x40010, .val=0x2604 },
    { .offset =0x40030, .val=0x15 },
    { .offset =0x40050, .val=0x0 },
    { .offset =0x40070, .val=0x0 },
    { .offset =0x40011, .val=0x708 },
    { .offset =0x40031, .val=0x5 },
    { .offset =0x40051, .val=0x0 },
    { .offset =0x40071, .val=0x2002 },
    { .offset =0x40012, .val=0x8 },
    { .offset =0x40032, .val=0x80 },
    { .offset =0x40052, .val=0x0 },
    { .offset =0x40072, .val=0x0 },
    { .offset =0x40013, .val=0x2604 },
    { .offset =0x40033, .val=0x1a },
    { .offset =0x40053, .val=0x0 },
    { .offset =0x40073, .val=0x0 },
    { .offset =0x40014, .val=0x708 },
    { .offset =0x40034, .val=0xa },
    { .offset =0x40054, .val=0x0 },
    { .offset =0x40074, .val=0x2002 },
    { .offset =0x40015, .val=0x4040 },
    { .offset =0x40035, .val=0x80 },
    { .offset =0x40055, .val=0x0 },
    { .offset =0x40075, .val=0x0 },
    { .offset =0x40016, .val=0x60a },
    { .offset =0x40036, .val=0x15 },
    { .offset =0x40056, .val=0x1200 },
    { .offset =0x40076, .val=0x0 },
    { .offset =0x40017, .val=0x61a },
    { .offset =0x40037, .val=0x15 },
    { .offset =0x40057, .val=0x1300 },
    { .offset =0x40077, .val=0x0 },
    { .offset =0x40018, .val=0x60a },
    { .offset =0x40038, .val=0x1a },
    { .offset =0x40058, .val=0x1200 },
    { .offset =0x40078, .val=0x0 },
    { .offset =0x40019, .val=0x642 },
    { .offset =0x40039, .val=0x1a },
    { .offset =0x40059, .val=0x1300 },
    { .offset =0x40079, .val=0x0 },
    { .offset =0x4001a, .val=0x4808 },
    { .offset =0x4003a, .val=0x880 },
    { .offset =0x4005a, .val=0x0 },
    { .offset =0x4007a, .val=0x0 },
    { .offset =0x900a7, .val=0x0 },
    { .offset =0x900a8, .val=0x790 },
    { .offset =0x900a9, .val=0x11a },
    { .offset =0x900aa, .val=0x8 },
    { .offset =0x900ab, .val=0x7aa },
    { .offset =0x900ac, .val=0x2a },
    { .offset =0x900ad, .val=0x10 },
    { .offset =0x900ae, .val=0x7b2 },
    { .offset =0x900af, .val=0x2a },
    { .offset =0x900b0, .val=0x0 },
    { .offset =0x900b1, .val=0x7c8 },
    { .offset =0x900b2, .val=0x109 },
    { .offset =0x900b3, .val=0x10 },
    { .offset =0x900b4, .val=0x2a8 },
    { .offset =0x900b5, .val=0x129 },
    { .offset =0x900b6, .val=0x8 },
    { .offset =0x900b7, .val=0x370 },
    { .offset =0x900b8, .val=0x129 },
    { .offset =0x900b9, .val=0xa },
    { .offset =0x900ba, .val=0x3c8 },
    { .offset =0x900bb, .val=0x1a9 },
    { .offset =0x900bc, .val=0xc },
    { .offset =0x900bd, .val=0x408 },
    { .offset =0x900be, .val=0x199 },
    { .offset =0x900bf, .val=0x14 },
    { .offset =0x900c0, .val=0x790 },
    { .offset =0x900c1, .val=0x11a },
    { .offset =0x900c2, .val=0x8 },
    { .offset =0x900c3, .val=0x4 },
    { .offset =0x900c4, .val=0x18 },
    { .offset =0x900c5, .val=0xc },
    { .offset =0x900c6, .val=0x408 },
    { .offset =0x900c7, .val=0x199 },
    { .offset =0x900c8, .val=0x8 },
    { .offset =0x900c9, .val=0x8568 },
    { .offset =0x900ca, .val=0x108 },
    { .offset =0x900cb, .val=0x18 },
    { .offset =0x900cc, .val=0x790 },
    { .offset =0x900cd, .val=0x16a },
    { .offset =0x900ce, .val=0x8 },
    { .offset =0x900cf, .val=0x1d8 },
    { .offset =0x900d0, .val=0x169 },
    { .offset =0x900d1, .val=0x10 },
    { .offset =0x900d2, .val=0x8558 },
    { .offset =0x900d3, .val=0x168 },
    { .offset =0x900d4, .val=0x70 },
    { .offset =0x900d5, .val=0x788 },
    { .offset =0x900d6, .val=0x16a },
    { .offset =0x900d7, .val=0x1ff8 },
    { .offset =0x900d8, .val=0x85a8 },
    { .offset =0x900d9, .val=0x1e8 },
    { .offset =0x900da, .val=0x50 },
    { .offset =0x900db, .val=0x798 },
    { .offset =0x900dc, .val=0x16a },
    { .offset =0x900dd, .val=0x60 },
    { .offset =0x900de, .val=0x7a0 },
    { .offset =0x900df, .val=0x16a },
    { .offset =0x900e0, .val=0x8 },
    { .offset =0x900e1, .val=0x8310 },
    { .offset =0x900e2, .val=0x168 },
    { .offset =0x900e3, .val=0x8 },
    { .offset =0x900e4, .val=0xa310 },
    { .offset =0x900e5, .val=0x168 },
    { .offset =0x900e6, .val=0xa },
    { .offset =0x900e7, .val=0x408 },
    { .offset =0x900e8, .val=0x169 },
    { .offset =0x900e9, .val=0x6e },
    { .offset =0x900ea, .val=0x0 },
    { .offset =0x900eb, .val=0x68 },
    { .offset =0x900ec, .val=0x0 },
    { .offset =0x900ed, .val=0x408 },
    { .offset =0x900ee, .val=0x169 },
    { .offset =0x900ef, .val=0x0 },
    { .offset =0x900f0, .val=0x8310 },
    { .offset =0x900f1, .val=0x168 },
    { .offset =0x900f2, .val=0x0 },
    { .offset =0x900f3, .val=0xa310 },
    { .offset =0x900f4, .val=0x168 },
    { .offset =0x900f5, .val=0x1ff8 },
    { .offset =0x900f6, .val=0x85a8 },
    { .offset =0x900f7, .val=0x1e8 },
    { .offset =0x900f8, .val=0x68 },
    { .offset =0x900f9, .val=0x798 },
    { .offset =0x900fa, .val=0x16a },
    { .offset =0x900fb, .val=0x78 },
    { .offset =0x900fc, .val=0x7a0 },
    { .offset =0x900fd, .val=0x16a },
    { .offset =0x900fe, .val=0x68 },
    { .offset =0x900ff, .val=0x790 },
    { .offset =0x90100, .val=0x16a },
    { .offset =0x90101, .val=0x8 },
    { .offset =0x90102, .val=0x8b10 },
    { .offset =0x90103, .val=0x168 },
    { .offset =0x90104, .val=0x8 },
    { .offset =0x90105, .val=0xab10 },
    { .offset =0x90106, .val=0x168 },
    { .offset =0x90107, .val=0xa },
    { .offset =0x90108, .val=0x408 },
    { .offset =0x90109, .val=0x169 },
    { .offset =0x9010a, .val=0x58 },
    { .offset =0x9010b, .val=0x0 },
    { .offset =0x9010c, .val=0x68 },
    { .offset =0x9010d, .val=0x0 },
    { .offset =0x9010e, .val=0x408 },
    { .offset =0x9010f, .val=0x169 },
    { .offset =0x90110, .val=0x0 },
    { .offset =0x90111, .val=0x8b10 },
    { .offset =0x90112, .val=0x168 },
    { .offset =0x90113, .val=0x0 },
    { .offset =0x90114, .val=0xab10 },
    { .offset =0x90115, .val=0x168 },
    { .offset =0x90116, .val=0x0 },
    { .offset =0x90117, .val=0x1d8 },
    { .offset =0x90118, .val=0x169 },
    { .offset =0x90119, .val=0x80 },
    { .offset =0x9011a, .val=0x790 },
    { .offset =0x9011b, .val=0x16a },
    { .offset =0x9011c, .val=0x18 },
    { .offset =0x9011d, .val=0x7aa },
    { .offset =0x9011e, .val=0x6a },
    { .offset =0x9011f, .val=0xa },
    { .offset =0x90120, .val=0x0 },
    { .offset =0x90121, .val=0x1e9 },
    { .offset =0x90122, .val=0x8 },
    { .offset =0x90123, .val=0x8080 },
    { .offset =0x90124, .val=0x108 },
    { .offset =0x90125, .val=0xf },
    { .offset =0x90126, .val=0x408 },
    { .offset =0x90127, .val=0x169 },
    { .offset =0x90128, .val=0xc },
    { .offset =0x90129, .val=0x0 },
    { .offset =0x9012a, .val=0x68 },
    { .offset =0x9012b, .val=0x9 },
    { .offset =0x9012c, .val=0x0 },
    { .offset =0x9012d, .val=0x1a9 },
    { .offset =0x9012e, .val=0x0 },
    { .offset =0x9012f, .val=0x408 },
    { .offset =0x90130, .val=0x169 },
    { .offset =0x90131, .val=0x0 },
    { .offset =0x90132, .val=0x8080 },
    { .offset =0x90133, .val=0x108 },
    { .offset =0x90134, .val=0x8 },
    { .offset =0x90135, .val=0x7aa },
    { .offset =0x90136, .val=0x6a },
    { .offset =0x90137, .val=0x0 },
    { .offset =0x90138, .val=0x8568 },
    { .offset =0x90139, .val=0x108 },
    { .offset =0x9013a, .val=0xb7 },
    { .offset =0x9013b, .val=0x790 },
    { .offset =0x9013c, .val=0x16a },
    { .offset =0x9013d, .val=0x1d },
    { .offset =0x9013e, .val=0x0 },
    { .offset =0x9013f, .val=0x68 },
    { .offset =0x90140, .val=0x8 },
    { .offset =0x90141, .val=0x8558 },
    { .offset =0x90142, .val=0x168 },
    { .offset =0x90143, .val=0xf },
    { .offset =0x90144, .val=0x408 },
    { .offset =0x90145, .val=0x169 },
    { .offset =0x90146, .val=0xc },
    { .offset =0x90147, .val=0x0 },
    { .offset =0x90148, .val=0x68 },
    { .offset =0x90149, .val=0x0 },
    { .offset =0x9014a, .val=0x408 },
    { .offset =0x9014b, .val=0x169 },
    { .offset =0x9014c, .val=0x0 },
    { .offset =0x9014d, .val=0x8558 },
    { .offset =0x9014e, .val=0x168 },
    { .offset =0x9014f, .val=0x8 },
    { .offset =0x90150, .val=0x3c8 },
    { .offset =0x90151, .val=0x1a9 },
    { .offset =0x90152, .val=0x3 },
    { .offset =0x90153, .val=0x370 },
    { .offset =0x90154, .val=0x129 },
    { .offset =0x90155, .val=0x20 },
    { .offset =0x90156, .val=0x2aa },
    { .offset =0x90157, .val=0x9 },
    { .offset =0x90158, .val=0x0 },
    { .offset =0x90159, .val=0x400 },
    { .offset =0x9015a, .val=0x10e },
    { .offset =0x9015b, .val=0x8 },
    { .offset =0x9015c, .val=0xe8 },
    { .offset =0x9015d, .val=0x109 },
    { .offset =0x9015e, .val=0x0 },
    { .offset =0x9015f, .val=0x8140 },
    { .offset =0x90160, .val=0x10c },
    { .offset =0x90161, .val=0x10 },
    { .offset =0x90162, .val=0x8138 },
    { .offset =0x90163, .val=0x10c },
    { .offset =0x90164, .val=0x8 },
    { .offset =0x90165, .val=0x7c8 },
    { .offset =0x90166, .val=0x101 },
    { .offset =0x90167, .val=0x8 },
    { .offset =0x90168, .val=0x0 },
    { .offset =0x90169, .val=0x8 },
    { .offset =0x9016a, .val=0x8 },
    { .offset =0x9016b, .val=0x448 },
    { .offset =0x9016c, .val=0x109 },
    { .offset =0x9016d, .val=0xf },
    { .offset =0x9016e, .val=0x7c0 },
    { .offset =0x9016f, .val=0x109 },
    { .offset =0x90170, .val=0x0 },
    { .offset =0x90171, .val=0xe8 },
    { .offset =0x90172, .val=0x109 },
    { .offset =0x90173, .val=0x47 },
    { .offset =0x90174, .val=0x630 },
    { .offset =0x90175, .val=0x109 },
    { .offset =0x90176, .val=0x8 },
    { .offset =0x90177, .val=0x618 },
    { .offset =0x90178, .val=0x109 },
    { .offset =0x90179, .val=0x8 },
    { .offset =0x9017a, .val=0xe0 },
    { .offset =0x9017b, .val=0x109 },
    { .offset =0x9017c, .val=0x0 },
    { .offset =0x9017d, .val=0x7c8 },
    { .offset =0x9017e, .val=0x109 },
    { .offset =0x9017f, .val=0x8 },
    { .offset =0x90180, .val=0x8140 },
    { .offset =0x90181, .val=0x10c },
    { .offset =0x90182, .val=0x0 },
    { .offset =0x90183, .val=0x1 },
    { .offset =0x90184, .val=0x8 },
    { .offset =0x90185, .val=0x8 },
    { .offset =0x90186, .val=0x4 },
    { .offset =0x90187, .val=0x8 },
    { .offset =0x90188, .val=0x8 },
    { .offset =0x90189, .val=0x7c8 },
    { .offset =0x9018a, .val=0x101 },
    { .offset =0x90006, .val=0x0 },
    { .offset =0x90007, .val=0x0 },
    { .offset =0x90008, .val=0x8 },
    { .offset =0x90009, .val=0x0 },
    { .offset =0x9000a, .val=0x0 },
    { .offset =0x9000b, .val=0x0 },
    { .offset =0xd00e7, .val=0x400 },
    { .offset =0x90017, .val=0x0 },
    { .offset =0x9001f, .val=0x2a },
    { .offset =0x90026, .val=0x6a },
    { .offset =0x400d0, .val=0x0 },
    { .offset =0x400d1, .val=0x101 },
    { .offset =0x400d2, .val=0x105 },
    { .offset =0x400d3, .val=0x107 },
    { .offset =0x400d4, .val=0x10f },
    { .offset =0x400d5, .val=0x202 },
    { .offset =0x400d6, .val=0x20a },
    { .offset =0x400d7, .val=0x20b },
    { .offset =0x2003a, .val=0x2 },
    { .offset =0x9000c, .val=0x0 },
    { .offset =0x9000d, .val=0x173 },
    { .offset =0x9000e, .val=0x60 },
    { .offset =0x9000f, .val=0x6110 },
    { .offset =0x90010, .val=0x2152 },
    { .offset =0x90011, .val=0xdfbd },
    { .offset =0x90012, .val=0x60 },
    { .offset =0x90013, .val=0x6152 },
    { .offset =0x20010, .val=0x5a },
    { .offset =0x20011, .val=0x3 },
    { .offset =0x40080, .val=0xe0 },
    { .offset =0x40081, .val=0x12 },
    { .offset =0x40082, .val=0xe0 },
    { .offset =0x40083, .val=0x12 },
    { .offset =0x40084, .val=0xe0 },
    { .offset =0x40085, .val=0x12 },
    { .offset =0x400fd, .val=0xf },
    { .offset =0x10011, .val=0x1 },
    { .offset =0x10012, .val=0x1 },
    { .offset =0x10013, .val=0x180 },
    { .offset =0x10018, .val=0x1 },
    { .offset =0x10002, .val=0x6209 },
    { .offset =0x100b2, .val=0x1 },
    { .offset =0x101b4, .val=0x1 },
    { .offset =0x102b4, .val=0x1 },
    { .offset =0x103b4, .val=0x1 },
    { .offset =0x104b4, .val=0x1 },
    { .offset =0x105b4, .val=0x1 },
    { .offset =0x106b4, .val=0x1 },
    { .offset =0x107b4, .val=0x1 },
    { .offset =0x108b4, .val=0x1 },
    { .offset =0x11011, .val=0x1 },
    { .offset =0x11012, .val=0x1 },
    { .offset =0x11013, .val=0x180 },
    { .offset =0x11018, .val=0x1 },
    { .offset =0x11002, .val=0x6209 },
    { .offset =0x110b2, .val=0x1 },
    { .offset =0x111b4, .val=0x1 },
    { .offset =0x112b4, .val=0x1 },
    { .offset =0x113b4, .val=0x1 },
    { .offset =0x114b4, .val=0x1 },
    { .offset =0x115b4, .val=0x1 },
    { .offset =0x116b4, .val=0x1 },
    { .offset =0x117b4, .val=0x1 },
    { .offset =0x118b4, .val=0x1 },
    { .offset =0x12011, .val=0x1 },
    { .offset =0x12012, .val=0x1 },
    { .offset =0x12013, .val=0x180 },
    { .offset =0x12018, .val=0x1 },
    { .offset =0x12002, .val=0x6209 },
    { .offset =0x120b2, .val=0x1 },
    { .offset =0x121b4, .val=0x1 },
    { .offset =0x122b4, .val=0x1 },
    { .offset =0x123b4, .val=0x1 },
    { .offset =0x124b4, .val=0x1 },
    { .offset =0x125b4, .val=0x1 },
    { .offset =0x126b4, .val=0x1 },
    { .offset =0x127b4, .val=0x1 },
    { .offset =0x128b4, .val=0x1 },
    { .offset =0x13011, .val=0x1 },
    { .offset =0x13012, .val=0x1 },
    { .offset =0x13013, .val=0x180 },
    { .offset =0x13018, .val=0x1 },
    { .offset =0x13002, .val=0x6209 },
    { .offset =0x130b2, .val=0x1 },
    { .offset =0x131b4, .val=0x1 },
    { .offset =0x132b4, .val=0x1 },
    { .offset =0x133b4, .val=0x1 },
    { .offset =0x134b4, .val=0x1 },
    { .offset =0x135b4, .val=0x1 },
    { .offset =0x136b4, .val=0x1 },
    { .offset =0x137b4, .val=0x1 },
    { .offset =0x138b4, .val=0x1 },
    { .offset =0x2003a, .val=0x2 },
    { .offset =0xc0080, .val=0x2 },
    { .offset =0xd0000, .val=0x1 },
};

static bool trained_csr_saved = false;

/* save the necessary trained CSRs before IO retention */
static void save_trained_csr(void)
{
	int index, offset, size;

	size = sizeof(lpddr4_saved_trained_csr) / 8;

	dwc_ddrphy_apb_wr(0xd0000, 0x0);
	dwc_ddrphy_apb_wr(0xc0080, 0x3);

	for (index = 0; index < size; index++) {
		offset = lpddr4_saved_trained_csr[index].offset;
		lpddr4_saved_trained_csr[index].val = dwc_ddrphy_apb_rd(offset);
	}

	dwc_ddrphy_apb_wr(0xc0080, 0x0);
	dwc_ddrphy_apb_wr(0xd0000, 0x1);
}

/* restore the trained CSRs before exit IO retention */
static void restore_trained_csr(void)
{
	int index, offset, val, size;

	size = sizeof(lpddr4_saved_trained_csr) / 8;

	for(index = 0; index < size; index++) {
		val = lpddr4_saved_trained_csr[index].val;
		offset = lpddr4_saved_trained_csr[index].offset;

		dwc_ddrphy_apb_wr(offset, val);
	}
}

static void phy_cfg_before_csr_restore(void)
{
	int index, offset, val, size;

	size = sizeof(data_before_restore) / 8;

	for(index = 0; index < size; index++) {
		val = data_before_restore[index].val;
		offset = data_before_restore[index].offset;

		dwc_ddrphy_apb_wr(offset, val);
	}

}

static void phy_cfg_after_csr_restore(void)
{
	int index, offset, val, size;

	size = sizeof(data_after_restore) / 8;

	for(index = 0; index < size; index++) {
		val = data_after_restore[index].val;
		offset = data_after_restore[index].offset;

		dwc_ddrphy_apb_wr(offset, val);
	}
}

void lpddr4_cfg_umctl2(void)
{
	mmio_write_32(IMX_DDRC_BASE + 0x00000304, 0x00000001);
	mmio_write_32(IMX_DDRC_BASE + 0x00000030, 0x00000001);
	mmio_write_32(IMX_DDRC_BASE + 0x00000000, 0x83080020);
	mmio_write_32(IMX_DDRC_BASE + 0x00000064, 0x006180e0);
	mmio_write_32(IMX_DDRC_BASE + 0x000000d0, 0xc003061B);
	mmio_write_32(IMX_DDRC_BASE + 0x000000d4, 0x009D0000);
	mmio_write_32(IMX_DDRC_BASE + 0x000000d8, 0x0000fe05);
	mmio_write_32(IMX_DDRC_BASE + 0x000000dc, 0x00d4002d);
	mmio_write_32(IMX_DDRC_BASE + 0x000000e0, 0x00310008);
	mmio_write_32(IMX_DDRC_BASE + 0x000000e4, 0x00040009);
	mmio_write_32(IMX_DDRC_BASE + 0x000000e8, 0x0046004d);
	mmio_write_32(IMX_DDRC_BASE + 0x000000ec, 0x0005004d);
	mmio_write_32(IMX_DDRC_BASE + 0x000000f4, 0x00000979);
	mmio_write_32(IMX_DDRC_BASE + 0x00000100, 0x1a203522);
	mmio_write_32(IMX_DDRC_BASE + 0x00000104, 0x00060630);
	mmio_write_32(IMX_DDRC_BASE + 0x00000108, 0x070e1214);
	mmio_write_32(IMX_DDRC_BASE + 0x0000010c, 0x00b0c006);
	mmio_write_32(IMX_DDRC_BASE + 0x00000110, 0x0f04080f);
	mmio_write_32(IMX_DDRC_BASE + 0x00000114, 0x0d0d0c0c);
	mmio_write_32(IMX_DDRC_BASE + 0x00000118, 0x01010007);
	mmio_write_32(IMX_DDRC_BASE + 0x0000011c, 0x0000060a);
	mmio_write_32(IMX_DDRC_BASE + 0x00000120, 0x01010101);
	mmio_write_32(IMX_DDRC_BASE + 0x00000124, 0x40000008);
	mmio_write_32(IMX_DDRC_BASE + 0x00000128, 0x00050d01);
	mmio_write_32(IMX_DDRC_BASE + 0x0000012c, 0x01010008);
	mmio_write_32(IMX_DDRC_BASE + 0x00000130, 0x00020000);
	mmio_write_32(IMX_DDRC_BASE + 0x00000134, 0x18100002);
	mmio_write_32(IMX_DDRC_BASE + 0x00000138, 0x00000dc2);
	mmio_write_32(IMX_DDRC_BASE + 0x0000013c, 0x80000000);
	mmio_write_32(IMX_DDRC_BASE + 0x00000144, 0x00a00050);
	mmio_write_32(IMX_DDRC_BASE + 0x00000180, 0x53200018);
	mmio_write_32(IMX_DDRC_BASE + 0x00000184, 0x02800070);
	mmio_write_32(IMX_DDRC_BASE + 0x00000188, 0x00000000);
	mmio_write_32(IMX_DDRC_BASE + 0x00000190, 0x0397820a);
	mmio_write_32(IMX_DDRC_BASE + 0x00000194, 0x00020103);
	mmio_write_32(IMX_DDRC_BASE + 0x000001a0, 0xe0400018);
	mmio_write_32(IMX_DDRC_BASE + 0x000001a4, 0x00df00e4);
	mmio_write_32(IMX_DDRC_BASE + 0x000001a8, 0x00000000);
	mmio_write_32(IMX_DDRC_BASE + 0x000001b0, 0x00000011);
	mmio_write_32(IMX_DDRC_BASE + 0x000001b4, 0x0000170a);
	mmio_write_32(IMX_DDRC_BASE + 0x000001c0, 0x00000001);
	mmio_write_32(IMX_DDRC_BASE + 0x000001c4, 0x00000000);
	/* Address map is from MSB 29: r15, r14, cs, r13-r0, b2-b0, c9-c0 */
	mmio_write_32(IMX_DDRC_BASE + DDRC_ADDRMAP0, 0x00000015);
	mmio_write_32(IMX_DDRC_BASE + DDRC_ADDRMAP4, 0x00001F1F);
	/* bank interleave */
	mmio_write_32(IMX_DDRC_BASE + DDRC_ADDRMAP1, 0x00080808);
	mmio_write_32(IMX_DDRC_BASE + DDRC_ADDRMAP5, 0x07070707);
	mmio_write_32(IMX_DDRC_BASE + DDRC_ADDRMAP6, 0x08080707);
	mmio_write_32(IMX_DDRC_BASE +  0x00000240, 0x020f0c54);
	mmio_write_32(IMX_DDRC_BASE +  0x00000244, 0x00000000);
	mmio_write_32(IMX_DDRC_BASE +  0x00000490, 0x00000001);

        /* performance setting */
	mmio_write_32(IMX_DDRC_BASE + DDRC_ODTCFG, 0x0b060908);
	mmio_write_32(IMX_DDRC_BASE + DDRC_ODTMAP, 0x00000000);
	mmio_write_32(IMX_DDRC_BASE + DDRC_SCHED, 0x29511505);
	mmio_write_32(IMX_DDRC_BASE + DDRC_SCHED1, 0x0000002c);
	mmio_write_32(IMX_DDRC_BASE + DDRC_PERFHPR1, 0x5900575b);
	mmio_write_32(IMX_DDRC_BASE + DDRC_PERFLPR1, 0x900093e7);
	mmio_write_32(IMX_DDRC_BASE + DDRC_PERFWR1, 0x02005574);
	mmio_write_32(IMX_DDRC_BASE + DDRC_DBG0, 0x00000016);
	mmio_write_32(IMX_DDRC_BASE + DDRC_DBG1, 0x00000000);
	mmio_write_32(IMX_DDRC_BASE + DDRC_DBGCMD, 0x00000000);
	mmio_write_32(IMX_DDRC_BASE + DDRC_SWCTL, 0x00000001);
	mmio_write_32(IMX_DDRC_BASE + DDRC_POISONCFG, 0x00000011);
	mmio_write_32(IMX_DDRC_BASE + DDRC_PCCFG, 0x00000111);
	mmio_write_32(IMX_DDRC_BASE + DDRC_PCFGR_0, 0x000010f3);
	mmio_write_32(IMX_DDRC_BASE + DDRC_PCFGW_0, 0x000072ff);
	mmio_write_32(IMX_DDRC_BASE + DDRC_PCTRL_0, 0x00000001);
	mmio_write_32(IMX_DDRC_BASE + DDRC_PCFGQOS0_0, 0x01110d00);
	mmio_write_32(IMX_DDRC_BASE + DDRC_PCFGQOS1_0, 0x00620790);
	mmio_write_32(IMX_DDRC_BASE + DDRC_PCFGWQOS0_0, 0x00100001);
	mmio_write_32(IMX_DDRC_BASE + DDRC_PCFGWQOS1_0, 0x0000041f);
	mmio_write_32(IMX_DDRC_BASE + DDRC_FREQ1_DERATEEN, 0x00000202);
	mmio_write_32(IMX_DDRC_BASE + DDRC_FREQ1_DERATEINT, 0xec78f4b5);
	mmio_write_32(IMX_DDRC_BASE + DDRC_FREQ1_RFSHCTL0, 0x00618040);
	mmio_write_32(IMX_DDRC_BASE + DDRC_FREQ1_RFSHTMG, 0x00610090);
}

void lpddr4_cfg_phy(void)
{
	/* phy init config before restore the trained CSR */
	phy_cfg_before_csr_restore();
	/* restore the trained CSR, no need to do re-training */
	restore_trained_csr();
	/* load the PIE */
	phy_cfg_after_csr_restore();
}

void ddrc_enter_retention(void)
{
	NOTICE("enter LPDDR4 retention\n");

	/* save the ddr phy trained csr in ocram */
	if (!trained_csr_saved) {
		save_trained_csr();
		trained_csr_saved = true;
	}

	/* wait DBGCAM to be empty */
	while (mmio_read_32(IMX_DDRC_BASE + 0x308) != 0x36000000)
		;
	/* Block AXI port for any transaction */
	mmio_write_32(IMX_DDRC_BASE + DDRC_PCTRL_0, 0x0);
	/* wait for AXI port is idle */
	while (mmio_read_32(IMX_DDRC_BASE + 0x3fc) & 0x10001)
		;
	/* enter self refresh */
	mmio_write_32(IMX_DDRC_BASE + DDRC_PWRCTL, 0xaa);
	/* wait for STAT refresh type = Self Refresh */
	while ((mmio_read_32(IMX_DDRC_BASE + DDRC_STAT) & 0x33f) != 0x223)
		;
	mmio_write_32(IMX_DDRC_BASE + DDRC_DFIMISC, 0x00000000);
	/* set SWCTL.sw_done to disable quasi-dynamic register programming outside reset */
	mmio_write_32(IMX_DDRC_BASE + DDRC_SWCTL, 0x00000000);

	mmio_write_32(IMX_DDRC_BASE + DDRC_DFIMISC, 0x00001f00);
	mmio_write_32(IMX_DDRC_BASE + DDRC_DFIMISC, 0x00001f20);

	/* wait DFISTAT.dfi_init_complete to 0 */
	while((mmio_read_32(IMX_DDRC_BASE + DDRC_DFISTAT) & 0x1) == 0x1)
		;

	mmio_write_32(IMX_DDRC_BASE + DDRC_DFIMISC, 0x00001f00);

	/* wait DFISTAT.dfi_init_complete to 1 */
	while(!(mmio_read_32(IMX_DDRC_BASE + DDRC_DFISTAT) & 0x1))
		;

	/* set the SWCTL done to enable quasi-dynamic register programming outside reset */
	mmio_write_32(IMX_DDRC_BASE + DDRC_SWCTL, 0x00000001);

	/*
	 * Transition the PHY to LP3/IO retention state by using a DFI
	 * frequency operation.
	 */
	mmio_write_32(0x3c000000 + 4 * 0xd0000, 0x0);
	mmio_write_32(0x3c000000 + 4 * 0xd0000, 0x1);

	/* loop for a while */
	for (int i; i < 20; i++);

	/* pwrdnreq_async adbm/adbs of ddr */
	mmio_write_32(0x303a01fc, mmio_read_32(0x303a01fc) & ~(0x2));
	while (mmio_read_32(0x303a01fc) & (1 << 18));

	/* set the PwrOkIn signal to 0, this will enable the data retention feature on
	 * the CKE and MEMRESET */
	mmio_write_32(0x30391000, 0x8f000008);

	/* loop for a while */
	for (int i; i < 20; i++);

	/* set CCM CCGR */
	mmio_write_32(0x30384050, 0x0);
	mmio_write_32(0x30380950, 0x2);

	/* enable ISO */
	mmio_write_32(0x303a0d40, 0x1);
	mmio_write_32(0x303a0104, (1 << 5));

	/*
	 * turn off the core VDD and VAA supplies, these supply can be turned
	 * off in any order, the VDDQ power supply must be active during LP3/IO
	 * retention.
	 */
}

void ddrc_exit_retention(void)
{
	int i;

	INFO("exit LPDDR4 retention\n");

	for(i = 0; i < 50; i++);

	/* start the clock and reset the PHY */
	/* assert all reset */
	mmio_write_32(0x30391000 + 0x4, 0x8f000001);
	mmio_write_32(0x30391000, 0x8f00000f);

	mmio_write_32(0x30384050, 0x2);
	mmio_write_32(0x30380950, 0x2);

	/* power up the DDRMIX */
	mmio_write_32(0x303a00f8, (1 << 5));
	/* wait for DDRMIX is power up suscessfully */
	while (mmio_read_32(0x303a0104) & ( 1 << 5))
		;

	mmio_write_32(0x303a01fc, mmio_read_32(0x303a01fc) | 0x2);

	mmio_write_32(0x30360060, 0x80000208);

	/* release the src_system_rst_b */
	mmio_write_32(0x30391004, 0x8f000000);
	mmio_write_32(0x30391000, 0x8f000006);

	/* short delay */
	for (int i = 0; i < 500; i++);

	/* config umctl2 */
	lpddr4_cfg_umctl2();

	/* Skip the DRAM init routine and starts up in self-refresh mode */
	/* Program INIT0.skip_dram_init = 2b'11 */
	mmio_write_32(IMX_DDRC_BASE + DDRC_INIT0, 0xc0000000 | mmio_read_32(IMX_DDRC_BASE + DDRC_INIT0));

	/* keep the controller in self-refresh mode */
	mmio_write_32(IMX_DDRC_BASE + DDRC_PWRCTL, 0x000000aa);

	/* release all reset */
	mmio_write_32(0x30391000, 0x8f000000);

	/* loop for a while */
	for (int i; i < 500; i++);

	mmio_write_32(IMX_DDRC_BASE + DDRC_DBG1, 0x0);

	mmio_write_32(IMX_DDRC_BASE + DDRC_SWCTL, 0x0);

	/* LPDDR4 mode */
	mmio_write_32(0x3d000000, 0x1);

	mmio_write_32(IMX_DDRC_BASE + DDRC_DFIMISC, 0x0);

	lpddr4_cfg_phy();

	mmio_write_32(0x3c000000 + 4 * 0xd0000, 0x0);
	while (mmio_read_32(0x3c000000 + 4 * 0x20097))
		;
	/* set the MicroContMuxSel to 1 */
	mmio_write_32(0x3c000000 + 4 * 0xd0000, 0x1);

	/* before write Dynamic reg, sw_done should be 0 */
	mmio_write_32(IMX_DDRC_BASE + DDRC_SWCTL, 0x0);
	/* trigger the PHY init start request */
	mmio_write_32(IMX_DDRC_BASE + DDRC_DFIMISC, 0x20);

	/* wait for dfi_init_complete done */
	while (!(mmio_read_32(IMX_DDRC_BASE + DDRC_DFISTAT) & 0x1))
		;
	/* clear DFIMISC.dfi_init_start */
	mmio_write_32(IMX_DDRC_BASE + DDRC_DFIMISC, 0x0);
	/* set the DFIMISC.dfi_init_complete_en */
	mmio_write_32(IMX_DDRC_BASE + DDRC_DFIMISC, 0x1);

	/* set SWCTL.sw_done to enable quasi-dynamic register programming outside reset */
	mmio_write_32(IMX_DDRC_BASE + DDRC_SWCTL, 0x1);
	/* wait for SWSTAT.sw_done_ack to 1 */
	while((mmio_read_32(IMX_DDRC_BASE + DDRC_SWSTAT) & 0x1) != 0x1)
		;

	mmio_write_32(IMX_DDRC_BASE + DDRC_PWRCTL, 0xaa);
	mmio_write_32(IMX_DDRC_BASE + DDRC_SWCTL, 0x0);
	mmio_write_32(IMX_DDRC_BASE + DDRC_DFIMISC, 0x1);
	mmio_write_32(IMX_DDRC_BASE + DDRC_SWCTL, 0x1);

	/* wait STAT to normal state */
	while((mmio_read_32(IMX_DDRC_BASE + DDRC_SWSTAT) & 0x1) != 0x1)
		;

	mmio_write_32(IMX_DDRC_BASE + DDRC_PWRCTL, 0x88);

	while((mmio_read_32(IMX_DDRC_BASE + DDRC_STAT) & 0x7) != 0x1)

	mmio_write_32(IMX_DDRC_BASE + DDRC_DERATEEN, 0x302);
	mmio_write_32(IMX_DDRC_BASE + DDRC_PCTRL_0, 0x1);
	mmio_write_32(IMX_DDRC_BASE + DDRC_RFSHCTL3, 0x0);
}

void lpddr4_dvfs_hwffc(int init_vrcg, int init_fsp, int target_freq,
		int discamdrain)
{
	uint32_t tmp, tmp_t;

	INFO("hwffc enter\n");

	/* step 1: hwffc flow enter, set the HWFCCCTL */
	tmp = ((init_fsp << 4) & 0x10) | ((init_vrcg << 5) & 0x20) | 0x40;
	tmp |= 0x3;
	mmio_write_32(IMX_DDRC_BASE + DDRC_HWFFCCTL, tmp);

	/*
	 * set SWCTL.sw_done to disable quasi-dynamic register programming
	 * outside reset
	 */
	mmio_write_32(IMX_DDRC_BASE + DDRC_SWCTL, 0x0);

	mmio_write_32(IMX_DDRC_BASE + DDRC_DFIMISC, 0x11);

	/*
	 * set SWCTL.sw_done to enable quasi-dynamic register programming
	 * outside reset.
	 */
	mmio_write_32(IMX_DDRC_BASE + DDRC_SWCTL, 0x1);
	/*wait SWSTAT.sw_done_ack to 1 */
	while (!(mmio_read_32(IMX_DDRC_BASE + DDRC_SWSTAT) & 0x1))
		;

	/* wait for DBGCAM is empty */
	if (discamdrain)
		while ((mmio_read_32(IMX_DDRC_BASE + DDRC_DBGCAM) &
			 0x30000000) != 0x30000000)
			;

	/* set HWFFC requirements, ddrc_csysfreqency_ddrc[1:0] = 2b'10;
	 * [15]: ccmsrcgpcmix_ddrc_csysdisdrain_ddrc;
	 * [14:13]: ccmsrcgpcmix_ddrc_csysfrequency_ddrc[1:0];
	 * [12]: ccmsrcgpcmix_ddrc_csysmode_ddrc;
	 */
	tmp = mmio_read_32(0x303a0164);
	tmp &= 0xFFFF0FFF;
	tmp |= ((discamdrain << 15) & 0x8000);
	tmp |= ((target_freq << 13) & 0x6000);
	tmp |= 0x1000;
	mmio_write_32(0x303a0164, tmp);

	/* set the gpc_ddr1_core_csysreq to active low */
	tmp = mmio_read_32(0x303a01fc);
	tmp &= 0xfffffffe;
	mmio_write_32(0x303a01fc, tmp);

	/* wait gpc_ddr1_core_csysack to active low */
	while ((mmio_read_32(0x303a01fc) & 0x10000))
		;

	/* step B: switch the clock */
	if (target_freq == 0x1) {
		/* change the clock source of dram_alt_clk_root to source 5 --400MHz */
		mmio_write_32(0x3038a008, (0x7 << 24) | (0x7 << 16));
		mmio_write_32(0x303aa004, (0x5 << 24));

		/* change the clock source of dram_apb_clk_root to source 2 --40MHz/2 */
		mmio_write_32(0x3038a088, (0x7 << 24) | (0x7 << 16));
		mmio_write_32(0x3038a084, (0x2 << 24) | (0x1 << 16));;

		/* bypass the DRAM PLL */
		mmio_write_32(0x30389804, 1 << 24);

	} else if (target_freq == 0x2) {
		/* change the clock source of dram_alt_clk_root to source 2 --100MHz */
		mmio_write_32(0x3038a008, (0x7 << 24) | (0x7 << 16));
		mmio_write_32(0x3038a004, (0x2 << 24));

		/* change the clock source of dram_apb_clk_root to source 2 --40Mhz/2 */
		mmio_write_32(0x3038a088, (0x7 << 24) | (0x7 << 16));
		mmio_write_32(0x3038a084, (0x2 << 24) | (0x1 << 16));;

		/* bypass the DRAM PLL */
		mmio_write_32(0x30389804, 1 << 24);
	} else {
		/* switch to the default freq fsp = 0x1 3200mts */
		mmio_write_32(0x3038a088,(0x7<<24)|(0x7<<16));
		/* to source 4 --800MHz/ 4 */
		mmio_write_32(0x3038a084,(0x4<<24)|(0x3<<16));
		mmio_write_32(0x30389808, 1<<24);
	}

	/* step C: exit hwffc flow */
	tmp = mmio_read_32(0x303a01fc);
	tmp |= 0x1;
	mmio_write_32(0x303a01fc, tmp);
	/* wait gpc_ddr1_core_csysack to high */
	while (!(mmio_read_32(0x303a01fc) & 0x10000))
		;

	/* [1:0]-->2b'10, to disable HWFFC */
	mmio_write_32(IMX_DDRC_BASE + DDRC_HWFFCCTL, 0x72);

	/*wait until hwffc_in_progress = 0 */
	while(mmio_read_32(IMX_DDRC_BASE + DDRC_HWFFCSTAT) & 0x1)
		;

	mmio_write_32(IMX_DDRC_BASE + DDRC_SWCTL, 0x0);
	/* set MSTR.frequency_mode MSTR.[29], MSTR2.target_frequency[1:0] */
	tmp = mmio_read_32(IMX_DDRC_BASE + DDRC_MSTR);
	tmp |= 0x20000000;
	mmio_write_32(IMX_DDRC_BASE + DDRC_MSTR, tmp);

	tmp = mmio_read_32(IMX_DDRC_BASE + DDRC_HWFFCSTAT);
	tmp_t = (tmp >> 4) & 0x3;
	tmp = mmio_read_32(IMX_DDRC_BASE + DDRC_MSTR2);
	tmp = (tmp & 0xfffffffc) | tmp_t;
	mmio_write_32(IMX_DDRC_BASE + DDRC_MSTR2, tmp);

	mmio_write_32(IMX_DDRC_BASE + DDRC_SWCTL, 0x1);
	mmio_write_32(IMX_DDRC_BASE + DDRC_HWFFCCTL, 0x70);

	/* why we need below flow */
	mmio_write_32(IMX_DDRC_BASE + DDRC_PWRCTL, 0x1a8);
	while ((mmio_read_32(IMX_DDRC_BASE + DDRC_STAT) & 0x3) != 0x3)
		;
	/* wait SDRAM into self refresh state */
	while(!(mmio_read_32(IMX_DDRC_BASE + DDRC_STAT) & 0x30))
		;

	tmp = mmio_read_32(IMX_DDRC_BASE + DDRC_CRCPARSTAT);
	tmp = mmio_read_32(IMX_DDRC_BASE + DDRC_CRCPARSTAT);

	mmio_write_32(IMX_DDRC_BASE + DDRC_PWRCTL, 0x188);

	/* wait STAT to normal mode */
	while ((mmio_read_32(IMX_DDRC_BASE + DDRC_STAT) & 0x3) != 0x1)
		;

	INFO("hwffc exit\n");
}

spinlock_t dfs_lock;
static unsigned int init_fsp = 0x1;
static unsigned int init_vrcg = 0x1;
static unsigned int discamdrain = 0x1;

static uint32_t irqs_used [] = {102, 109, 110, 111};
static volatile uint32_t wfe_done;
static volatile bool wait_ddrc_hwffc_done = true;

/*
 * x1 for frequency setpoint info;
 * x2 is used for online cpu info
 */
int lpddr4_dvfs_handler(uint32_t smc_fid,
			u_register_t x1,
			u_register_t x2,
			u_register_t x3)
{
	uint64_t mpidr = read_mpidr_el1();
	unsigned int cpu_id = MPIDR_AFFLVL0_VAL(mpidr);
	unsigned int target_freq = x1;
	uint32_t online_cores = x2;

	if (target_freq == 0xf) {
		/* set the WFE done status */
		spin_lock(&dfs_lock);
		wfe_done |= (1 << cpu_id * 8);
		spin_unlock(&dfs_lock);

		while (1) {
			/* ddr frequency change done */
			wfe();
			if (!wait_ddrc_hwffc_done) {
				break;
			}
		}
	} else {
		wait_ddrc_hwffc_done = true;
		/* trigger the IRQ */
		for (int i = 0; i < 4; i++) {
			int irq = irqs_used[i] % 32;
			if (cpu_id != i && (online_cores & (0x1 << (i * 8))))
				mmio_write_32(0x38800204 + (irqs_used[i] / 32) * 4, (1 << irq));
		}

		/* make sure all the core in WFE */
		online_cores &= ~(0x1 << (cpu_id * 8));
		while (1) {
			if (online_cores == wfe_done)
				break;
		}

		lpddr4_dvfs_hwffc(init_vrcg, init_fsp, target_freq, discamdrain);
		init_fsp = (~init_fsp) & 0x1;
		wait_ddrc_hwffc_done = false;
		wfe_done = 0;
		dsb();
		sev();
		isb();
	}
	return 0;
}
