
#include <inttypes.h>
#include <stdint.h>
#include <arch.h>
#include <arch_helpers.h>
#include <assert.h>
#include <common/bl_common.h>
#include <lib/mmio.h>
#include <drivers/delay_timer.h>

#define WDT_BASE 0x12800800
#define WDTCNT 0x00
#define WDTSET 0x04
#define WDTTIM 0x08
#define WDTINT 0x0C

#define WDTCNT_ENABLE BIT(0)
#define WDTINT_CLEAR BIT(0)

#define MICROSECONDS_IN_SECOND 1000000UL
#define WDT_CYCLES_PER_USEC 43690
#define WDTSET_COUNTER_MASK 0xFFF00000
#define WDTSET_COUNTER_VAL(f) ((f) << 20)

#define CPG_CLKON_WDT 0x11010548
#define CPG_RST_WDT 0x11010848

static void rzg_wdt_write(uint32_t val, unsigned int reg) {
    if (reg == WDTSET)
        val &= WDTSET_COUNTER_MASK;

    mmio_write_32(WDT_BASE + reg, val);
    if (reg != WDTINT)
        udelay(1);
}

static void rzg_wdt_init_timeout(uint32_t timeout_sec) {
    uint32_t cycles = (timeout_sec * (MICROSECONDS_IN_SECOND / 2)) / WDT_CYCLES_PER_USEC;
    rzg_wdt_write(WDTINT_CLEAR, WDTINT);
    rzg_wdt_write(WDTSET_COUNTER_VAL(cycles), WDTSET);
}

void rzg_wdt_start(uint32_t timeout_sec) {
    NOTICE("Starting WDT, Timeout %ds\n", timeout_sec);
    mmio_write_32(CPG_RST_WDT, 0x000F000F);
    mmio_write_32(CPG_CLKON_WDT, 0x00FF00FF);
    udelay(35);

    rzg_wdt_init_timeout(timeout_sec);
    rzg_wdt_write(0, WDTTIM);
    rzg_wdt_write(WDTCNT_ENABLE, WDTCNT);
}
