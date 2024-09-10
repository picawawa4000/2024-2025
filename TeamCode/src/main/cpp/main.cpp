//Don't forget to configure the file settings in CMakeLists.txt when adding a library!

#include <jni.h>
#include <libcardinal.hpp>
#include <camera.hpp>

//Entry point for the program.
void run(JNIEnv * env, jobject thiz) {

}

//Everything below here is a JNI wrapper function. Don't touch unless adding or removing one.

extern "C" JNIEXPORT void JNICALL Java_org_firstinspires_ftc_teamcode_RemoteOperation_run(
        JNIEnv *env, jobject thiz) {
    run(env, thiz);
}