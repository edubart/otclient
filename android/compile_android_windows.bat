cd ..
mkdir build_android 
cd build_android

cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../android/android.toolchain.cmake -DANDROID_ABI=armeabi-v7a -DANDROID_NATIVE_API_LEVEL=android-16 -DANDROID_TOOLCHAIN_NAME=arm-linux-androideabi-4.6 ..
make

cd ../

xcopy /E /Y libs android\project\libs

cd android\project

call android update project -p . --name OTClient --target android-16 
call ant debug

cd bin
adb install -r OTClient-debug.apk