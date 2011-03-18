#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <errno.h>
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

static void setCurrentSocket(JNIEnv* env, jobject thiz, int newValue)
{
	jfieldID fid = (*env)->GetFieldID(env, (*env)->GetObjectClass(env, thiz),
			"nativeSocket", "Lnet/hidroid/l2cap/NativeSocket;");
	jobject socket = (*env)->GetObjectField(env, thiz, fid);
	jclass clazz = (*env)->GetObjectClass(env, socket);
	jmethodID mid = (*env)->GetMethodID(env, clazz, "set", "(I)V");

	(*env)->CallVoidMethod(env, socket, mid, newValue);
}

void Java_net_hidroid_l2cap_L2capSocket_getNativeSocket(JNIEnv* env,
		jobject thiz)
{
	int s;
	//s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_L2CAP);

	setCurrentSocket(env, thiz, s);
	if (s < 0)
	{
		Throw(env, "java/io/IOException", "Could not get any socket: %s",
				strerror(errno));
	}
}

void Java_net_hidroid_l2cap_L2capSocket_nativeConnect(JNIEnv* env,
		jobject thiz, jstring address, int timeout)
{
	struct sockaddr_l2 addr =
	{ 0 };
	int s, status;
	const char *addressCString;

	// Set up parameters
	addr.l2_family = AF_BLUETOOTH;
	addr.l2_psm = htobs(0x1001);
	addressCString = (*env)->GetStringUTFChars(env, address, NULL);
	str2ba(addressCString, &addr.l2_bdaddr);
	(*env)->ReleaseStringUTFChars(env, address, addressCString);

	// Connect
	s = getCurrentSocket(env, thiz);
	status = connect(s, (struct sockaddr *) &addr, sizeof(addr));
	if (status < 0)
	{
		Throw(env, "java/io/IOException", "Could not connect on socket %d: %s",
				s, strerror(errno));
	}
}

void Java_net_hidroid_l2cap_L2capSocket_nativeClose(JNIEnv* env, jobject thiz)
{
	int s, status;
	s = getCurrentSocket(env, thiz);
	status = close(s);

	if (status < 0)
	{
		Throw(env, "java/io/IOException", "Could not close socket %d: %s", s,
				strerror(errno));
	}
	else
	{
		setCurrentSocket(env, thiz, -1);
	}
}
