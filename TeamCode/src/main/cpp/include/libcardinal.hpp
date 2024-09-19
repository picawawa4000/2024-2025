#pragma once
#ifndef LIBCARDINAL_HPP
#define LIBCARDINAL_HPP

#include <jni.h>

#include <string>
#include <map>
#include <future>
#include <thread>
#include <chrono>
#include <exception>
#include <locale>
#include <codecvt>
#include <vector>

//Some of the stuff in here is unused and (maybe) slated for removal.
//Most of it is actually quite helpful.

namespace libcardinal {
//The JNI environment and Java virtual machine that libcardinal will use

    inline JNIEnv * jnienv;
    inline JavaVM * javavm;

//Helper functions to set, get, and initialise the VM and environment

/*If you are using a function called from Java, use the environment that Java provides you*/
    void setenv(JNIEnv *env);
    JNIEnv * getenv();
    void setvm(JavaVM *vm);
    JavaVM * getvm();

/*void initenv() {
    JavaVMInitArgs args;
    args.version = JNI_VERSION_1_2;
    args.nOptions = 0;
    args.ignoreUnrecognized = 1;
    JNI_CreateJavaVM(&libcardinal_javavm, (void **)&libcardinal::jnienv, (void *)(&args));
}*/

    void terminate();

//Java interface functions for generic usage

    jvalue to_jvalue(jobject l);
    jvalue to_jvalue(jint i);
    jvalue to_jvalue(jbyte b);
    jvalue to_jvalue(jchar c);
    jvalue to_jvalue(jdouble d);
    jvalue to_jvalue(jfloat f);
    jvalue to_jvalue(jlong j);
    jvalue to_jvalue(jshort s);
    jvalue to_jvalue(jboolean z);

    jobject new_instance(const char *id, const char *constructor_sig, const jvalue *constructor_args);
    jobject new_instance(const char *id, const char *constructor_sig, jvalue constructor_args...);
    jobject altenv_new_instance(JNIEnv *env, const char *id, const char *constructor_sig, const jvalue *constructor_args);
    jobject altenv_new_instance(JNIEnv *env, const char *id, const char *constructor_sig, jvalue constructor_args...);

    jobject alloc_instance(const char *id);
    jobject altenv_alloc_instance(JNIEnv *env, const char *id);

    jvalue get_field(jobject target, const char *id, const char *object_sig);
    jvalue altenv_get_field(JNIEnv *env, jobject target, const char *id, const char *object_sig);
    jvalue get_static_field(jclass target, const char *id, const char *object_sig);
    jvalue altenv_get_static_field(JNIEnv * env, jclass target, const char *id, const char *object_sig);
    template<typename ArrayOf> jarray get_array_field(jobject target, const char *id, const char *object_sig);
    template<typename ArrayOf> jarray altenv_get_array_field(JNIEnv *env, jobject target, const char *id, const char *object_sig);

    jvalue call_instance(jobject target, const char *name, const char *sig, jvalue * args);
    jvalue call_instance(jobject target, const char *name, const char *sig, jvalue args...);
    jvalue altenv_call_instance(JNIEnv *env, jobject target, const char *name, const char *sig, jvalue *args);
    jvalue altenv_call_instance(JNIEnv *env, jobject target, const char *name, const char *sig, jvalue args...);
    void call_void_instance(jobject target, const char *name, const char *sig, jvalue * args);
    void call_void_instance(jobject target, const char *name, const char *sig, jvalue args...);
    void altenv_call_void_instance(JNIEnv *env, jobject target, const char *name, const char *sig, jvalue *args);
    void altenv_call_void_instance(JNIEnv *env, jobject target, const char *name, const char *sig, jvalue args...);
    jobject call_nonvirtual(jobject target, const char *name, const char *sig, jvalue *args);
    jobject call_nonvirtual(jobject target, const char *name, const char *sig, jvalue args...);
    jobject altenv_call_nonvirtual(JNIEnv *env, jobject target, const char *name, const char *sig, jvalue *args);
    jobject altenv_call_nonvirtual(JNIEnv *env, jobject target, const char *name, const char *sig, jvalue args...);
    jvalue call_static(jobject target, const char *name, const char *sig, jvalue *args);
    jvalue call_static(jobject target, const char *name, const char *sig, jvalue args...);
    jvalue altenv_call_static(JNIEnv *env, jobject target, const char *name, const char *sig, jvalue *args);
    jvalue call_static(jclass target, const char *name, const char *sig, jvalue args...);
    jvalue altenv_call_static(JNIEnv *env, jclass target, const char *name, const char *sig, jvalue *args);
    jvalue altenv_call_static(JNIEnv *env, jclass target, const char *name, const char *sig, jvalue args...);

    std::string get_enum_name(jobject target);
    std::string altenv_get_enum_name(JNIEnv *env, jobject target);

//FTC-specific functions

    jobject get_telemetry(jobject opMode);
    jobject altenv_get_telemetry(JNIEnv *env, jobject opMode);
    jobject get_hardware_map(jobject opMode);
    jobject altenv_get_hardware_map(JNIEnv *env, jobject opMode);

//Reports telemetry for the specified opmode
//Only accepts strings for the value
    void report_telemetry_for(jobject opMode, const std::string &key, const std::string &value);
    void altenv_report_telemetry_for(JNIEnv *env, jobject opMode, const std::string &key, const std::string &value);
    void report_telemetry_for(jobject opMode, const std::string& key, jvalue value);
    void altenv_report_telemetry_for(JNIEnv * env, jobject opMode, const std::string& key, jvalue value);

    void log(jobject logger, const std::string& message);
    void altenv_log(JNIEnv * env, jobject logger, const std::string& message);

    jobject get_device_from_hardware_map(jobject opMode, const std::string &id, const std::string &idClass);
    jobject altenv_get_device_from_hardware_map(JNIEnv *env, jobject opMode, const std::string &id, const std::string &idClass);
    jobject get_device_from_hardware_map(jobject opMode, const std::string &id);
    jobject altenv_get_device_from_hardware_map(JNIEnv *env, jobject opMode, const std::string &id);

    void kill(jobject opMode);
    void altenv_kill(JNIEnv * env, jobject opMode);
}
//Tests

/*
An example of how to interface with the LinearOpMode from C++
Notice that, in the Java file, we only have a call to System.loadLibrary()
and a declaration of runOpMode() plus some camera functions (see next
example); all the rest is done in C++
*/
/*
extern "C" JNIEXPORT void
JNICALL  __attribute__((unused))
Java_org_firstinspires_ftc_teamcode_libcardinal_runOpMode(JNIEnv *env, jobject thiz) {
    //Set up libcardinal's JNI environment so libcardinal works correctly
    libcardinal::setenv(env);
    //Set up a TelemetryStream
    //Not really necessary, but it's useful when there are large amounts of telemetry
    libcardinal::TelemetryStream out(thiz);
    //Call waitForStart() like a good FTC programmer
    //If the method returned, we would use call_instance
    libcardinal::call_void_instance(thiz, "waitForStart", "()V", nullptr);
    //Report some telemetry
    out << "Output from native: 5";
    //libcardinal::report_telemetry_for(thiz, "Output from native", "5");
}
*/

/*
An example of how to interact with hardware devices from C++
(I inadvertently chose the worst one)
*/

/*
jobject cameraCharacteristics;
jobject camera;
jobject serialThreadPool;

extern "C" JNIEXPORT void
JNICALL  __attribute__((unused))
Java_org_firstinspires_ftc_teamcode_CameraTest_runOpMode(JNIEnv *env, jobject thiz) {
    libcardinal::setenv(env);
    jobject cameraName = libcardinal::get_device_from_hardware_map(thiz, "webcam");
    jobject cameraManager = libcardinal::call_instance(
            libcardinal::call_static(
                    env->FindClass("org/firstinspires/ftc/robotcore/external/ClassFactory"),
                    "getInstance",
                    "()Lorg/firstinspires/ftc/robotcore/external/ClassFactory;",
                    nullptr
                    ).l,
            "getCameraManager",
            "()Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraManager;",
            nullptr
            ).l;
    jvalue cargs[2] = {
            {.j=(jlong) 5000},
            libcardinal::get_static_field(
                    env->FindClass("java/util/concurrent/TimeUnit"),
                    "MILLISECONDS",
                    "Ljava/util/concurrent/TimeUnit;"
                    )
    };
    jvalue args[3] = {
            {.l=libcardinal::new_instance(
                    "org/firstinspires/ftc/robotcore/internal/system/Deadline",
                    "(JLjava/util/concurrent/TimeUnit;)V",
                    cargs
            )},
            {.l=cameraName},
            {.l=(jobject)nullptr} //NULL i think
    };
    camera = env->NewGlobalRef(libcardinal::call_instance(
            cameraManager,
            "requestPermissionAndOpenCamera",
            "(Lorg/firstinspires/ftc/robotcore/internal/system/Deadline;Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraName;Lorg/firstinspires/ftc/robotcore/external/function/Continuation;)Lorg/firstinspires/ftc/robotcore/external/hardware/camera/Camera;",
            args).l);
    camera = env->NewGlobalRef(libcardinal::call_instance(
            cameraManager,
            "requestPermissionAndOpenCamera",
            "(Lorg/firstinspires/ftc/robotcore/internal/system/Deadline;Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraName;Lorg/firstinspires/ftc/robotcore/external/function/Continuation;)Lorg/firstinspires/ftc/robotcore/external/hardware/camera/Camera;",
            libcardinal::to_jvalue(
                    libcardinal::new_instance(
                    "org/firstinspires/ftc/robotcore/internal/system/Deadline",
                    "(JLjava/util/concurrent/TimeUnit;)V",
                    cargs)),
            libcardinal::to_jvalue(cameraName),
            libcardinal::to_jvalue(nullptr)
    ).l);
    cameraCharacteristics = env->NewGlobalRef(
            libcardinal::call_instance(
                libcardinal::call_instance(
                        camera,
                        "getCameraName",
                        "()Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraName;",
                        nullptr
                        ).l,
                "getCameraCharacteristics",
                "()Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraCharacteristics;",
                nullptr
                ).l
            );
    libcardinal::call_void_instance(thiz, "waitForStart", "()V", nullptr);
    if (camera == nullptr) {
        libcardinal::report_telemetry_for(thiz, "Camera is", "Broken");
        return;
    }
    serialThreadPool = env->NewGlobalRef(libcardinal::call_instance(
            cameraManager,
            "getSerialThreadPool",
            "()Ljava/util/concurrent/Executor;",
            nullptr
            ).l);
    jvalue dargs[2] = {
            {.l=serialThreadPool},
            {
                .l=libcardinal::alloc_instance(
                        "org/firstinspires/ftc/teamcode/libcardinal$StateCallback"
                        )
            }
    };
    jvalue largs[1] = {
                libcardinal::call_static(
                        env->FindClass("org/firstinspires/ftc/robotcore/external/function/Continuation"),
                        "create",
                        "(Ljava/util/concurrent/Executor;Ljava/lang/Object;)Lorg/firstinspires/ftc/robotcore/external/function/Continuation;",
                        dargs
                        )
    };
    jobject session = libcardinal::call_instance(
            camera,
            "createCaptureSession",
            "(Lorg/firstinspires/ftc/robotcore/external/function/Continuation;)Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraCaptureSession;",
            largs
            ).l;
    libcardinal::report_telemetry_for(thiz, "Status", "So far, so good...");
    std::this_thread::sleep_for(std::chrono::milliseconds(15000));
    env->DeleteGlobalRef(cameraCharacteristics);
    env->DeleteGlobalRef(camera);
    env->DeleteGlobalRef(serialThreadPool);
}

//Process a frame from the camera

extern "C" JNIEXPORT void JNICALL  __attribute__((unused))
Java_org_firstinspires_ftc_teamcode_libcardinal_00024CaptureCallback_onNewFrame(JNIEnv * env, jobject thiz, jobject session, jobject request, jobject frame) {

}

//Set up the camera more

extern "C" JNIEXPORT void JNICALL  __attribute__((unused))
Java_org_firstinspires_ftc_teamcode_libcardinal_00024StateCallback_onConfigured(JNIEnv * env, jobject thiz, jobject session) {
    jvalue args[1] = {
            libcardinal::altenv_get_static_field(
                    env,
                    env->FindClass("org/firstinspires/ftc/robotcore/internal/vuforia/externalprovider/FrameFormat"),
                    "YUYV",
                    "Lorg/firstinspires/ftc/robotcore/internal/vuforia/externalprovider/FrameFormat;"
            )
    };
    jvalue dargs[2] = {
            {.i=(jint)640},
            {.i=(jint)480}
    };
    jvalue cargs[2] = {
            libcardinal::altenv_call_static(
                    env,
                    env->FindClass("org/firstinspires/ftc/robotcore/internal/camera/ImageFormatMapper"),
                    "androidFromVuforiaWebcam",
                    "(Lorg/firstinspires/ftc/robotcore/internal/vuforia/externalprovider/FrameFormat;)I",
                    args
                    ),
            {
                .l=libcardinal::altenv_new_instance(
            env,
            "org/firstinspires/ftc/robotcore/external/android/util/Size",
            "(II)V",
            dargs
            )
            }
    };
    jvalue jargs[4] = {
            {.i=(jint) 640},
            {.i=(jint) 480},
            libcardinal::altenv_call_instance(
                    env,
                    cameraCharacteristics,
                    "getMaxFramesPerSecond",
                    "(ILorg/firstinspires/ftc/robotcore/external/android/util/Size;)I",
                    cargs
                    ),
            libcardinal::altenv_get_static_field(
                    env,
                    env->FindClass("org/firstinspires/ftc/robotcore/internal/vuforia/externalprovider/FrameFormat"),
                    "YUYV",
                    "Lorg/firstinspires/ftc/robotcore/internal/vuforia/externalprovider/FrameFormat;"
            )
    };
    jobject streamingMode = libcardinal::altenv_new_instance(
            env,
            "org/firstinspires/ftc/robotcore/internal/vuforia/externalprovider/CameraMode",
            "(IIILorg/firstinspires/ftc/robotcore/internal/vuforia/externalprovider/FrameFormat;)V",
            jargs
            );
    jvalue fargs[3] = {
            libcardinal::altenv_call_instance(
                    env,
                    streamingMode,
                    "getAndroidFormat",
                    "()I",
                    nullptr
                    ),
            libcardinal::altenv_call_instance(
                    env,
                    streamingMode,
                    "getSize",
                    "()Lorg/firstinspires/ftc/robotcore/external/android/util/Size;",
                    nullptr
                    ),
            libcardinal::altenv_call_instance(
                    env,
                    streamingMode,
                    "getFramesPerSecond",
                    "()I",
                    nullptr
                    )
    };
    jobject cameraCaptureRequest = libcardinal::altenv_call_instance(
            env,
            camera,
            "createCaptureRequest",
            "(ILorg/firstinspires/ftc/robotcore/external/android/util/Size;I)Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraCaptureRequest;",
            fargs
            ).l;
    jvalue yargs[2] {
            {.l=serialThreadPool},
            {.l=libcardinal::altenv_alloc_instance(env, "org/firstinspires/ftc/teamcode/libcardinal$StatusCallback")}
    };
    jvalue pargs[3] {
            {.l=cameraCaptureRequest},
            {.l=libcardinal::altenv_alloc_instance(env, "org/firstinspires/ftc/teamcode/libcardinal$CaptureCallback")},
            libcardinal::altenv_call_static(
                    env,
                    env->FindClass("org/firstinspires/ftc/robotcore/external/function/Continuation"),
                    "create",
                    "(Ljava/util/concurrent/Executor;Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraCapureSession$StatusCallback;)Lorg/firstinspires/ftc/robotcore/external/function/Continuation;",
                    yargs
            )
    };
    libcardinal::altenv_call_instance(
            env,
            session,
            "startCapture",
            "(Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraCaptureRequest;Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraCaptureSession$CaptureCallback;Lorg/firstinspires/ftc/robotcore/external/function/Continuation;)Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraCaptureSequenceId;",
            pargs
            );
}
*/

#endif