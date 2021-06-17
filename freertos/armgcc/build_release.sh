#!/bin/sh
cmake -DCMAKE_TOOLCHAIN_FILE="../SDK_2.8.2_FRDM-K64F/tools/cmake_toolchain_files/armgcc.cmake" -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=release  .
make -j 2>&1 | tee build_log.txt
