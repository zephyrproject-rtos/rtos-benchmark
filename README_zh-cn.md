
\[ [English](README.md) | [简体中文] \]

# README

该基准测试项目包含一组测试，旨在测量某些操作系统操作的性能。目前，它支持 Zephyr 上的 qemu_x86 和 frdm_k64f 开发板，以及 FreeRTOS 上的 frdm_k64f 开发板。未来预计会添加更多的开发板和 RTOS 支持。

需要注意的是，在 QEMU 上运行基准测试套件通常不被推荐，其结果应谨慎对待。尽管如此，添加 QEMU 支持的主要目的是提供一个集成其他开发板和架构的蓝本。

## 设置环境

### 通用设置

为 `frdm_k64f` 开发板安装 GNU ARM 编译器，
[GNU ARM Embedded toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)，
并将 `ARMGCC_DIR` 设置为安装目录。

### Zephyr 相关设置

请参考 [Zephyr 入门指南](https://docs.zephyrproject.org/latest/getting_started/index.html) 以安装和配置 Zephyr 环境。这是构建独立应用程序所必需的。正如“通用设置”部分所述，请确保正确设置 `ARMGCC_DIR` 环境变量。

要启用第三方 GNU ARM Embedded 工具链，请设置以下环境变量：

```
export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
export GNUARMEMB_TOOLCHAIN_PATH=<path to installed toolchain>
```

### FreeRTOS 相关设置

从 [NXP MCUXpresso SDK Builder](https://mcuxpresso.nxp.com/en/welcome) 下载并构建 FRDM-K64F SDK。
选择 `Linux` 作为主机操作系统，并选择 `GCC ARM Embedded` 作为工具链。
此外，请确保勾选 `FreeRTOS` 选项，以在 SDK 中包含 FreeRTOS 支持和示例。

```
mkdir freertos/SDK_<version>_FRDM-K64F
unzip ~/Downloads/SDK_<version>_FRDM-K64F.zip -d <SDK Install Dir>/SDK_<version>_FRDM-K64F
```

安装 [pyOCD](https://github.com/pyocd/pyOCD) —— 该工具用于刷写 FRDM_K64F。

### VxWorks 相关设置

安装 VxWorks 24.03 及以上版本。

## 构建与刷写

### 在 FRDM K64F 上运行 Zephyr

```
cmake -GNinja -DRTOS=zephyr -DBOARD=frdm_k64f -S . -B build
ninja -C build
ninja -C build flash
```

#### 在其他开发板上运行 Zephyr

构建其他开发板时，步骤类似。在 `cmake` 命令中，在 `-DBOARD` 选项后指定开发板名称。例如：

```
cmake -GNinja -DRTOS=zephyr -DBOARD=qemu_x86 -S . -B build
```

请确保 `ZEPHYR_BASE` 环境变量已正确设置，以便找到 Zephyr 的 `west` 工具。

### 在 FRDM K64F 上运行 FreeRTOS

```
cmake -GNinja -DRTOS=freertos -DBOARD=frdm_k64f -DMCUX_SDK_PATH=<SDK Install Dir>/SDK_<version>_FRDM-K64F -S . -B build
ninja -C build
ninja -C build flash
```

### VxWorks

VxWorks 支持在不同的开发板上运行 rtos-benchmark，可选择 POSIX 接口或非 POSIX 接口的用户空间版本。
例如，针对 BSP `nxp_s32g274`：

#### 创建并构建 VSB

```
vxprj vsb create -force -S -smp -lp64 -bsp nxp_s32g274 vsb_nxp_s32g274
cd vsb_nxp_s32g274
vxprj vsb config -s -add _WRS_CONFIG_BENCHMARKS=y -add _WRS_CONFIG_BENCHMARKS_RTOS_BENCHMARK=y
make
```

#### 创建并构建 VIP

以下步骤适用于 POSIX 接口测试。如需测试非 POSIX 接口，
请将 `INCLUDE_RTOS_BENCHMARK_POSIX` 替换为 `INCLUDE_RTOS_BENCHMARK_NONPOSIX`，
并将 `rtos_benchmark_posix.vxe` 替换为 `rtos_benchmark_non_posix.vxe`。

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

将生成的镜像加载到目标设备，rtos-benchmark 将在启动时自动运行。

## 连接设备

通过 USB 将 `frdm_k64f` 连接到主机。在另一个终端中，打开串口终端（如 screen、minicom 等）以查看输出：

```
minicom -D /dev/ttyACM0
```

## 调试

Zephyr 和 FreeRTOS 的调试方法类似：

```
cmake <RTOS Debug Options>
ninja -C build debugserver
```

在另一个终端中启动 GDB：

```
$ARMGCC_DIR/bin/arm-none-eabi-gdb build/freertos.elf
(gdb) target remote :3333
```

### cmake Zephyr 调试选项

无需额外操作，只需标准的 cmake 指令。

```
cmake -GNinja -DRTOS=zephyr -DBOARD=frdm_k64f -S . -B build
```

### cmake FreeRTOS 调试选项
在构建选项中添加 -DCMAKE_BUILD_TYPE=debug：

```
cmake -GNinja -DCMAKE_BUILD_TYPE=debug -DRTOS=freertos -DBOARD=frdm_k64f -DMCUX_SDK_PATH=SDK_<version>_FRDM-K64F -S . -B build
```
