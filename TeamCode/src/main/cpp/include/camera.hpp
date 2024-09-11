#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <jni.h>
#include <functional>

namespace camera {
    //Sets the logger that the camera class will use
    //Currently only used in one case; I may remove it
    void set_logger(std::function<void(std::string)> logger);
}

//Object that represents a physical camera
struct Camera {
    Camera(JNIEnv *env, jobject opmode);

    ~Camera();

    //Starts a recording, passing every frame to handler
    //Keep in mind that the handler runs in a different thread
    //than the main program, and thus has a different JNI env
    //Argument format to handler: env, xsize, ysize, data
    void record(std::function<void(JNIEnv *, int, int, std::vector<unsigned char>)> handler);

    //Stops the current recording
    void stop();

    //Returns true if the camera is currently recording
    //This class cannot create more than one recording per object
    //We should only need one anyways
    bool isRecording() const;

    //DO NOT EVER CALL THIS FROM USER CODE
    //The JNI doesn't really give me an easy way to hide it
    //I programmed it so that if you call it from your code,
    //it will detect that and issue a Halt and Catch Fire
    void onConfigured(JNIEnv *env, jobject cameraCaptureSession);

    //DO NOT EVER CALL THIS FROM USER CODE
    //The JNI doesn't really give me an easy way to hide it
    //I programmed it so that if you call it from your code,
    //it will detect that and issue a Halt and Catch Fire
    void onNewFrame(JNIEnv *env, jobject camera_frame);

private:
    JNIEnv *env;
    jobject camera;
    jobject cameraManager;
    jobject cameraCharacteristics;
    jobject serialThreadPool;
    jobject currentCaptureSession;
    bool recording;
    std::function<void(JNIEnv *, int, int, std::vector<unsigned char>)> frameHandler;
    int xsize, ysize;
};

#endif