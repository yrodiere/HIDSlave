/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <jni.h>

#include <android/log.h> 

#include <errno.h>

#define LOGV(...) __android_log_write(ANDROID_LOG_VERBOSE, "HID Emul", __VA_ARGS__)
#define LOGD(...) __android_log_write(ANDROID_LOG_DEBUG  , "HID Emul", __VA_ARGS__)
#define LOGI(...) __android_log_write(ANDROID_LOG_INFO   , "HID Emul", __VA_ARGS__)
#define LOGW(...) __android_log_write(ANDROID_LOG_WARN   , "HID Emul", __VA_ARGS__)
#define LOGE(...) __android_log_write(ANDROID_LOG_ERROR  , "HID Emul", __VA_ARGS__) 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include "bluetooth/bluetooth.h"
#include "bluetooth/l2cap.h"
#include "bluetooth/hci.h"
#include "bluetooth/hci_lib.h"

/* from original bluez hidp.h */
#define HIDP_MINIMUM_MTU 48
#define HIDP_DEFAULT_MTU 48

/* from stuffkeys.c but you can found it on one of the original bluez file ... maybe ...*/
#define L2CAP_PSM_HIDP_CTRL 0x11
#define L2CAP_PSM_HIDP_INTR 0x13

/* L2CAP socket options from original bluez l2cap.h */
#define L2CAP_OPTIONS	0x01
struct l2cap_options {
	uint16_t	omtu;
	uint16_t	imtu;
	uint16_t	flush_to;
	uint8_t		mode;
	uint8_t		fcs;
	uint8_t		max_tx;
	uint16_t	txwin_size;
};

/* inquiry_info from hci.h */
typedef struct {
	bdaddr_t	bdaddr;
	uint8_t		pscan_rep_mode;
	uint8_t		pscan_period_mode;
	uint8_t		pscan_mode;
	uint8_t		dev_class[3];
	uint16_t	clock_offset;
} __attribute__ ((packed)) inquiry_info;

int is = 0,  iss = 0, cs = 0, css = 0;
char logmsg[256];


void sendChar( char keycode)
{
    unsigned char pkg[12];
    int ret;

    pkg[0] = 0xa1;
    pkg[1] = 0x01;
    pkg[2] = 0x00; // modifiers ?
    pkg[3] = 0x00;
    pkg[4] = keycode;
    pkg[5] = 0x00;
    pkg[6] = 0x00;
    pkg[7] = 0x00;
    pkg[8] = 0x00;
    pkg[9] = 0x00;

    //if ( write(is, pkg, 10) <= 0) {
        ret = write(is, pkg, 12);
        
        if (errno == EBADF)
            LOGE("EBADF");
        if (errno == EINVAL)
            LOGE("EINVAL");
        if (errno == EFAULT)
            LOGE("EFAULT");
        if (errno == EPIPE)
            LOGE("EPIPE");
        if (errno == EAGAIN)
            LOGE("EAGAIN");
        if (errno == EINTR)
            LOGE("EINTR");
        if (errno == ENOSPC)
            LOGE("ENOSPC");
        if (errno == EIO)
            LOGE("EIO");
            
        sprintf ( logmsg, "HID EMUL : sending key : %d", ret );
        LOGE( logmsg );
        
    //}
}


static int l2cap_connect(bdaddr_t *src, bdaddr_t *dst, unsigned short psm)
{
	struct sockaddr_l2 addr;
	struct l2cap_options opts;
	int sk;
	
    sprintf ( logmsg, "HID EMUL : before error errno : %d", errno );
    LOGV( logmsg );


    // PF_BLUETOOTH vs. AF_BLUETOOTH ?? BTPROTO_L2CAP
	if ((sk = socket(PF_BLUETOOTH, SOCK_SEQPACKET,  BTPROTO_L2CAP)) < 0)
	{
	    if(errno == EACCES)
	        LOGE("EACCES");
        if(errno == EAFNOSUPPORT)
	        LOGE("EAFNOSUPPORT");
        if(errno == EINVAL)
	        LOGE("EINVAL");
        if(errno == EMFILE)
	        LOGE("EMFILE");
        if(errno == ENFILE)
	        LOGE("ENFILE");
        if(errno == ENOBUFS)
	        LOGE("ENOBUFS");
        if(errno == ENOMEM)
	        LOGE("ENOMEM");
        if(errno == EPROTONOSUPPORT )
	        LOGE("EPROTONOSUPPORT");

	    sprintf ( logmsg, "HID EMUL : can't open socket bluetooth : errno : %d, return : %d", errno, sk );
        LOGE( logmsg );
		return -1;
    }
    
	memset(&addr, 0, sizeof(addr));
	addr.l2_family  = AF_BLUETOOTH;
	bacpy(&addr.l2_bdaddr, src);

	if (bind(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		close(sk);
		return -1;
	}

	memset(&opts, 0, sizeof(opts));
	opts.imtu = HIDP_DEFAULT_MTU;
	opts.omtu = HIDP_DEFAULT_MTU;
	opts.flush_to = 0xffff;

	setsockopt(sk, SOL_L2CAP, L2CAP_OPTIONS, &opts, sizeof(opts));

	memset(&addr, 0, sizeof(addr));
	addr.l2_family  = AF_BLUETOOTH;
	bacpy(&addr.l2_bdaddr, dst);
	addr.l2_psm = htobs(psm);

	if (connect(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		close(sk);
		return -1;
	}

	return sk;
}

/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/HelloJni/HelloJni.java
 */
jstring
Java_com_example_hellojni_HelloJni_stringFromJNI( JNIEnv* env,
                                                  jobject thiz )
{

    bdaddr_t dst;
    char keycode;
    
    str2ba("00:10:60:A8:57:35", &dst);

    LOGV("======================= Begin ====================");

    LOGV("connecting HID control channel to host");
    while(cs <= 0)
    {
        cs = l2cap_connect(BDADDR_ANY, &dst, L2CAP_PSM_HIDP_CTRL);
        if (cs < 0) {
		    LOGE(" -> connect failed");
		    sleep(1);
		}
		else
		{
            sprintf ( logmsg, " -> connected : cs = %d", cs );
            LOGV( logmsg );
        }
    }
    
    LOGV("connecting HID interrupt channel to host");

    while(is <= 0)
    {
        is = l2cap_connect(BDADDR_ANY, &dst, L2CAP_PSM_HIDP_INTR);
        if (is < 0) {
	        LOGE(" -> connect failed");
	        sleep(1);
        }
        else
        {
            sprintf ( logmsg, " -> connected : is = %d", is );
            LOGV( logmsg );        
        }
    }    

    
    // from AndroHID : a=4, b=5, c= 6 ... 0=39
    
    for( keycode = 4 ; keycode < 39; keycode++ )
    {
	    sprintf ( logmsg, "sending keycode %d", keycode);
        LOGV( logmsg );
        
	    sendChar(keycode); // key up
	    sendChar(0); // key down
    }
    
	close(is);
	close(cs);

    LOGV("======================== End =====================");

    return (*env)->NewStringUTF(env, "Hello from HID and JNI too !");
}

/*
================================================================================
                         TEST CODE FROM SIMPLE SCAN
================================================================================
*/
/*{
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[19] = { 0 };
    char name[248] = { 0 };

    // hci_get_route(NULL) return the id of the first available adapter
    dev_id = hci_get_route(NULL);
    
    sprintf ( logmsg, "dev_id found : %d", dev_id );
    LOGV( logmsg ); 

    // hci_open_dev return a socket opened on the adapter pointed by dev_id
    sock = hci_open_dev( dev_id );
    if (dev_id < 0 || sock < 0) {
        sprintf ( logmsg, " can't open sock : errno = %d", errno );
        LOGE( logmsg ); 
        exit(1);
    }



    len  = 8;
    max_rsp = 255;

    // flush the inquiry cache to not display out of range device (ghost devices)
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
    
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if( num_rsp < 0 )
        LOGE("hci_inquiry");

    for (i = 0; i < num_rsp; i++) {
        ba2str(&(ii+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), 
            name, 0) < 0)
        strcpy(name, "[unknown]");
        sprintf(logmsg, "%s  %s\n", addr, name);
        LOGV(logmsg);
    }

    free( ii );
    close( sock );
    return (*env)->NewStringUTF(env, "Hello from HID and JNI too !");
}*/
