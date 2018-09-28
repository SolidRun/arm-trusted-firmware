/*
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
 * Neither the name of NXP nor the names of its contributors may be used
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

#include <bl_common.h>
#include <gicv3.h>
#include <interrupt_props.h>
#include <plat_imx8.h>
#include <platform.h>
#include <platform_def.h>

/* the GICv3 driver only needs to be initialized in EL3 */
uintptr_t rdistif_base_addrs[PLATFORM_CORE_COUNT];

/* array of Group0 interrupts to be configured by the gic driver */
const interrupt_prop_t imx8_interrupt_props[] = {
	/* G0 interrupts */
	/* SGI0 */
	INTR_PROP_DESC(7, GIC_HIGHEST_SEC_PRIORITY, INTR_GROUP0,
			GIC_INTR_CFG_EDGE),

	/* G1S interrupts */
	/* SGI1 */
	INTR_PROP_DESC(6, GIC_HIGHEST_SEC_PRIORITY, INTR_GROUP1S,
			GIC_INTR_CFG_EDGE),

};

static unsigned int mx8_mpidr_to_core_pos(unsigned long mpidr)
{
        return (unsigned int)plat_calc_core_pos(mpidr);
}

const gicv3_driver_data_t arm_gic_data = {
	.gicd_base = PLAT_GICD_BASE,
	.gicr_base = PLAT_GICR_BASE,
	.interrupt_props = imx8_interrupt_props,
	.interrupt_props_num = ARRAY_SIZE(imx8_interrupt_props),
	.rdistif_num = PLATFORM_CORE_COUNT,
	.rdistif_base_addrs = rdistif_base_addrs,
	.mpidr_to_core_pos = mx8_mpidr_to_core_pos,
};

void plat_gic_driver_init(void)
{
	/*
	 * the GICv3 driver is initialized in EL3 and does not need
	 * to be initialized again in SEL1. This is because the S-EL1
	 * can use GIC system registers to manage interrupts and does
	 * not need GIC interface base addresses to be configured.
	 */
#if IMAGE_BL31
	gicv3_driver_init(&arm_gic_data);
#endif
}

void plat_gic_init(void)
{
	gicv3_distif_init();
	gicv3_rdistif_init(plat_my_core_pos());
	gicv3_cpuif_enable(plat_my_core_pos());
}

void plat_gic_cpuif_enable(void)
{
	gicv3_cpuif_enable(plat_my_core_pos());
}

void plat_gic_cpuif_disable(void)
{
	gicv3_cpuif_disable(plat_my_core_pos());
}

void plat_gic_pcpu_init(void)
{
	gicv3_rdistif_init(plat_my_core_pos());
}
