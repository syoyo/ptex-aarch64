#!/bin/bash

rm -rf build-aarch64-cross
mkdir build-aarch64-cross

# Set path to aarch64 build of zlib 
PTEX_ZLIB_PATH=$HOME/local/aarch64-cross

cmake -Bbuild-aarch64-cross -H. \
  -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
  -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ \
  -DPTEX_ZLIB_PATH=${PTEX_ZLIB_PATH}
