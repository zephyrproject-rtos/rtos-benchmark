#!/bin/bash

SWITCH=run

TYPE=debug
./build_${TYPE}.sh
~/gnu_arm_embedded/bin/arm-none-eabi-objcopy -S -O binary ./${TYPE}/freertos.elf ./${TYPE}/freertos.bin

if [ ${SWITCH} = ${TYPE} ]; then
	/opt/SEGGER/JLink/JLinkGDBServer ./${TYPE}/freertos.elf -device MK64FN1M0XXX12
	# RUN THE FOLLOWING IN OTHER WINDOW / TAB
	# sudo minicom -D /dev/ttyACM0
else
	/opt/SEGGER/JLink/JLinkExe -device MK64FN1M0XXX12 -if SWD -NoGui 1 -Speed auto -CommandFile CommandFile.jlink
	# TODO: Fix issue in that CommandFile.jlink will always pick debug... can't pass type
	# RUN THE FOLLOWING IN OTHER WINDOW / TAB
	# sudo minicom -D /dev/ttyACM0
	# ~/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-gdb debug/freertos.elf 
fi