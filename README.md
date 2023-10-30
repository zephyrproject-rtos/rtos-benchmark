# README

The benchmark project contains a set of tests aimed to measure the performance
of certain OS operations. It currently supports both the qemu_x86 and frdm_k64f
boards on Zephyr and only the frdm_k64f board on FreeRTOS. Additional boards
and RTOSes are expected to be added in the future.

It is recognized that running a benchmark test suite on QEMU is not generally
recommended, and any results from that should be taken with a grain of salt.
That being said, the primary reason it has been added has been to act as a
blueprint for integrating additional boards and architectures.

## Setting Up

### Common

Install the GNU ARM compiler for the `frdm_k64f` board,
[GNU ARM Embedded toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)
and set `ARMGCC_DIR` to the installation directory.

### Zephyr Specifics

Refer to the [Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/getting_started/index.html)
for installing and setting up the Zephyr environment. This must be done so that
a freestanding application can be built. As indicated under "Common", be sure
to set the `ARMGCC_DIR` environment variable appropriately.

To enable use of the 3rd Party GNU ARM Embedded toolchain, set the following
environment variables.

```
export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
export GNUARMEMB_TOOLCHAIN_PATH=<path to installed toolchain>
```

### FreeRTOS Specifics

Build and download the FRDM-K64F SDK from the [NXP MCUXpresso SDK Builder](https://mcuxpresso.nxp.com/en/welcome).
Select `Linux` as your host OS and `GCC ARM Embedded` as your toolchain.
Be sure to also check the `FreeRTOS` checkbox to build FreeRTOS
awareness and examples into the SDK.

```
mkdir freertos/SDK_<version>_FRDM-K64F
unzip ~/Downloads/SDK_<version>_FRDM-K64F.zip -d <SDK Install Dir>/SDK_<version>_FRDM-K64F
```

Install [pyOCD](https://github.com/pyocd/pyOCD) - tool used to flash FRDM_K64F.

### VxWorks Specifics

Install VxWorks version later than 24.03 (including).

## Building and Flashing

### Zephyr on FRDM K64F

```
cmake -GNinja -DRTOS=zephyr -DBOARD=frdm_k64f -S . -B build
ninja -C build
ninja -C build flash
```

#### Zephyr on other boards

Similar steps apply to when building for a different board. In the `cmake`
command, specify the name of the board after `-DBOARD`. For example ...

```
cmake -GNinja -DRTOS=zephyr -DBOARD=qemu_x86 -S . -B build
```

Remember that the `ZEPHYR_BASE` environment variable must be set so that the
Zephyr `west` tool can be found.

### FreeRTOS on FRDM K64F

```
cmake -GNinja -DRTOS=freertos -DBOARD=frdm_k64f -DMCUX_SDK_PATH=<SDK Install Dir>/SDK_<version>_FRDM-K64F -S . -B build
ninja -C build
ninja -C build flash
```

### VxWorks

VxWorks supports to run rtos-benchmark for either POSIX interfaces,
or non-POSIX interfaces at user space on difference boards.
For example with the BSP `nxp_s32g274`:

#### Create and build VSB

```
vxprj vsb create -force -S -smp -lp64 -bsp nxp_s32g274 vsb_nxp_s32g274
cd vsb_nxp_s32g274
vxprj vsb config -s -add _WRS_CONFIG_BENCHMARKS=y -add _WRS_CONFIG_BENCHMARKS_RTOS_BENCHMARK=y
make
```

#### Create and build VIP

The followings are the steps for POSIX interfaces. To test non-POSIX interfaces,
just replace `INCLUDE_RTOS_BENCHMARK_POSIX` with `INCLUDE_RTOS_BENCHMARK_NONPOSIX`,
and replace `rtos_benchmark_posix.vxe` with `rtos_benchmark_non_posix.vxe`.

```
vxprj create -vsb </path/to/vsb_nxp_s32g274> vip_nxp_s32g274
cd vip_nxp_s32g274
vxprj component add INCLUDE_RTOS_BENCHMARK_POSIX
mkdir romfs
cp </path/to/vsb_nxp_s32g274/usr/root/llvm/bin/rtos_benchmark_posix.vxe> romfs/
vxprj parameter set RTOS_BENCHMARK_OPTIONS 1
vxprj parameter set HIGH_RES_POSIX_CLOCK TRUE (Needed when POSIX clock is used for timestamping)
vxprj component remove INCLUDE_NETWORK
vxprj build
```

Load the image on the target, rtos-benchmark will automatically run on bootup.

## Connecting

Connect the `frdm_k64f` to your host via USB. In another terminal, open
a serial terminal (such as screen, minicom, etc) to see the output:

```
minicom -D /dev/ttyACM0
```

## Debugging

Debugging on both Zephyr and FreeRTOS are quite similar.

```
cmake <RTOS Debug Options>
ninja -C build debugserver
```

In another terminal, start GDB:

```
$ARMGCC_DIR/bin/arm-none-eabi-gdb build/freertos.elf
(gdb) target remote :3333
```

### cmake Zephyr Debug Options

Nothing extra--just the standard `cmake` instruction.

```
cmake -GNinja -DRTOS=zephyr -DBOARD=frdm_k64f -S . -B build
```

### cmake FreeRTOS Debug Options

Add `-DCMAKE_BUILD_TYPE=debug` to the build options:

```
cmake -GNinja -DCMAKE_BUILD_TYPE=debug -DRTOS=freertos -DBOARD=frdm_k64f -DMCUX_SDK_PATH=SDK_<version>_FRDM-K64F -S . -B build
```
