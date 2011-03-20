#ifndef THROW_H
#define THROW_H
#include <jni.h>

static const size_t THROW_BUFFER_SIZE = 256;

void Throw(JNIEnv* env, const char* exceptionName, const char* format, ...);

#endif // THROW_H
