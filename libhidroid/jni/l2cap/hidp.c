#include <jni.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/hidp.h>

#include "utils/throw.h"

void Java_net_hidroid_hidp_HidpSockOptSetter_setSockOpt(JNIEnv* env,
		jobject thiz, int sk)
{
	struct l2cap_options opts;
	memset(&opts, 0, sizeof(opts));
	opts.imtu = HIDP_DEFAULT_MTU;
	opts.omtu = HIDP_DEFAULT_MTU;
	opts.flush_to = 0xffff;

	if (setsockopt(sk, SOL_L2CAP, L2CAP_OPTIONS, &opts, sizeof(opts)) < 0)
	{
		Throw(env, "java/io/IOException",
				"Could not read on socket %d: %s", sk, strerror(errno));
	}
}
