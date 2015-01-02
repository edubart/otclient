#!/bin/sh
mkdir ../build && cd ../build

cmake -DCMAKE_TOOLCHAIN_FILE=../android/android.toolchain.cmake -DANDROID_ABI=armeabi-v7a -DANDROID_NATIVE_API_LEVEL=android-16 -DANDROID_TOOLCHAIN_NAME=arm-linux-androideabi-4.6 ..
make

cd ../

cp -r libs android/project/ && rm -r libs

cd android/project

android update project -p . --name OTClientMob --target android-16

ant debug

cd bin

adb install -r OTClientMob-debug.apk
