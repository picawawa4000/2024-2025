#include <hardware.hpp>
#include <libcardinal.hpp>

DcMotor::DcMotor(JNIEnv *env, jobject opmode, std::string name) : env(env) {
    this->handle = env->NewGlobalRef(libcardinal::altenv_get_device_from_hardware_map(env, opmode, name, "com/qualcomm/robotcore/hardware/DcMotor"));
}

DcMotor::~DcMotor() {
    this->env->DeleteGlobalRef(this->handle);
}