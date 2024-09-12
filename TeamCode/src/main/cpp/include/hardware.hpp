#ifndef INC_2024_2025_HARDWARE_HPP
#define INC_2024_2025_HARDWARE_HPP

/**
 * The hardware library.
 * **/

#include <jni.h>
#include <string>

namespace hardware {
    void set_logger(std::function<void(std::string)> logger);
}

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
    DcMotor(JNIEnv *env, jobject opmode, const std::string& name) : HardwareDevice(env, opmode, name, "com/qualcomm/robotcore/hardware/DcMotor") {}

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

    //Sets the zero power behaviour of the motor (i.e. whether
    //it should decelerate (brake) or not)
    void setZeroPowerBehaviour(DcMotorZeroPowerBehaviour behaviour);

    //Again, questionably written. See setZeroPowerBehaviour
    DcMotorZeroPowerBehaviour getZeroPowerBehaviour();
};

//I don't know how long this class will stay for...
struct Gamepad {
    Gamepad(JNIEnv *env, jobject opmode, bool isGamepad2);

    ~Gamepad();

    //Argument format (here we go...):
    //bool:  a, b, x, y, dpad_down, dpad_up, dpad_left, dpad_right, bumper_left, bumper_right, trigger_left, trigger_right
    //float: stick_left_x, stick_left_y, stick_right_x, stick_right_y
    void set_handler(std::function<void(bool,bool,bool,bool,bool,bool,bool,bool,bool,bool,bool,bool,float,float,float,float)> handler);

    //Calls handler()
    //Should be called in a loop
    //Even if it's in another thread, handler can simply capture the env, so we won't pass one
    void tick();

private:
    JNIEnv * env;
    jobject handle;
    //so many arguments!!
    std::function<void(bool, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool, float, float, float, float)> handler;

    bool get_bool_value(const std::string& name);
    float get_float_value(const std::string& name);
};

#endif