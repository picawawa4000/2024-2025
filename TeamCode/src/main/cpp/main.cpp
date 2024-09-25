//Don't forget to configure the file settings in CMakeLists.txt when adding a library!

#include <jni.h>
#include <android/log.h>

#include <libcardinal.hpp>
#include <camera.hpp>
#include <hardware.hpp>
#include <cvlib.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

//TODO: UPDATE THE SDK

#define LOGGER_NAME "milk"
#define print(...) __android_log_print(ANDROID_LOG_INFO, LOGGER_NAME, __VA_ARGS__)

//The logging function that we use when one of my libraries
//wants a logging function
auto log_lambda = [](const std::string& out){
    print("%s", out.c_str());
};

auto camera_handler = [](JNIEnv * env, int xsize, int ysize, std::vector<unsigned char> frameraw) {
    print("Got frame from camera, processing...");
    cv::Mat frame(xsize, ysize, CV_8UC2, &frameraw[0]);
    cv::cvtColor(frame, frame, cv::COLOR_YUV2GRAY_YUYV);
    CameraStream::get(env)->send(xsize, ysize, frame.data);
};

//Entry point for the program.
void run(JNIEnv * env, jobject thiz) {
    //This should hopefully not be necessary...
    libcardinal::setenv(env);

    //Tell the libraries how to log
    camera::set_logger(log_lambda);
    hardware::set_logger(log_lambda);

    libcardinal::altenv_call_void_instance(
            env,
            thiz,
            "waitForStart",
            "()V",
            nullptr
    );

    print("About to create camera...");
    Camera camera(env, thiz, 640, 480);
    print("Camera successfully created...");
    camera.record(camera_handler);
}

void printMat(cv::Mat mat) {
    std::string out = "mat " + std::to_string(mat.rows) + "x" + std::to_string(mat.cols) + " [";
    for (int i = 0; i < mat.cols; ++i) {
        out += "[";
        for (int j = 0; j < mat.rows; ++j) {
            out += std::to_string(mat.at<int>(j, i)) + ", ";
        }
        out += "],";
    }
    out += "]";
    print("%s", out.c_str());
}

//Everything below here is a JNI wrapper function. Don't touch unless adding or removing one.

extern "C" JNIEXPORT void JNICALL Java_org_firstinspires_ftc_teamcode_RemoteOperation_run(
        JNIEnv *env, jobject thiz) {
    run(env, thiz);
}