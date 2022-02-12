# README

The benchmark project contains a set of tests aimed to measure the
performance of certain OS operations. It currently supports Zephyr
and FreeRTOS (partial). Tests are expected to be run on a
FRDM-K64F board.

## Zephyr

Note that Zephyr must be available on the system, so that a freestanding
application can be built. `ZEPHYR_BASE` environment variable must be set
in order to make flash options available.

To build the benchmark for Zephyr, one can use:

```
cmake -GNinja -DRTOS=zephyr -DTEST=<test> -DBOARD=<board> -S . -B build
```

That will generate the build files, for `ninja`, in the `build` directory.

* `<test>` is which test to run (check `AVAILABLE_TESTS` variables in
`CMakeLists.txt` to see available tests).

* `<board>` is the target board.

Then, to build one can use:

```
ninja -C build
```

And to flash (environment variable `ZEPHYR_BASE` must be set, so that
Zephyr `west` tool can be found):

```
ninja -C build flash
```

Note that one can also use `qemu` targets for Zephyr. For instance:

```
cmake -GNinja -DRTOS=zephyr -DTEST=<test> -DBOARD=qemu_x86 -S . -B build
ninja run
```

### Zephyr on FRDM K64F

Connect the `frdm_k64f` to your host via USB. In another terminal, open
a serial terminal (such as screen, minicom, etc) to see the output:

```
minicom -D /dev/ttyACM0
```

To build and flash:

```
ninja -C build flash
```

## FreeRTOS on FRDM_K64F


### Setup

Install the GNU ARM compiler for the `frdm_k64f` board,
[GNU ARM Embedded toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)
and set `ARMGCC_DIR` to the installation directory.


Build and download the FRDM-K64F SDK from the [NXP MCUXpresso SDK Builder](https://mcuxpresso.nxp.com/en/welcome).
Select Linux as your host OS and GCC ARM Embedded as your toolchain.
Be sure to also check the FreeRTOS checkbox to build FreeRTOS
awareness and examples into the SDK.

```
mkdir freertos/SDK_<version>_FRDM-K64F
unzip ~/Downloads/SDK_<version>_FRDM-K64F.zip -d freertos/SDK_<version>_FRDM-K64F
```

Install [pyOCD](https://github.com/pyocd/pyOCD) - tool used to flash FRDM_K64F.

### Build

To build the benchmark for FreeRTOS, one can use:

```
cmake -GNinja -DRTOS=freertos -DMCUX_SDK_PATH=SDK_<version>_FRDM-K64F -DTEST=<test> -DBOARD=frdm_k64f -S . -B build
```

That will generate the build files, for `ninja`, in the `build` directory.
Note that only `FRDM K64F` is supported for FreeRTOS.

* `<test>` is which test to run (check `AVAILABLE_TESTS` variables in
`CMakeLists.txt` to see available tests).

Then, to build one can use:

```
ninja -C build
```

And to flash (note that `pyOCD` must be installed):

```
ninja -C build flash
```

### Debug

To build the a debug version of the benchmark for FreeRTOS, one can add
`-DCMAKE_BUILD_TYPE=debug` to the build options:

```
cmake -GNinja -DCMAKE_BUILD_TYPE=debug -DRTOS=freertos -DMCUX_SDK_PATH=SDK_<version>_FRDM-K64F -DTEST=<test> -DBOARD=frdm_k64f -S . -B build
```

Then run the `debugserver` target:

```
ninja -C build debugserver
```

In another terminal, start GDB:

```
$ARMGCC_DIR/bin/arm-none-eabi-gdb build/freertos.elf
(gdb) target remote :3333
```
