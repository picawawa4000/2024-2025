#include <hardware.hpp>
#include <libcardinal.hpp>
#include <utility>

std::function<void(std::string)> output;

void hardware::set_logger(std::function<void(std::string)> logger) {
    output = std::move(logger);
}

HardwareDevice::HardwareDevice(JNIEnv *env, jobject opmode, const std::string& name,
                               const std::string& classpath) : env(env) {
    this->handle = env->NewGlobalRef(libcardinal::altenv_get_device_from_hardware_map(env, opmode, name, classpath));
}

HardwareDevice::~HardwareDevice() {
    this->env->DeleteGlobalRef(this->handle);
}

void DcMotor::setPower(double power) {
    libcardinal::altenv_call_void_instance(
            this->env,
            this->handle,
            "setPower",
            "(D)V",
            libcardinal::to_jvalue(power)
    );
}

double DcMotor::getPower() {
    return libcardinal::altenv_call_instance(
            this->env,
            this->handle,
            "getPower",
            "()D",
            nullptr
    ).d;
}

void DcMotor::setReverse(bool reverse) {
    //This is the function that is used to get enum values
    jobject enum_value = libcardinal::altenv_get_static_field(
            this->env,
            this->env->FindClass("com/qualcomm/robotcore/hardware/DcMotorSimple$Direction"),
            reverse ? "REVERSE" : "FORWARD",
            "Lcom/qualcomm/robotcore/hardware/DcMotorSimple$Direction;"
    ).l;
    libcardinal::altenv_call_void_instance(
            this->env,
            this->handle,
            "setDirection",
            "(Lcom/qualcomm/robotcore/hardware/DcMotorSimple$Direction;)V",
            libcardinal::to_jvalue(enum_value)
    );
}

bool DcMotor::getReverse() {
    jobject current_enum_value = libcardinal::altenv_call_instance(
            this->env,
            this->handle,
            "getDirection",
            "()Lcom/qualcomm/robotcore/hardware/DcMotorSimple$Direction;",
            nullptr
    ).l;
    //This is the function that is used to get enum values
    jobject reverse = libcardinal::altenv_get_static_field(
            this->env,
            this->env->FindClass("com/qualcomm/robotcore/hardware/DcMotorSimple$Direction"),
            "REVERSE",
            "Lcom/qualcomm/robotcore/hardware/DcMotorSimple$Direction;"
    ).l;
    //Hopefully static variables don't move in the memory...
    //*fingers crossed*
    return current_enum_value == reverse;
}

void DcMotor::setZeroPowerBehaviour(DcMotorZeroPowerBehaviour behaviour) {
    if (behaviour != DCMOTOR_BRAKE and behaviour != DCMOTOR_FLOAT) {
        output("WARNING: Trying to call DcMotor::setZeroPowerBehaviour(UNKNOWN). Problems may occur.");
    }
    //Again, this is how we get enum values
    jobject enum_value = libcardinal::altenv_get_static_field(
            this->env,
            this->env->FindClass("com/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehaviour"),
            behaviour == DCMOTOR_BRAKE ? "BRAKE" : (behaviour == DCMOTOR_FLOAT ? "FLOAT" : "UNKNOWN"),
            "Lcom/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehaviour;"
    ).l;
    //Calls the Java method
    libcardinal::altenv_call_void_instance(
            this->env,
            this->handle,
            "setZeroPowerBehaviour",
            "(Lcom/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehaviour;)V",
            libcardinal::to_jvalue(enum_value)
    );
}

DcMotorZeroPowerBehaviour DcMotor::getZeroPowerBehaviour() {
    //We get the real enum value and compare it against the values in the enum.
    //That is a poorly-written description for a poorly-written function.
    //ADD CACHES
    jobject value = libcardinal::altenv_call_instance(
            this->env,
            this->handle,
            "getZeroPowerBehaviour",
            "()Lcom/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehaviour;",
            nullptr
    ).l;
    jobject enum_brake = libcardinal::altenv_get_static_field(
            this->env,
            this->env->FindClass("com/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehaviour"),
            "BRAKE",
            "Lcom/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehaviour;"
    ).l;
    if (value == enum_brake) return DCMOTOR_BRAKE;
    jobject enum_float = libcardinal::altenv_get_static_field(
            this->env,
            this->env->FindClass("com/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehaviour"),
            "FLOAT",
            "Lcom/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehaviour;"
    ).l;
    if (value == enum_float) return DCMOTOR_FLOAT;
    return DCMOTOR_UNKNOWN;
}

Gamepad::Gamepad(JNIEnv *env, jobject opmode, bool isGamepad2 = false) : env(env) {
    this->handle = env->NewGlobalRef(libcardinal::altenv_get_field(
            env,
            opmode,
            isGamepad2 ? "gamepad2" : "gamepad1",
            "Lcom/qualcomm/robotcore/hardware/Gamepad;"
    ).l);
}

Gamepad::~Gamepad() {
    this->env->DeleteGlobalRef(this->handle);
}

void Gamepad::set_handler(
        std::function<void(bool, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool,
                           float, float, float, float)> ihandler) {
    this->handler = std::move(ihandler);
}

void Gamepad::tick() {
    this->handler(
            this->get_bool_value("a"),
            this->get_bool_value("b"),
            this->get_bool_value("x"),
            this->get_bool_value("y"),
            this->get_bool_value("dpad_down"),
            this->get_bool_value("dpad_up"),
            this->get_bool_value("dpad_left"),
            this->get_bool_value("dpad_right"),
            this->get_bool_value("left_bumper"),
            this->get_bool_value("right_bumper"),
            this->get_bool_value("left_trigger"),
            this->get_bool_value("right_trigger"),
            this->get_float_value("left_stick_x"),
            this->get_float_value("left_stick_y"),
            this->get_float_value("right_stick_x"),
            this->get_float_value("right_stick_y")
    );
}

bool Gamepad::get_bool_value(const std::string &name) {
    return libcardinal::altenv_get_field(
            this->env,
            this->handle,
            name.c_str(),
            "Z"
    ).z == JNI_TRUE;
}

float Gamepad::get_float_value(const std::string &name) {
    return libcardinal::altenv_get_field(
            this->env,
            this->handle,
            name.c_str(),
            "F"
    ).f;
}
