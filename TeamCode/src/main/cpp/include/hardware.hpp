#ifndef INC_2024_2025_HARDWARE_HPP
#define INC_2024_2025_HARDWARE_HPP

#include <jni.h>
#include <string>

struct DcMotor {
    DcMotor(JNIEnv *env, jobject opmode, std::string name);
    ~DcMotor();



private:
    JNIEnv *env;
    jobject handle;
};

#endif