# Arm Trusted Firmware (ATF) — RZ/V2N SolidRun SoM

This tree builds ATF (BL2 + FIP + boot parameter tooling) for the
Renesas **RZ/V2N** on the SolidRun SoM (`BOARD=sr_som`). BL2 performs the
DDR init and loads the FIP (BL31 + BL33/U-Boot).

## Prerequisites

- An AArch64 cross toolchain (`aarch64-none-elf-` / `aarch64-linux-gnu-`)
  on your `PATH`.
- A built `u-boot.bin` for RZ/V2N — this is passed in as **BL33**.

## Building ATF for V2N

From the ATF source directory:

```
arm-trusted-firmware
```

run:

```sh
make distclean && \
make PLAT=v2n BOARD=sr_som \
     BL33=<full_path>/u-boot/u-boot.bin \
     bl2 fip bptool pkg
```

Replace `<full_path>/u-boot/u-boot.bin` with the absolute path to your
U-Boot binary, e.g.:

```sh
make distclean && \
make PLAT=v2n BOARD=sr_som \
     BL33=<full_path>/u-boot.bin \
     bl2 fip bptool pkg
```

Make targets:

| Target   | Purpose                                                       |
|----------|--------------------------------------------------------------|
| `bl2`    | Build BL2 (first-stage loader, does DDR init)                |
| `fip`    | Build the FIP image (packs BL31 + BL33/U-Boot)               |
| `bptool` | Build the boot-parameter tool                                |
| `pkg`    | Wrap BL2 with the boot parameter header (`bl2_bp_*` outputs) |

`make distclean` is run first to guarantee a clean rebuild.

## Where to find the outputs

All build artifacts land under:

```
build/v2n/release/
```

The files you flash:

```
build/v2n/release/bl2_bp_mmc.bin     # BL2 + boot param header for eMMC
build/v2n/release/bl2_bp_mmc.srec    # same, S-record format
build/v2n/release/bl2_bp_spi.bin     # BL2 + boot param header for SPI/QSPI flash
build/v2n/release/bl2_bp_spi.srec    # same, S-record format
build/v2n/release/fip.bin            # FIP image (BL31 + BL33), and fip.srec
```

Choose the `mmc` or `spi` variant depending on your boot device.

## Note: `RZ_V2N_DDR_PROBE=1` — in-BL2 DDR sanity test

BL2 contains an optional DDR calibration/sanity test
(`plat/renesas/rz/soc/v2n/plat_ddr_setup.c`). It is **disabled by
default** (`RZ_V2N_DDR_PROBE ?= 0` in
`plat/renesas/rz/soc/v2n/platform.mk`) because it adds tens of seconds to
boot and floods the console — DDR calibration is already complete for the
production SoM.

To re-enable it for DDR bring-up / debugging, add `RZ_V2N_DDR_PROBE=1` to
the make command line:

```sh
make distclean && \
make PLAT=v2n BOARD=sr_som \
     BL33=<full_path>/u-boot/u-boot.bin \
     RZ_V2N_DDR_PROBE=1 \
     bl2 fip bptool pkg
```

When enabled, BL2 runs four phases over the usable DDR range
(`0x48000000`..`0x140000000`) and prints results to the console:

1. **16-MiB dense sweep** — quick smoke test, one write/read per 16 MiB.
   Catches gross addressing failures.
2. **Full-range Stuck-Address** — writes each 64-byte cache line with its
   own physical address and reads it back. Flags row/column miswires and
   address-line aliasing (memtester "Stuck Address" equivalent).
3. **Full-range pattern stress** — writes `0xAAAA…`/`0x5555…`/`0xFFFF…`
   across the full range, three iterations each before verify. Sustained
   back-to-back traffic that exposes refresh / VREF / thermal marginality.
4. **Walking-ones / walking-zeros** at a mid-DDR cache line — data-line
   integrity bit by bit.

Each phase reports its error count (`… — clean` when zero), finishing
with `DDR probe: all phases complete`. A pass here that still fails a
Linux-side `memtester` points to corruption exposed only by Linux access
patterns (cache eviction, controller QoS) — a stronger PHY/QoS-tuning
case for Renesas.

Rebuild **without** the flag (or with `RZ_V2N_DDR_PROBE=0`) for normal /
production builds.
