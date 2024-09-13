//Don't forget to configure the file settings in CMakeLists.txt when adding a library!

#include <jni.h>
#include <libcardinal.hpp>
#include <camera.hpp>
#include <hardware.hpp>
#include <android/log.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#define LOGGER_NAME "milk"
#define print(...) __android_log_print(ANDROID_LOG_INFO, LOGGER_NAME, __VA_ARGS__)

//I don't know if I'll wind up keeping this or not...
using namespace libcardinal;

//The logging function that we use when one of my libraries
//wants a logging function
auto log_lambda = [](const std::string& out){
    print("%s", out.c_str());
};

auto camera_handler = [](JNIEnv * env, int xsize, int ysize, std::vector<unsigned char> frameraw) {
    cv::Mat frame(xsize, ysize, CV_8UC2, &frameraw[0]);
    cv::cvtColor(frame, frame, cv::COLOR_YUV2GRAY_YUYV);
    CameraStream::get(env)->send(xsize, ysize, frame.data);
};

//Entry point for the program.
void run(JNIEnv * env, jobject thiz) {
    //Tell the libraries how to log
    camera::set_logger(log_lambda);
    hardware::set_logger(log_lambda);

    Camera camera(env, thiz, 640, 480);
    camera.record(camera_handler);
}

//Everything below here is a JNI wrapper function. Don't touch unless adding or removing one.

extern "C" JNIEXPORT void JNICALL Java_org_firstinspires_ftc_teamcode_RemoteOperation_run(
        JNIEnv *env, jobject thiz) {
    run(env, thiz);
}