#include <camera.hpp>

#include <libcardinal.hpp>
#include <functional>
#include <string>
#include <utility>
#include "libcardinal.hpp"

std::function<void(std::string)> output;

void set_logger(std::function<void(std::string)> logger) {
    output = std::move(logger);
}

Camera::Camera(JNIEnv *env, jobject opmode, int xsize, int ysize) : env(env), recording(false), userxsize(xsize), userysize(ysize) {
    //Gets the name of the camera, which is more or less an interface to the camera
    jobject cameraName = libcardinal::get_device_from_hardware_map(opmode, "webcam");

    //Creates a new camera manager, which manages our cameras
    this->cameraManager = env->NewGlobalRef(libcardinal::altenv_call_instance(
            env,
            libcardinal::altenv_call_static(
                    env,
                    env->FindClass("org/firstinspires/ftc/robotcore/external/ClassFactory"),
                    "getInstance",
                    "()Lorg/firstinspires/ftc/robotcore/external/ClassFactory;",
                    nullptr
            ).l,
            "getCameraManager",
            "()Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraManager;",
            nullptr
    ).l);

    //Actually creates the camera object
    this->camera = env->NewGlobalRef(libcardinal::altenv_call_instance(
            env,
            this->cameraManager,
            "requestPermissionAndOpenCamera",
            "(Lorg/firstinspires/ftc/robotcore/internal/system/Deadline;Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraName;Lorg/firstinspires/ftc/robotcore/external/function/Continuation;)Lorg/firstinspires/ftc/robotcore/external/hardware/camera/Camera;",
            libcardinal::to_jvalue(libcardinal::altenv_new_instance(
                    env,
                    "org/firstinspires/ftc/robotcore/internal/system/Deadline",
                    "(JLjava/util/concurrent/TimeUnit;)V",
                    libcardinal::to_jvalue((jlong)5000),
                    libcardinal::altenv_get_static_field(
                            env,
                            env->FindClass("java/util/concurrent/TimeUnit"),
                            "MILLISECONDS",
                            "Ljava/util/concurrent/TimeUnit;"
                    )
            )),
            libcardinal::to_jvalue(cameraName),
            libcardinal::to_jvalue(nullptr)
    ).l);

    //Not entirely sure why I need this
    this->cameraCharacteristics = env->NewGlobalRef(
            libcardinal::altenv_call_instance(
                    env,
                    libcardinal::altenv_call_instance(
                            env,
                            this->camera,
                            "getCameraName",
                            "()Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraName;",
                            nullptr
                    ).l,
                    "getCameraCharacteristics",
                    "()Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraCharacteristics;",
                    nullptr
            ).l
    );

    //Same as above
    this->serialThreadPool = this->env->NewGlobalRef(libcardinal::altenv_call_instance(
            this->env,
            this->cameraManager,
            "getSerialThreadPool",
            "()Ljava/util/concurrent/Executor;",
            nullptr
    ).l);
}

Camera::~Camera() {
    this->env->DeleteGlobalRef(this->cameraManager);
    this->env->DeleteGlobalRef(this->camera);
    this->env->DeleteGlobalRef(this->cameraCharacteristics);
    this->env->DeleteGlobalRef(this->serialThreadPool);
}

void Camera::record(std::function<void(JNIEnv*,int,int,std::vector<unsigned char>)> handler) {
    if (this->recording) {
        output("WARNING: Camera attempted to start second recording session.");
        return;
    }
    this->recording = true;

    this->frameHandler = std::move(handler);

    jobject stateCallback = libcardinal::altenv_new_instance(
            this->env,
            "org/firstinspires/ftc/teamcode/NativeInterfaces$StateInterface",
            "(J)V",
            libcardinal::to_jvalue((long)this)
    );

    this->currentCaptureSession = this->env->NewGlobalRef(libcardinal::altenv_call_instance(
            this->env,
            this->camera,
            "createCaptureSession",
            "(Lorg/firstinspires/ftc/robotcore/external/function/Continuation;)Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraCaptureSession;",
            libcardinal::altenv_call_static(
                    this->env,
                    this->env->FindClass("org/firstinspires/ftc/robotcore/external/function/Continuation"),
                    "create",
                    "(Ljava/util/concurrent/Executor;Ljava/lang/Object;)Lorg/firstinspires/ftc/robotcore/external/function/Continuation;",
                    libcardinal::to_jvalue(serialThreadPool),
                    libcardinal::to_jvalue(stateCallback)
            )
    ).l);
}

void Camera::stop() {
    libcardinal::altenv_call_void_instance(
            this->env,
            this->currentCaptureSession,
            "stop",
            "()V",
            nullptr
    );
    libcardinal::altenv_call_void_instance(
            this->env,
            this->currentCaptureSession,
            "close",
            "()V",
            nullptr
    );

    this->env->DeleteGlobalRef(this->currentCaptureSession);

    this->recording = false;
}

bool Camera::isRecording() const {
    return this->recording;
}

void Camera::onConfigured(JNIEnv *altenv, jobject cameraCaptureSession) {
    jobject size = libcardinal::altenv_new_instance(
            altenv,
            "org/firstinspires/ftc/robotcore/external/android/util/Size",
            "(II)V",
            libcardinal::to_jvalue(this->userxsize),
            libcardinal::to_jvalue(this->userysize)
    );
    jvalue androidImageFormat = libcardinal::altenv_get_static_field(
            altenv,
            altenv->FindClass("org/firstinspires/ftc/robotcore/internal/vuforia/externalprovider/FrameFormat"),
            //this may need to be changed
            "YUYV",
            "Lorg/firstinspires/ftc/robotcore/internal/vuforia/externalprovider/FrameFormat;"
    );

    //Gets the maximum frames per second for our desired camera configuration
    //(dynamic resolution, YUYV/YUY2 format)
    int maxFramesPerSecond = libcardinal::altenv_call_instance(
            altenv,
            this->cameraCharacteristics,
            "getMaxFramesPerSecond",
            "(ILorg/firstinspires/ftc/robotcore/external/android/util/Size;)I",
            libcardinal::altenv_call_static(
                    altenv,
                    altenv->FindClass("org/firstinspires/ftc/robotcore/internal/camera/ImageFormatMapper"),
                    "androidFromVuforiaWebcam",
                    "(Lorg/firstinspires/ftc/robotcore/internal/vuforia/externalprovider/FrameFormat;)I",
                    androidImageFormat
            ),
            libcardinal::to_jvalue(size)
    ).i;

    //Puts in the arguments
    jobject streamingMode = libcardinal::altenv_new_instance(
            altenv,
            "org/firstinspires/ftc/robotcore/internal/vuforia/externalprovider/CameraMode",
            "(IIILorg/firstinspires/ftc/robotcore/internal/vuforia/externalprovider/FrameFormat;)V",
            libcardinal::to_jvalue(this->userxsize),
            libcardinal::to_jvalue(this->userysize),
            maxFramesPerSecond,
            androidImageFormat
    );

    //Creates the capture request to the camera
    jobject cameraCaptureRequest = libcardinal::altenv_call_instance(
            altenv,
            this->camera,
            "createCaptureRequest",
            "(ILorg/firstinspires/ftc/robotcore/external/android/util/Size;I)Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraCaptureRequest;",
            libcardinal::altenv_call_instance(env, streamingMode, "getAndroidFormat", "()I",nullptr),
            libcardinal::altenv_call_instance(env,streamingMode,"getSize","()Lorg/firstinspires/ftc/robotcore/external/android/util/Size;",nullptr),
            libcardinal::altenv_call_instance(env,streamingMode,"getFramesPerSecond","()I",nullptr)
    ).l;

    //Stores the size of the frames so we know how big they are
    jobject fsize = libcardinal::altenv_call_instance(env,streamingMode,"getSize","()Lorg/firstinspires/ftc/robotcore/external/android/util/Size;",nullptr).l;
    this->xsize = libcardinal::altenv_call_instance(env,fsize,"getWidth","()I",nullptr).i;
    this->ysize = libcardinal::altenv_call_instance(env,fsize,"getHeight","()I",nullptr).i;

    //Tells the camera to start processing frames
    //The climactic conclusion of all this madness
    libcardinal::altenv_call_instance(
            altenv,
            cameraCaptureSession,
            "startCapture",
            "(Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraCaptureRequest;Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraCaptureSession$CaptureCallback;Lorg/firstinspires/ftc/robotcore/external/function/Continuation;)Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraCaptureSequenceId;",
            libcardinal::to_jvalue(cameraCaptureRequest),
            libcardinal::altenv_new_instance(
                    altenv,
                    "org/firstinspires/ftc/teamcode/NativeInterfaces$CaptureInterface",
                    "(J)V",
                    libcardinal::to_jvalue((long)this)
            ),
            libcardinal::altenv_call_static(
                    altenv,
                    altenv->FindClass("org/firstinspires/ftc/robotcore/external/function/Continuation"),
                    "create",
                    "(Ljava/util/concurrent/Executor;Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraCapureSession$StatusCallback;)Lorg/firstinspires/ftc/robotcore/external/function/Continuation;",
                    {.l=this->serialThreadPool},
                    libcardinal::to_jvalue(libcardinal::altenv_alloc_instance(
                            altenv,
                            "org/firstinspires/ftc/teamcode/libcardinal$StatusCallback")
                    )
            )
    );
}

void Camera::onNewFrame(JNIEnv *altenv, jobject camera_frame) {
    jbyteArray data = (jbyteArray)libcardinal::altenv_call_instance(
            altenv,
            camera_frame,
            "getImageData",
            "()[B",
            nullptr
    ).l;
    jbyte * rdata = altenv->GetByteArrayElements(data, JNI_FALSE);
    jsize length = altenv->GetArrayLength(data);
    std::vector<unsigned char> frame_data(rdata, rdata + length - 1);
    this->frameHandler(altenv, this->xsize, this->ysize, frame_data);
}

void CameraStream::init(JNIEnv *env) {
    jobject cameraStreamServer = libcardinal::altenv_call_static(
            env,
            env->FindClass("org/firstinspires/ftc/robotcore/external/stream/CameraStreamServer"),
            "getInstance",
            "()Lorg/firstinspires/ftc/robotcore/external/stream/CameraStreamServer;",
            nullptr
    ).l;
    jobject cameraStreamSource = libcardinal::altenv_alloc_instance(
            env,
            "org/firstinspires/ftc/teamcode/NativeInterfaces$FrameSource"
    );
    libcardinal::altenv_call_void_instance(
            env,
            cameraStreamServer,
            "setSource",
            "(Lorg/firstinspires/ftc/robotcore/external/stream/CameraStreamSource;)V",
            libcardinal::to_jvalue(cameraStreamSource)
    );
}

void CameraStream::send(int x_size, int y_size, unsigned char *framedata) {
    this->xsize = x_size;
    this->ysize = y_size;
    this->frame = framedata;
}

CameraStream * CameraStream::get(JNIEnv *env) {
    if (CameraStream::instancePtr == nullptr) {
        CameraStream::init(env);
        CameraStream::instancePtr = new CameraStream();
    }
    return CameraStream::instancePtr;
}

//Helper function to make greyscale image render properly
//This function is really poorly written
std::vector<int> widenToInt(jsize len, const unsigned char * data) {
    std::vector<int> ret(len);
    for (int i = 0; i < len; ++i) {
        ret[i] = (data[len] << 24) + (data[len] << 16) + (data[len] << 8) + data[len];
    }
    return ret;
}

//More JNI wrapper functions!

extern "C" JNIEXPORT void JNICALL Java_org_firstinspires_ftc_teamcode_NativeInterfaces_00024StateInterface_onConfigured(
        JNIEnv *env, jobject thiz, jobject session) {
    Camera * thisptr = (Camera*)libcardinal::altenv_get_field(env, thiz, "thisptr", "J").j;
    thisptr->onConfigured(env, session);
}

extern "C" JNIEXPORT void JNICALL Java_org_firstinspires_ftc_teamcode_NativeInterfaces_00024CaptureInterface_onNewFrame(
        JNIEnv *env, jobject thiz, jobject session,
        jobject request, jobject camera_frame) {
    Camera * thisptr = (Camera*)libcardinal::altenv_get_field(env, thiz, "thisptr", "J").j;
    thisptr->onNewFrame(env, camera_frame);
}

extern "C" JNIEXPORT jintArray JNICALL Java_org_firstinspires_ftc_teamcode_NativeInterfaces_00024FrameSource_getFrame(
        JNIEnv *env, jobject thiz) {
    unsigned char * data = CameraStream::get(env)->frame;
    jsize len = CameraStream::get(env)->xsize * CameraStream::get(env)->ysize;
    std::vector<int> idata = widenToInt(len, data);
    jintArray jints = env->NewIntArray(len);
    env->SetIntArrayRegion(jints, 0, len, idata.data());
    return jints;
}

extern "C" JNIEXPORT jint JNICALL Java_org_firstinspires_ftc_teamcode_NativeInterfaces_00024FrameSource_getXSize(
        JNIEnv *env, jobject thiz) {
    return (jint)(CameraStream::get(env)->xsize);
}

extern "C" JNIEXPORT jint JNICALL Java_org_firstinspires_ftc_teamcode_NativeInterfaces_00024FrameSource_getYSize(
        JNIEnv *env, jobject thiz) {
    return (jint)(CameraStream::get(env)->ysize);
}