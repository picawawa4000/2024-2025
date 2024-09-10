#ifndef CAMERA_HPP
#define CAMERA_HPP

void set_logger(std::function<void(std::string)> logger);

struct Camera {
    Camera(jobject opmode);

private:
    jobject camera;
    jobject cameraManager;
};

#endif