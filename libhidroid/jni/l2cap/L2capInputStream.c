#include <jni.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "utils/throw.h"

static int getCurrentSocket(JNIEnv* env, jobject thiz)
{
	jfieldID fid = (*env)->GetFieldID(env, (*env)->GetObjectClass(env, thiz),
			"nativeSocket", "Lnet/hidroid/l2cap/NativeSocket;");
	jobject socket = (*env)->GetObjectField(env, thiz, fid);
	jclass clazz = (*env)->GetObjectClass(env, socket);
	jmethodID mid = (*env)->GetMethodID(env, clazz, "get", "()I");

	return (*env)->CallIntMethod(env, socket, mid);
}

int Java_net_hidroid_l2cap_L2capInputStream_nativeRead(JNIEnv* env,
		jobject thiz, jbyteArray buffer, int offset, int count)
{
	size_t bufferSize;
	ssize_t nRead = 0;

	bufferSize = (*env)->GetArrayLength(env, buffer);

	if (offset < 0)
	{
		// Don't write in the wild
		offset = 0;
	}

	if (offset < bufferSize && count > 0) // Read only if possible
	{
		int s;
		char *bufferElements;

		if (count > (bufferSize - offset))
		{
			// Don't read more than we can
			count = bufferSize - offset;
		}

		s = getCurrentSocket(env, thiz);

		bufferElements = (*env)->GetByteArrayElements(env, buffer, NULL);
		nRead = read(s, bufferElements, count);

		if (nRead < 0)
		{
			(*env)->ReleaseByteArrayElements(env, buffer, bufferElements,
					JNI_ABORT);
			Throw(env, "java/io/IOException",
					"Could not read on socket %d: %s", s, strerror(errno));
		}
		else if (nRead == 0)
		{
			(*env)->ReleaseByteArrayElements(env, buffer, bufferElements,
					JNI_ABORT);
			Throw(env, "java/io/IOException",
					"Could not read on socket %d: Unknown error", s);
		}
		else
		{
			(*env)->ReleaseByteArrayElements(env, buffer, bufferElements,
					JNI_COMMIT);
		}
	}

	return nRead;
}
