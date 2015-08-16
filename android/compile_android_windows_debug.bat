cd ..
mkdir build_android 
cd build_android

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=../android/android.toolchain.cmake -DANDROID_ABI=armeabi-v7a -DANDROID_NATIVE_API_LEVEL=android-16 -DANDROID_TOOLCHAIN_NAME=arm-linux-androideabi-4.6 ..
make

cd ../

ECHO D|xcopy /E /Y android\project\jni\libSDL2.so android\project\libs\armeabi-v7a
xcopy /E /Y libs\armeabi-v7a\libotclient.so android\project\jni

cd android\project

call android update project -p . --name OTClient --target android-16 
call ndk-build.cmd all NDK_DEBUG=1
call ant clean
call ant debug

cd bin
adb install -r OTClient-debug.apk