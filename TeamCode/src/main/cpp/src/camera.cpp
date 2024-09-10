#include <libcardinal.hpp>
#include <functional>
#include <string>
#include "../include/camera.hpp"


std::function<void(std::string)> output;

void set_logger(std::function<void(std::string)> logger) {
    output = logger;
}

Camera::Camera(jobject opmode) {
    jobject cameraName = libcardinal::get_device_from_hardware_map(opmode, "webcam");
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
    this->cameraManager = cameraManager;
    jobject camera = env->NewGlobalRef(libcardinal::call_instance(
            cameraManager,
            "requestPermissionAndOpenCamera",
            "(Lorg/firstinspires/ftc/robotcore/internal/system/Deadline;Lorg/firstinspires/ftc/robotcore/external/hardware/camera/CameraName;Lorg/firstinspires/ftc/robotcore/external/function/Continuation;)Lorg/firstinspires/ftc/robotcore/external/hardware/camera/Camera;",
            libcardinal::to_jvalue(libcardinal::new_instance(
                    "org/firstinspires/ftc/robotcore/internal/system/Deadline",
                    "(JLjava/util/concurrent/TimeUnit;)V",
                    libcardinal::to_jvalue((jlong)5000),
                    libcardinal::to_jvalue(libcardinal::get_static_field(
                            libcardinal::jnienv->FindClass("java/util/concurrent/TimeUnit"),
                            "MILLISECONDS",
                            "Ljava/util/concurrent/TimeUnit;"
                    ))
            )),
            libcardinal::to_jvalue(cameraName),
            libcardinal::to_jvalue(nullptr)
    ).l);
    this->camera = camera;
}