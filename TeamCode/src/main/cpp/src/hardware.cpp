#include <hardware.hpp>
#include <libcardinal.hpp>

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
            "com/qualcomm/robotcore/hardware/DcMotorSimple$Direction"
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
            "com/qualcomm/robotcore/hardware/DcMotorSimple$Direction"
    ).l;
    //Hopefully static variables don't move in the memory...
    //*fingers crossed*
    return current_enum_value == reverse;
}

void DcMotor::setZeroPowerBehaviour(DcMotorZeroPowerBehaviour behaviour) {

}

DcMotorZeroPowerBehaviour DcMotor::getZeroPowerBehaviour() {
    return DCMOTOR_UNKNOWN;
}
