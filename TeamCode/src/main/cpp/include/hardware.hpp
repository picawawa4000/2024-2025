#ifndef INC_2024_2025_HARDWARE_HPP
#define INC_2024_2025_HARDWARE_HPP

#include <jni.h>
#include <string>

struct HardwareDevice {
    HardwareDevice(JNIEnv *env, jobject opmode, const std::string& name, const std::string& classpath);
    ~HardwareDevice();

protected:
    JNIEnv *env;
    jobject handle;
};

enum DcMotorZeroPowerBehaviour {
    DCMOTOR_BRAKE,
    DCMOTOR_FLOAT,
    DCMOTOR_UNKNOWN
};

//No encoders; I don't think we'll need them
//I may add a DcMotorEncoder class later though
struct DcMotor : public HardwareDevice {
    DcMotor(JNIEnv *env, jobject opmode, const std::string& name) : HardwareDevice(env, opmode, name, "com/qualcomm/robotcore/hardware/DcMotor") {};
    ~DcMotor() {};

    //Power is expressed as a percentage of the motor's max
    void setPower(double power);

    //Power is expressed as a percentage of the motor's max
    double getPower();

    //True = reverse, false = forwards
    void setReverse(bool reverse);

    //This is one of those functions that we don't need until
    //we do. It's also one of those functions that is...
    //questionably written
    bool getReverse();

    void setZeroPowerBehaviour(DcMotorZeroPowerBehaviour behaviour);

    DcMotorZeroPowerBehaviour getZeroPowerBehaviour();
};

#endif