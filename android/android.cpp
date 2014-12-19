#include <jni.h>
#include <main.cpp>

void android_main( struct android_app* state ) {
    int argc = 0;
    const char* argv[1];
    argv[0] = NULL;

    main(argc, argv);
}
