#ifndef INC_2024_2025_HARDWARE_HPP
#define INC_2024_2025_HARDWARE_HPP

#include <jni.h>
#include <string>

struct DcMotor {
    DcMotor(JNIEnv *env, jobject opmode, std::string name);
    ~DcMotor();

    void setPower(double power);
    double getPower();
    void setReverse(bool reverse);

    //This is one of those functions that we don't need until
    //we do. It's also one of those functions that is...
    //questionably written
    bool getReverse();

private:
    JNIEnv *env;
    jobject handle;
};

#endif