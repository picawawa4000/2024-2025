#ifndef CAMERA_HPP
#define CAMERA_HPP

/**
 * The camera library.
 * From the header file, it looks simple, but it's a mess,
 * which is why it's not part of the hardware library.
 * **/

#include <jni.h>
#include <functional>

namespace camera {
    //Sets the logger that the camera class will use
    //Currently only used in one case; I may remove it
    void set_logger(std::function<void(std::string)> logger);
}

//Object that represents a physical camera
struct Camera {
    //Constructs the camera.
    //Notice that the actual size may not be equal to the
    //desired size due to technical restrictions.
    Camera(JNIEnv *env, jobject opmode, int xsize, int ysize);

    ~Camera();

    //Starts a recording, passing every frame to handler
    //Keep in mind that the handler runs in a different thread
    //than the main program, and thus has a different JNIEnv *
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
    jobject currentCaptureSession{};
    bool recording;
    std::function<void(JNIEnv *, int, int, std::vector<unsigned char>)> frameHandler;
    int xsize{}, ysize{};
    int userxsize, userysize;
};

//Object that sends camera frames to the Driver Station
//Let's make it a singleton!
struct CameraStream {
    CameraStream(const CameraStream&) = delete;

    //Gets the CameraStream
    static CameraStream * get(JNIEnv * env);

    //Sends a frame to the Driver Station
    //What this really does is updates the frame that the Driver Station gets
    //when it asks for a frame.
    void send(int x_size, int y_size, unsigned char * framedata);

    //These three values can be modified directly.
    //It's poor form, but it can be done.
    int xsize;

    //These three values can be modified directly.
    //It's poor form, but it can be done.
    int ysize;

    //These three values can be modified directly.
    //It's poor form, but it can be done.
    unsigned char *frame;

private:
    //This is the worst singleton I've ever made.
    static CameraStream * instancePtr;

    CameraStream() : xsize(0), ysize(0), frame(nullptr) {}

    //Initialises the CameraStream
    static void init(JNIEnv * env);
};

#endif