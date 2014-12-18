#!/bin/sh
mkdir build && cd build

cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_CMAKE/android.toolchain.cmake -DANDROID_ABI=x86 ..
make

cd ../ && rm -r build && mkdir build && cd build
