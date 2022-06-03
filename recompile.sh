#!/bin/bash

if [ -d "build" ]
then
	echo "Directory 'build' already exists, moving to it"
	cd build
	echo "Clean build directory"
	rm -rf *
	echo "Configuring"
	cmake -DCMAKE_TOOLCHAIN_FILE=../../vcpkg/scripts/buildsystems/vcpkg.cmake ..
else
	mkdir "build" && cd build
	cmake -DCMAKE_TOOLCHAIN_FILE=../../vcpkg/scripts/buildsystems/vcpkg.cmake ..
fi

make -j$(nproc) || exit 1
if [ $? -eq 1 ]
then
	echo "Compilation failed!"
else
	echo "Compilation successful!"
fi
