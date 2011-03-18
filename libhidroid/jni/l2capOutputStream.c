#include <jni.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "throw.h"

static int getCurrentSocket(JNIEnv* env, jobject thiz)
{
	jfieldID fid = (*env)->GetFieldID(env, (*env)->GetObjectClass(env, thiz),
			"nativeSocket", "Lnet/hidroid/l2cap/NativeSocket;");
	jobject socket = (*env)->GetObjectField(env, thiz, fid);
	jclass clazz = (*env)->GetObjectClass(env, socket);
	jmethodID mid = (*env)->GetMethodID(env, clazz, "get", "()I");

	return (*env)->CallIntMethod(env, socket, mid);
}

int Java_net_hidroid_l2cap_L2capOutputStream_nativeWrite(JNIEnv* env,
		jobject thiz, jbyteArray buffer, int offset, int count)
{
	size_t bufferSize;
	ssize_t nWritten = 0;

	bufferSize = (*env)->GetArrayLength(env, buffer);

	if (offset < 0)
	{
		// Don't read in the wild
		offset = 0;
	}

	if (offset < bufferSize && count > 0) // Write only if needed
	{
		int s;
		char *bufferElements;

		if (count > (bufferSize - offset))
		{
			// Don't write more than we can
			count = bufferSize - offset;
		}

		s = getCurrentSocket(env, thiz);

		bufferElements = (*env)->GetByteArrayElements(env, buffer, NULL);
		nWritten = write(s, bufferElements + offset, count);
		(*env)->ReleaseByteArrayElements(env, buffer, bufferElements, JNI_ABORT);
		if (nWritten < 0)
		{
			Throw(env, "java/io/IOException",
					"Could not write on socket %d: %s", s, strerror(errno));
		}
	}

	return nWritten;
}
