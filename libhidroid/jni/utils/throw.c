#include <jni.h>
#include <stdio.h>
#include <stdarg.h>

#include "throw.h"

void Throw(JNIEnv* env, const char* exceptionName, const char* format, ...)
{
	jclass exceptionClass = (*env)->FindClass(env, exceptionName);

	if (exceptionClass != NULL)
	{
		char buffer[THROW_BUFFER_SIZE];
		va_list args;

		va_start(args, format);
		vsprintf(buffer, format, args);
		va_end(args);

		jstring message = (*env)->NewStringUTF(env, buffer);
		jmethodID mid = (*env)->GetMethodID(env, exceptionClass, "<init>",
				"(Ljava/lang/String;)V");
		jobject exception = (*env)->NewObject(env, exceptionClass, mid, message);
		(*env)->Throw(env,exception);
	}
}

