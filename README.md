# README

The following instructions were written on Ubuntu 18.04 with Zephyr 2.6 and Zephyr SDK 12.3 and may require modification for your development environment.

## Setup

To use the GNU ARM compiler for the `frdm_k64f` board, install [GNU ARM Embedded toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm) and set appropriate environment variables following Zephyr Project directions on [3rd Party Toolchains](https://docs.zephyrproject.org/latest/getting_started/toolchain_3rd_party_x_compilers.html#third-party-x-compilers). It's highly recommended you install it in your home directory (`~`).

## Zephyr

```
cd zephyr
```

To change the test being built and run, comment in the appropriate line in `CMakeLists.txt`. For example, to run the basic `Hello World` benchmark, comment in the below line:

```
...
# target_sources(app PRIVATE ${SRC_DIR}/bench_sem_signal_release_test.c)
# target_sources(app PRIVATE ${SRC_DIR}/bench_thread_switch_yield_test.c)
# target_sources(app PRIVATE ${SRC_DIR}/bench_thread_test.c)
target_sources(app PRIVATE ${SRC_DIR}/bench_hello_world.c)
```

### Zephyr on QEMU

QEMU is useful for building and debugging the benchmark application quickly. You'll need to reset `ZEPHYR_TOOLCHAIN_VARIANT` back to `zephyr` if you were using `gnuarmemb` previously.

To build and run:

```
west build -b qemu_x86 -t run
```

You can see the output directly in the terminal.

### Zephyr on FRDM K64F

Connect the `frdm_k64f` to your host via USB. In another terminal, open Minicom to see the output:

```
minicom -D /dev/ttyACM0
```

To build and flash:

```
west build -b frdm_k64f -t flash
```

## FreeRTOS on FRDM_K64F

Build and download version 2.8.2 of the FRDM-K64F SDK from the [NXP MCUXpresso SDK Builder](https://mcuxpresso.nxp.com/en/welcome). Select Linux as your host OS and GCC ARM Embedded as your toolchain. Be sure to also check the FreeRTOS checkbox to build FreeRTOS awareness and examples into the SDK. 

```
mkdir freertos/SDK_2.8.2_FRDM-K64F
unzip ~/Downloads/SDK_2_8_0_FRDM-K64F.zip -d freertos/SDK_2.8.2_FRDM-K64F
```

Download the [Segger J-Link firmware](https://www.segger.com/downloads/jlink/OpenSDA_FRDM-K64F) and flash it onto the `frdm_k64f`. You can do this by holding the reset button while plugging in the USB cable to enter bootloader mode and dragging and dropping the firmware onto the `BOOTLOADER` drive.

Download and install the [Segger J-Link software and documentation pack](https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack) (recommended into `/opt/`).

Navigate into the `freertos` directory.

```
cd freertos/
```

### Build

To change the sample being built, edit `freertos/armgcc/CMakeLists.txt:372`:

```
...
add_executable(${PROJECT_NAME}.elf 

"${BenchDirPath}/bench_sem_signal_release_test.c" # Change me
"${ProjDirPath}/../bench_porting_layer_freertos.c"
...
```

Set `ARMGCC_DIR` to make CMake build with the toolchain:

```
export ARMGCC_DIR=$HOME/gnu_arm_embedded
```

Build and flash (check paths in `shortcut.sh` to be sure they'll work for your JLink and GNU ARM toolchain install):

```
./shortcut.sh
```

You can also build and flash yourself:

```
# Build
./build_debug.sh
# Copy to binary format
~/gnu_arm_embedded/bin/arm-none-eabi-objcopy -S -O binary ./debug/freertos.elf ./${TYPE}/freertos.bin
# Flash using commands in CommandFile.jlink (for JLink CLI)
/opt/SEGGER/JLink/JLinkExe -device MK64FN1M0XXX12 -if SWD -NoGui 1 -Speed auto -CommandFile CommandFile.jlink
```

To clean up the generated files, you can use the ./clean.sh command. 

### Debug

Once you've built the application and copied to .bin, start the JLink GDB Server:

```
/opt/SEGGER/JLink/JLinkGDBServer ./debug/freertos.elf -device MK64FN1M0XXX12
```

In another terminal, start GDB:

```
~/gnu_arm_embedded/bin/arm-none-eabi-gdb ./debug/freertos.elf
```

# TODO
- Change from commenting CMakeLists.txt to shell script (?)
- Adjust tests for any changes in benchmarking suite since Fall 2020
- Recheck Kconfig options since Fall 2020
- Update FRDM-K64F SDK to 2.9 (release 1/15/21)
- Clean up CMakeLists.txt file copied and modified from examples in SDK to reduce repetition and length
- Fix PRINTF for FreeRTOS
- Figure out why debugging is broken on FRDM???
