#include <libcardinal.hpp>

#include <jni.h>

#include <string>
#include <map>
#include <future>
#include <thread>
#include <chrono>
#include <exception>
#include <locale>
#include <codecvt>
#include <vector>

namespace libcardinal {
    void setenv(JNIEnv *env) {
        libcardinal::jnienv = env;
    }

    JNIEnv * getenv() {
        return libcardinal::jnienv;
    }

    void setvm(JavaVM *vm) {
        libcardinal::javavm = vm;
    }

    JavaVM * getvm() {
        return libcardinal::javavm;
    }

/*void initenv() {
    JavaVMInitArgs args;
    args.version = JNI_VERSION_1_2;
    args.nOptions = 0;
    args.ignoreUnrecognized = 1;
    JNI_CreateJavaVM(&libcardinal_javavm, (void **)&libcardinal::jnienv, (void *)(&args));
}*/

    void terminate() {
        libcardinal::javavm->DestroyJavaVM();
    }

//Java interface functions for generic usage

    jvalue to_jvalue(jobject l) {return {.l=l};}
    jvalue to_jvalue(jint i) {return {.i=i};}
    jvalue to_jvalue(jbyte b) {return {.b=b};}
    jvalue to_jvalue(jchar c) {return {.c=c};}
    jvalue to_jvalue(jdouble d) {return {.d=d};}
    jvalue to_jvalue(jfloat f) {return {.f=f};}
    jvalue to_jvalue(jlong j) {return {.j=j};}
    jvalue to_jvalue(jshort s) {return {.s=s};}
    jvalue to_jvalue(jboolean z) {return {.z=z};}

    jobject
    new_instance(const char *id, const char *constructor_sig, const jvalue *constructor_args) {
        //libcardinal::exception_check();
        jclass target = libcardinal::jnienv->FindClass(id);
        jmethodID constructor = libcardinal::jnienv->GetMethodID(target, "<init>", constructor_sig);
        return libcardinal::jnienv->NewObjectA(target, constructor, constructor_args);
    }

    jobject
    new_instance(const char *id, const char *constructor_sig, jvalue constructor_args...) {
        std::vector<jvalue> argpack{constructor_args};
        return new_instance(id, constructor_sig, &argpack[0]);
    }

    jobject altenv_new_instance(JNIEnv *env, const char *id, const char *constructor_sig,
                                const jvalue *constructor_args) {
        jclass target = env->FindClass(id);
        jmethodID constructor = env->GetMethodID(target, "<init>", constructor_sig);
        return env->NewObjectA(target, constructor, constructor_args);
    }

    jobject
    altenv_new_instance(JNIEnv *env, const char *id, const char *constructor_sig, jvalue constructor_args...) {
        std::vector<jvalue> argpack{constructor_args};
        return altenv_new_instance(env, id, constructor_sig, &argpack[0]);
    }

    jobject alloc_instance(const char *id) {
        //libcardinal::exception_check();
        jclass target = libcardinal::jnienv->FindClass(id);
        return libcardinal::jnienv->AllocObject(target);
    }

    jobject altenv_alloc_instance(JNIEnv *env, const char *id) {
        jclass target = env->FindClass(id);
        return env->AllocObject(target);
    }

    jvalue get_field(jobject target, const char *id, const char *object_sig) {
        //libcardinal::exception_check();
        jfieldID field = libcardinal::jnienv->GetFieldID(libcardinal::jnienv->GetObjectClass(target),
                                                         id,
                                                         object_sig);
        jvalue output;
        std::string sig = std::string(object_sig);
        if (sig == "Z") {
            output = {.z=libcardinal::jnienv->GetBooleanField(target, field)};
        } else if (sig == "B") {
            output = {.b=libcardinal::jnienv->GetByteField(target, field)};
        } else if (sig == "C") {
            output = {.c=libcardinal::jnienv->GetCharField(target, field)};
        } else if (sig == "S") {
            output = {.s=libcardinal::jnienv->GetShortField(target, field)};
        } else if (sig == "I") {
            output = {.i=libcardinal::jnienv->GetIntField(target, field)};
        } else if (sig == "L") {
            output = {.j=libcardinal::jnienv->GetLongField(target, field)};
        } else if (sig == "F") {
            output = {.f=libcardinal::jnienv->GetFloatField(target, field)};
        } else if (sig == "D") {
            output = {.d=libcardinal::jnienv->GetDoubleField(target, field)};
        } else {
            output = {.l=libcardinal::jnienv->GetObjectField(target, field)};
        }
        return output;
    }

    jvalue altenv_get_field(JNIEnv *env, jobject target, const char *id, const char *object_sig) {
        jclass clazz = env->GetObjectClass(target);
        jfieldID field = env->GetFieldID(clazz, id,object_sig);
        env->DeleteLocalRef(clazz);
        jvalue output;
        std::string sig = std::string(object_sig);
        if (sig == "Z") {
            output = {.z=env->GetBooleanField(target, field)};
        } else if (sig == "B") {
            output = {.b=env->GetByteField(target, field)};
        } else if (sig == "C") {
            output = {.c=env->GetCharField(target, field)};
        } else if (sig == "S") {
            output = {.s=env->GetShortField(target, field)};
        } else if (sig == "I") {
            output = {.i=env->GetIntField(target, field)};
        } else if (sig == "L") {
            output = {.j=env->GetLongField(target, field)};
        } else if (sig == "F") {
            output = {.f=env->GetFloatField(target, field)};
        } else if (sig == "D") {
            output = {.d=env->GetDoubleField(target, field)};
        } else {
            output = {.l=env->GetObjectField(target, field)};
        }
        return output;
    }

    jvalue get_static_field(jclass target, const char *id, const char *object_sig) {
        //libcardinal::exception_check();
        jfieldID field = libcardinal::jnienv->GetStaticFieldID(target,id,object_sig);
        jvalue output;
        std::string sig = std::string(object_sig);
        if (sig == "Z") {
            output = {.z=libcardinal::jnienv->GetStaticBooleanField(target, field)};
        } else if (sig == "B") {
            output = {.b=libcardinal::jnienv->GetStaticByteField(target, field)};
        } else if (sig == "C") {
            output = {.c=libcardinal::jnienv->GetStaticCharField(target, field)};
        } else if (sig == "S") {
            output = {.s=libcardinal::jnienv->GetStaticShortField(target, field)};
        } else if (sig == "I") {
            output = {.i=libcardinal::jnienv->GetStaticIntField(target, field)};
        } else if (sig == "L") {
            output = {.j=libcardinal::jnienv->GetStaticLongField(target, field)};
        } else if (sig == "F") {
            output = {.f=libcardinal::jnienv->GetStaticFloatField(target, field)};
        } else if (sig == "D") {
            output = {.d=libcardinal::jnienv->GetStaticDoubleField(target, field)};
        } else {
            output = {.l=libcardinal::jnienv->GetStaticObjectField(target, field)};
        }
        return output;
    }

    jvalue altenv_get_static_field(JNIEnv * env, jclass target, const char *id, const char *object_sig) {
        jfieldID field = env->GetStaticFieldID(target,id,object_sig);
        jvalue output;
        std::string sig = std::string(object_sig);
        if (sig == "Z") {
            output = {.z=env->GetStaticBooleanField(target, field)};
        } else if (sig == "B") {
            output = {.b=env->GetStaticByteField(target, field)};
        } else if (sig == "C") {
            output = {.c=env->GetStaticCharField(target, field)};
        } else if (sig == "S") {
            output = {.s=env->GetStaticShortField(target, field)};
        } else if (sig == "I") {
            output = {.i=env->GetStaticIntField(target, field)};
        } else if (sig == "L") {
            output = {.j=env->GetStaticLongField(target, field)};
        } else if (sig == "F") {
            output = {.f=env->GetStaticFloatField(target, field)};
        } else if (sig == "D") {
            output = {.d=env->GetStaticDoubleField(target, field)};
        } else {
            output = {.l=env->GetStaticObjectField(target, field)};
        }
        return output;
    }

    template<typename ArrayOf>
    jarray get_array_field(jobject target, const char *id, const char *object_sig) {
        //libcardinal::exception_check();
        jobject array_holder = libcardinal::get_field(target, id, object_sig).l;
        return static_cast<ArrayOf>(array_holder);
    }

    template<typename ArrayOf>
    jarray
    altenv_get_array_field(JNIEnv *env, jobject target, const char *id, const char *object_sig) {
        jobject array_holder = libcardinal::altenv_get_field(env, target, id, object_sig).l;
        return static_cast<ArrayOf>(array_holder);
    }

    jvalue call_instance(jobject target, const char *name, const char *sig, jvalue * args) {
        //libcardinal::exception_check();
        jclass clasz = libcardinal::jnienv->GetObjectClass(target);
        jmethodID id = libcardinal::jnienv->GetMethodID(clasz,
                                                        name, sig);
        libcardinal::jnienv->DeleteLocalRef(clasz);
        char sig_last_char = sig[strlen(sig) - 1];
        char sig_second_last_char = sig[strlen(sig) - 2];
        if (sig_second_last_char == '[') {
            return {.l=libcardinal::jnienv->CallObjectMethodA(target, id, args)};
        }
        if (sig_last_char == 'Z') {
            return {.z=libcardinal::jnienv->CallBooleanMethodA(target, id, args)};
        } else if (sig_last_char == 'B') {
            return {.b=libcardinal::jnienv->CallByteMethodA(target, id, args)};
        } else if (sig_last_char == 'S') {
            return {.s=libcardinal::jnienv->CallShortMethodA(target, id, args)};
        } else if (sig_last_char == 'I') {
            return {.i=libcardinal::jnienv->CallIntMethodA(target, id, args)};
        } else if (sig_last_char == 'J') {
            return {.j=libcardinal::jnienv->CallLongMethodA(target, id, args)};
        } else if (sig_last_char == 'C') {
            return {.c=libcardinal::jnienv->CallCharMethodA(target, id, args)};
        } else if (sig_last_char == 'F') {
            return {.f=libcardinal::jnienv->CallFloatMethodA(target, id, args)};
        } else if (sig_last_char == 'D') {
            return {.d=libcardinal::jnienv->CallDoubleMethodA(target, id, args)};
        }
        return {.l=libcardinal::jnienv->NewGlobalRef(
                libcardinal::jnienv->CallObjectMethodA(target, id, args))};
    }

    jvalue call_instance(jobject target, const char *name, const char *sig, jvalue args...) {
        std::vector<jvalue> argpack{args};
        return call_instance(target, name, sig, &argpack[0]);
    }

    jvalue altenv_call_instance(JNIEnv *env, jobject target, const char *name, const char *sig,
                                jvalue *args) {
        jclass clasz = env->GetObjectClass(target);
        jmethodID id = env->GetMethodID(clasz, name, sig);
        env->DeleteLocalRef(clasz);
        char sig_last_char = sig[strlen(sig) - 1];
        char sig_second_last_char = sig[strlen(sig) - 2];
        if (sig_second_last_char == '[') {
            return {.l=env->CallObjectMethodA(target, id, args)};
        }
        if (sig_last_char == 'Z') {
            return {.z=env->CallBooleanMethodA(target, id, args)};
        } else if (sig_last_char == 'B') {
            return {.b=env->CallByteMethodA(target, id, args)};
        } else if (sig_last_char == 'S') {
            return {.s=env->CallShortMethodA(target, id, args)};
        } else if (sig_last_char == 'I') {
            return {.i=env->CallIntMethodA(target, id, args)};
        } else if (sig_last_char == 'J') {
            return {.j=env->CallLongMethodA(target, id, args)};
        } else if (sig_last_char == 'C') {
            return {.c=env->CallCharMethodA(target, id, args)};
        } else if (sig_last_char == 'F') {
            return {.f=env->CallFloatMethodA(target, id, args)};
        } else if (sig_last_char == 'D') {
            return {.d=env->CallDoubleMethodA(target, id, args)};
        }
        jobject value = env->CallObjectMethodA(target, id, args);
        jvalue out = {.l=env->NewGlobalRef(value)};
        env->DeleteLocalRef(value);
        return out;
    }

    jvalue altenv_call_instance(JNIEnv *env, jobject target, const char *name, const char *sig, jvalue args...) {
        std::vector<jvalue> argpack{args};
        return altenv_call_instance(env, target, name, sig, &argpack[0]);
    }

    void call_void_instance(jobject target, const char *name, const char *sig, jvalue * args) {
        //libcardinal::exception_check();
        jclass clasz = libcardinal::jnienv->GetObjectClass(target);
        jmethodID id = libcardinal::jnienv->GetMethodID(clasz,
                                                        name, sig);
        libcardinal::jnienv->DeleteLocalRef(clasz);
        libcardinal::jnienv->CallVoidMethodA(target, id, args);
    }

    void call_void_instance(jobject target, const char *name, const char *sig, jvalue args...) {
        std::vector<jvalue> argpack{args};
        call_void_instance(target, name, sig, &argpack[0]);
    }

    void altenv_call_void_instance(JNIEnv *env, jobject target, const char *name, const char *sig,
                                   jvalue *args) {
        jclass clasz = env->GetObjectClass(target);
        jmethodID id = env->GetMethodID(clasz, name, sig);
        env->DeleteLocalRef(clasz);
        env->CallVoidMethodA(target, id, args);
    }

    void altenv_call_void_instance(JNIEnv *env, jobject target, const char *name, const char *sig, jvalue args...) {
        std::vector<jvalue> argpack{args};
        altenv_call_void_instance(env, target, name, sig, &argpack[0]);
    }

    jobject call_nonvirtual(jobject target, const char *name, const char *sig, jvalue *args) {
        //libcardinal::exception_check();
        jmethodID id = libcardinal::jnienv->GetMethodID(libcardinal::jnienv->GetObjectClass(target),
                                                        name, sig);
        return libcardinal::jnienv->CallNonvirtualObjectMethodA(target,
                                                                libcardinal::jnienv->GetObjectClass(
                                                                        target), id, args);
    }

    jobject call_nonvirtual(jobject target, const char *name, const char *sig, jvalue args...) {
        std::vector<jvalue> argpack{args};
        return call_nonvirtual(target, name, sig, &argpack[0]);
    }

    jobject altenv_call_nonvirtual(JNIEnv *env, jobject target, const char *name, const char *sig,
                                   jvalue *args) {
        jmethodID id = env->GetMethodID(env->GetObjectClass(target), name, sig);
        return env->CallNonvirtualObjectMethodA(target, env->GetObjectClass(target), id, args);
    }

    jobject altenv_call_nonvirtual(JNIEnv *env, jobject target, const char *name, const char *sig, jvalue args...) {
        std::vector<jvalue> argpack{args};
        return altenv_call_nonvirtual(env, target, name, sig, &argpack[0]);
    }

    jvalue call_static(jobject target, const char *name, const char *sig, jvalue *args) {
        //libcardinal::exception_check();
        jmethodID id = libcardinal::jnienv->GetStaticMethodID(
                libcardinal::jnienv->GetObjectClass(target), name, sig);
        char sig_last_char = sig[strlen(sig) - 1];
        char sig_second_last_char = sig[strlen(sig) - 2];
        if (sig_second_last_char == '[') {
            return {.l=libcardinal::jnienv->CallObjectMethodA(libcardinal::jnienv->GetObjectClass(target), id, args)};
        }
        if (sig_last_char == 'Z') {
            return {.z=libcardinal::jnienv->CallStaticBooleanMethodA(libcardinal::jnienv->GetObjectClass(target), id, args)};
        } else if (sig_last_char == 'B') {
            return {.b=libcardinal::jnienv->CallStaticByteMethodA(libcardinal::jnienv->GetObjectClass(target), id, args)};
        } else if (sig_last_char == 'S') {
            return {.s=libcardinal::jnienv->CallStaticShortMethodA(libcardinal::jnienv->GetObjectClass(target), id, args)};
        } else if (sig_last_char == 'I') {
            return {.i=libcardinal::jnienv->CallStaticIntMethodA(libcardinal::jnienv->GetObjectClass(target), id, args)};
        } else if (sig_last_char == 'J') {
            return {.j=libcardinal::jnienv->CallStaticLongMethodA(libcardinal::jnienv->GetObjectClass(target), id, args)};
        } else if (sig_last_char == 'C') {
            return {.c=libcardinal::jnienv->CallStaticCharMethodA(libcardinal::jnienv->GetObjectClass(target), id, args)};
        } else if (sig_last_char == 'F') {
            return {.f=libcardinal::jnienv->CallStaticFloatMethodA(libcardinal::jnienv->GetObjectClass(target), id, args)};
        } else if (sig_last_char == 'D') {
            return {.d=libcardinal::jnienv->CallStaticDoubleMethodA(libcardinal::jnienv->GetObjectClass(target), id, args)};
        }
        return {.l=libcardinal::jnienv->CallStaticObjectMethodA(
                libcardinal::jnienv->GetObjectClass(target), id, args)};
    }

    jvalue call_static(jobject target, const char *name, const char *sig, jvalue args...) {
        std::vector<jvalue> argpack{args};
        return call_static(target, name, sig, &argpack[0]);
    }

    jvalue altenv_call_static(JNIEnv *env, jobject target, const char *name, const char *sig,
                              jvalue *args) {
        //libcardinal::exception_check();
        jmethodID id = env->GetStaticMethodID(
                env->GetObjectClass(target), name, sig);
        char sig_last_char = sig[strlen(sig) - 1];
        char sig_second_last_char = sig[strlen(sig) - 2];
        if (sig_second_last_char == '[') {
            return {.l=env->CallObjectMethodA(env->GetObjectClass(target), id, args)};
        }
        if (sig_last_char == 'Z') {
            return {.z=env->CallStaticBooleanMethodA(env->GetObjectClass(target), id, args)};
        } else if (sig_last_char == 'B') {
            return {.b=env->CallStaticByteMethodA(env->GetObjectClass(target), id, args)};
        } else if (sig_last_char == 'S') {
            return {.s=env->CallStaticShortMethodA(env->GetObjectClass(target), id, args)};
        } else if (sig_last_char == 'I') {
            return {.i=env->CallStaticIntMethodA(env->GetObjectClass(target), id, args)};
        } else if (sig_last_char == 'J') {
            return {.j=env->CallStaticLongMethodA(env->GetObjectClass(target), id, args)};
        } else if (sig_last_char == 'C') {
            return {.c=env->CallStaticCharMethodA(env->GetObjectClass(target), id, args)};
        } else if (sig_last_char == 'F') {
            return {.f=env->CallStaticFloatMethodA(env->GetObjectClass(target), id, args)};
        } else if (sig_last_char == 'D') {
            return {.d=env->CallStaticDoubleMethodA(env->GetObjectClass(target), id, args)};
        }
        return {.l=env->CallStaticObjectMethodA(
                env->GetObjectClass(target), id, args)};
    }

    jvalue altenv_call_static(JNIEnv *env, jobject target, const char *name, const char *sig, jvalue args...) {
        std::vector<jvalue> argpack{args};
        return altenv_call_static(env, target, name, sig, &argpack[0]);
    }

    jvalue call_static(jclass target, const char *name, const char *sig, jvalue *args) {
        //libcardinal::exception_check();
        jmethodID id = libcardinal::jnienv->GetStaticMethodID(target, name, sig);
        char sig_last_char = sig[strlen(sig) - 1];
        char sig_second_last_char = sig[strlen(sig) - 2];
        if (sig_second_last_char == '[') {
            return {.l=libcardinal::jnienv->CallObjectMethodA(target, id, args)};
        }
        if (sig_last_char == 'Z') {
            return {.z=libcardinal::jnienv->CallStaticBooleanMethodA(target, id, args)};
        } else if (sig_last_char == 'B') {
            return {.b=libcardinal::jnienv->CallStaticByteMethodA(target, id, args)};
        } else if (sig_last_char == 'S') {
            return {.s=libcardinal::jnienv->CallStaticShortMethodA(target, id, args)};
        } else if (sig_last_char == 'I') {
            return {.i=libcardinal::jnienv->CallStaticIntMethodA(target, id, args)};
        } else if (sig_last_char == 'J') {
            return {.j=libcardinal::jnienv->CallStaticLongMethodA(target, id, args)};
        } else if (sig_last_char == 'C') {
            return {.c=libcardinal::jnienv->CallStaticCharMethodA(target, id, args)};
        } else if (sig_last_char == 'F') {
            return {.f=libcardinal::jnienv->CallStaticFloatMethodA(target, id, args)};
        } else if (sig_last_char == 'D') {
            return {.d=libcardinal::jnienv->CallStaticDoubleMethodA(target, id, args)};
        }
        return {.l=libcardinal::jnienv->CallStaticObjectMethodA(target, id, args)};
    }

    jvalue call_static(jclass target, const char *name, const char *sig, jvalue args...) {
        std::vector<jvalue> argpack{args};
        return call_static(target, name, sig, &argpack[0]);
    }

    jvalue altenv_call_static(JNIEnv *env, jclass target, const char *name, const char *sig,
                              jvalue *args) {
        jmethodID id = env->GetStaticMethodID(target, name, sig);
        char sig_last_char = sig[strlen(sig) - 1];
        char sig_second_last_char = sig[strlen(sig) - 2];
        if (sig_second_last_char == '[') {
            return {.l=env->CallStaticObjectMethodA(target, id, args)};
        }
        if (sig_last_char == 'Z') {
            return {.z=env->CallStaticBooleanMethodA(target, id, args)};
        } else if (sig_last_char == 'B') {
            return {.b=env->CallStaticByteMethodA(target, id, args)};
        } else if (sig_last_char == 'S') {
            return {.s=env->CallStaticShortMethodA(target, id, args)};
        } else if (sig_last_char == 'I') {
            return {.i=env->CallStaticIntMethodA(target, id, args)};
        } else if (sig_last_char == 'J') {
            return {.j=env->CallStaticLongMethodA(target, id, args)};
        } else if (sig_last_char == 'C') {
            return {.c=env->CallStaticCharMethodA(target, id, args)};
        } else if (sig_last_char == 'F') {
            return {.f=env->CallStaticFloatMethodA(target, id, args)};
        } else if (sig_last_char == 'D') {
            return {.d=env->CallStaticDoubleMethodA(target, id, args)};
        }
        return {.l=env->NewGlobalRef(env->CallStaticObjectMethodA(target, id, args))};
    }

    jvalue altenv_call_static(JNIEnv *env, jclass target, const char *name, const char *sig, jvalue args...) {
        std::vector<jvalue> argpack{args};
        return altenv_call_static(env, target, name, sig, &argpack[0]);
    }

    std::string get_enum_name(jobject target) {
        //libcardinal::exception_check();
        jobject str = libcardinal::call_instance(target, "name", "()Ljava/lang/String;", nullptr).l;
        auto byteArray = (jbyteArray) (libcardinal::call_instance(str, "getBytes", "()[B",
                                                                  nullptr).l);
        size_t length = libcardinal::jnienv->GetArrayLength(byteArray);
        jbyte *pBytes = libcardinal::jnienv->GetByteArrayElements(byteArray, nullptr);
        std::string ret = std::string((char *) pBytes, length);
        libcardinal::jnienv->ReleaseByteArrayElements(byteArray, pBytes, JNI_ABORT);
        libcardinal::jnienv->DeleteLocalRef(byteArray);
        return ret;
    }

    std::string altenv_get_enum_name(JNIEnv *env, jobject target) {
        jobject str = libcardinal::altenv_call_instance(env, target, "name", "()Ljava/lang/String;",
                                                        nullptr).l;
        auto byteArray = (jbyteArray) (libcardinal::altenv_call_instance(env, str, "getBytes",
                                                                         "()[B",
                                                                         nullptr).l);
        size_t length = env->GetArrayLength(byteArray);
        jbyte *pBytes = env->GetByteArrayElements(byteArray, nullptr);
        std::string ret = std::string((char *) pBytes, length);
        env->ReleaseByteArrayElements(byteArray, pBytes, JNI_ABORT);
        env->DeleteLocalRef(byteArray);
        return ret;
    }

//FTC-specific functions

    jobject get_telemetry(jobject opMode) {
        return libcardinal::jnienv->NewGlobalRef(libcardinal::get_field(opMode, "telemetry",
                                                                        "Lorg/firstinspires/ftc/robotcore/external/Telemetry;").l);
    }

    jobject altenv_get_telemetry(JNIEnv *env, jobject opMode) {
        return env->NewGlobalRef(libcardinal::altenv_get_field(env, opMode, "telemetry",
                                                               "Lorg/firstinspires/ftc/robotcore/external/Telemetry;").l);
    }

    jobject get_hardware_map(jobject opMode) {
        return libcardinal::jnienv->NewGlobalRef(libcardinal::get_field(opMode, "hardwareMap", "Lcom/qualcomm/robotcore/hardware/HardwareMap;").l);
    }

    jobject altenv_get_hardware_map(JNIEnv *env, jobject opMode) {
        return env->NewGlobalRef(libcardinal::altenv_get_field(env, opMode, "hardwareMap",
                                                               "Lcom/qualcomm/robotcore/hardware/HardwareMap;").l);
    }

//Reports telemetry for the specified opmode
//Only accepts strings for the value
    void report_telemetry_for(jobject opMode, const std::string &key, const std::string &value) {
        jobject telemetry = libcardinal::get_telemetry(opMode);
        jvalue args[2] = {{.l=libcardinal::jnienv->NewStringUTF(key.c_str())},
                          {.l=libcardinal::jnienv->NewStringUTF(value.c_str())}};
        libcardinal::call_instance(telemetry, "addData",
                                   "(Ljava/lang/String;Ljava/lang/Object;)Lorg/firstinspires/ftc/robotcore/external/Telemetry$Item;",
                                   args);
        libcardinal::call_instance(telemetry, "update", "()Z", nullptr);
        libcardinal::jnienv->DeleteGlobalRef(telemetry);
    }

    void altenv_report_telemetry_for(JNIEnv *env, jobject opMode, const std::string &key,
                                     const std::string &value) {
        jobject telemetry = libcardinal::altenv_get_telemetry(env, opMode);
        jvalue args[2] = {{.l=env->NewStringUTF(key.c_str())},
                          {.l=env->NewStringUTF(value.c_str())}};
        libcardinal::altenv_call_instance(env, telemetry, "addData",
                                          "(Ljava/lang/String;Ljava/lang/Object;)Lorg/firstinspires/ftc/robotcore/external/Telemetry$Item;",
                                          args);
        libcardinal::altenv_call_instance(env, telemetry, "update", "()Z", nullptr);
        env->DeleteGlobalRef(telemetry);
    }

    void report_telemetry_for(jobject opMode, const std::string& key, jvalue value) {
        jobject telemetry = libcardinal::get_telemetry(opMode);
        jvalue args[2] = {{.l=libcardinal::jnienv->NewStringUTF(key.c_str())},
                          value};
        libcardinal::call_instance(telemetry, "addData", "(Ljava/lang/String;Ljava/lang/Object;)Lorg/firstinspires/ftc/robotcore/external/Telemetry$Item;", args);
        libcardinal::call_instance(telemetry, "update", "()Z", nullptr);
        libcardinal::jnienv->DeleteGlobalRef(telemetry);
    }

    void altenv_report_telemetry_for(JNIEnv * env, jobject opMode, const std::string& key, jvalue value) {
        jobject telemetry = libcardinal::altenv_get_telemetry(env, opMode);
        jvalue args[2] = {{.l=env->NewStringUTF(key.c_str())},
                          value};
        libcardinal::altenv_call_instance(env, telemetry, "addData", "(Ljava/lang/String;Ljava/lang/Object;)Lorg/firstinspires/ftc/robotcore/external/Telemetry$Item;", args);
        libcardinal::altenv_call_instance(env, telemetry, "update", "()Z", nullptr);
        libcardinal::jnienv->DeleteGlobalRef(telemetry);
    }

    void log(jobject logger, const std::string& message) {
        jvalue message2 = {.l=libcardinal::jnienv->NewStringUTF(message.c_str())};
        libcardinal::call_void_instance(logger, "info", "(Ljava/lang/String;)V", &message2);
        libcardinal::jnienv->DeleteLocalRef(message2.l);
    }

    void altenv_log(JNIEnv * env, jobject logger, const std::string& message) {
        jvalue message2 = {.l=env->NewStringUTF(message.c_str())};
        libcardinal::altenv_call_void_instance(env, logger, "info", "(Ljava/lang/String;)V", &message2);
        env->DeleteLocalRef(message2.l);
    }

    jobject get_device_from_hardware_map(jobject opMode, const std::string &id,
                                         const std::string &idClass) {
        jobject hardwareMap = libcardinal::get_hardware_map(opMode);
        jvalue args[2] = {{.l=libcardinal::jnienv->FindClass(idClass.c_str())},
                          {.l=libcardinal::jnienv->NewStringUTF(id.c_str())}};
        jobject item = libcardinal::call_instance(hardwareMap, "get",
                                                  "(Ljava/lang/Class;Ljava/lang/String;)Ljava/lang/Object;",
                                                  args).l;
        libcardinal::jnienv->DeleteLocalRef(args[0].l);
        return libcardinal::jnienv->NewGlobalRef(item);
    }

    jobject altenv_get_device_from_hardware_map(JNIEnv *env, jobject opMode, const std::string &id,
                                                const std::string &idClass
    ) {
        jobject hardwareMap = env->NewLocalRef(libcardinal::altenv_get_hardware_map(env, opMode));
        jvalue args[2] = {{.l=env->NewLocalRef(env->FindClass(idClass.c_str()))},
                          {.l=env->NewLocalRef(env->NewStringUTF(id.c_str()))}};
        jobject item = libcardinal::altenv_call_instance(env, hardwareMap, "get",
                                                         "(Ljava/lang/Class;Ljava/lang/String;)Ljava/lang/Object;",
                                                         args).l;
        env->DeleteLocalRef(args[0].l);
        env->DeleteLocalRef(args[1].l);
        env->DeleteLocalRef(hardwareMap);
        return env->NewGlobalRef(item);
    }

    jobject get_device_from_hardware_map(jobject opMode, const std::string &id) {
        jobject hardwareMap = libcardinal::get_hardware_map(opMode);
        jvalue args[1] = {{.l=libcardinal::jnienv->NewStringUTF(id.c_str())}};
        jobject item = libcardinal::call_instance(hardwareMap, "get",
                                                  "(Ljava/lang/String;)Ljava/lang/Object;",
                                                  args).l;
        return libcardinal::jnienv->NewGlobalRef(item);
    }

    jobject
    altenv_get_device_from_hardware_map(JNIEnv *env, jobject opMode, const std::string &id) {
        jobject hardwareMap = libcardinal::altenv_get_hardware_map(env, opMode);
        jvalue args[1] = {{.l=env->NewStringUTF(id.c_str())}};
        jobject item = libcardinal::altenv_call_instance(env, hardwareMap, "get",
                                                         "(Ljava/lang/String;)Ljava/lang/Object;",
                                                         args).l;
        return env->NewGlobalRef(item);
    }

    void kill(jobject opMode) {
        libcardinal::call_instance(opMode, "requestOpModeStop", "()V", nullptr);
        exit(3);
    }

    void altenv_kill(JNIEnv * env, jobject opMode) {
        libcardinal::altenv_call_instance(env, opMode, "requestOpModeStop", "()V", nullptr);
        exit(3);
    }
}
