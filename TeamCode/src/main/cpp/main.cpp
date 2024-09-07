//Don't forget to configure the file settings in CMakeLists.txt when adding a library!

#include <jni.h>
#include <libcardinal.hpp>

void run(JNIEnv * env, jobject thiz) {

}

extern "C" JNIEXPORT void JNICALL Java_org_firstinspires_ftc_teamcode_RemoteOperation_run(
        JNIEnv *env, jobject thiz) {
    run(env, thiz);
}