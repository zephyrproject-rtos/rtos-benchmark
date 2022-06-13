RTEMS uses the WAF build system. This project has not yet integrated it
in with CMAKE.

To build this project with RTEMS, please do the following.

Part 1 : Setup
--------------
1. Download the WAF tool into the current directory
   curl https://waf.io/waf-2.0.19 > waf
   chmod +x waf
2. Add RTEMS WAF support as a submodule
   git submodule add git://git.rtems.org/rtems_waf.git rtems_waf
3. Set environment variables. Actual install paths may vary.
   export PATH=$HOME/RTEMS/rtems/6/bin:"${PATH}"
   export PREFIX=$HOME/RTEMS/rtems/6

Part 2 : Configuring and Building
---------------------------------
3. Configure the project. Only riscv/rv32i BSP is currently supported.
   ./waf configure --rtems=$PREFIX --rtems-bsp=riscv/rv32i
4. Build
   ./waf

Part 3 : Executing
------------------
5. Execute the final image using Zephyr's QEMU for RISCV binary.
  <zephyr SDK path>/sysroots/x86_64-pokysdk-linux/usr/bin/qemu-system-riscv32 \
      -nographic -machine virt -bios none -m 256 -net none -pidfile qemu.pid \
      -chardev stdio,id=con,mux=on -serial chardev:con \
      -mon chardev=con,mode=readline -icount shift=6,align=off,sleep=off \
      -rtc clock=vm -kernel build/riscv-rtems6-rv32i/rtos-benchmark.elf
