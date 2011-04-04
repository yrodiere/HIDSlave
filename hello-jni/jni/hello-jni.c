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
#include <sys/poll.h>
#include "bluetooth/bluetooth.h"
#include "bluetooth/l2cap.h"
#include "bluetooth/hci.h"
#include "bluetooth/hci_lib.h"
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

/* from original bluez hidp.h */
#define HIDP_MINIMUM_MTU 48
#define HIDP_DEFAULT_MTU 185

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

#define NUMFDS 2
#define BUFLEN 65536

static const char* FD_NAMES[] = {
	"control out",
	"interrupt out",
	"control in",
	"interrupt in"
};

static const char ACK[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

sdp_record_t *sdp_record;
sdp_session_t *sdp_session;

int is = 0,  iss = 0, cs = 0, css = 0;
char logmsg[256];
int ret;
unsigned char msg[12];

void sendChar( char keycode)
{
    unsigned char pkg[12];

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
    
    
    // Mouse datagram
    /*
    pkg[0] = 0xa1;
    pkg[1] = 0x02;
    pkg[2] = 0x00; // modifiers ?
    pkg[3] = 0xFF;
    pkg[4] = 0xFF;
    pkg[5] = 0x00;
    pkg[6] = 0x00;
    pkg[7] = 0x00;
    pkg[8] = 0x00;
    pkg[9] = 0x00;*/
    

    //if ( write(is, pkg, 10) <= 0) {
        ret = send(is, pkg, 10, 0);
        
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

	if ((sk = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP)) < 0)
		return -1;

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

static void add_lang_attr(sdp_record_t *r)
{
	sdp_lang_attr_t base_lang;
	sdp_list_t *langs = 0;

	/* UTF-8 MIBenum (http://www.iana.org/assignments/character-sets) */
	base_lang.code_ISO639 = (0x65 << 8) | 0x6e;
	base_lang.encoding = 106;
	base_lang.base_offset = SDP_PRIMARY_LANG_BASE;
	langs = sdp_list_append(0, &base_lang);
	sdp_set_lang_attr(r, langs);
	sdp_list_free(langs, 0);
}

void sdp_add_keyboard()
{
	sdp_list_t *svclass_id, *pfseq, *apseq, *root;
	uuid_t root_uuid, hidkb_uuid, l2cap_uuid, hidp_uuid;
	sdp_profile_desc_t profile[1];
	sdp_list_t *aproto, *proto[3];
	sdp_data_t *channel, *lang_lst, *lang_lst2, *hid_spec_lst, *hid_spec_lst2;
	int i;
	uint8_t dtd = SDP_UINT16;
	uint8_t dtd2 = SDP_UINT8;
	uint8_t dtd_data = SDP_TEXT_STR8;
	sdp_session_t *session;
	void *dtds[2];
	void *values[2];
	void *dtds2[2];
	void *values2[2];
	int leng[2];
	uint8_t hid_spec_type = 0x22;
	uint16_t hid_attr_lang[] = {0x409,0x100};

	static const uint8_t ctrl = L2CAP_PSM_HIDP_CTRL;
	static const uint8_t intr = L2CAP_PSM_HIDP_INTR;

	static const uint16_t hid_attr[] = {0x100,0x111,0x40,0x0d,0x01,0x01};
        /* SDP_ATTR_HID_DEVICE_RELEASE_NUMBER = 0x100
         * SDP_ATTR_HID_PARSER_VERSION = 0x111
         * SDP_ATTR_HID_DEVICE_SUBCLASS = 0x40
         * SDP_ATTR_HID_COUNTRY_CODE = 0x0d
         * SDP_ATTR_HID_VIRTUAL_CABLE = 0x01
         * SDP_ATTR_HID_RECONNECT_INITIATE = 0x01
         */
	
	static const uint16_t hid_attr2[] = {0x0,0x01,0x100,0x1f40,0x01,0x01};
	// taken from Apple Wireless Keyboard
	const uint8_t hid_spec[] = { 
		0x05, 0x01, // usage page
		0x09, 0x06, // keyboard
		0xa1, 0x01, // key codes
		0x85, 0x01, // minimum
		0x05, 0x07, // max
		0x19, 0xe0, // logical min
		0x29, 0xe7, // logical max
		0x15, 0x00, // report size
		0x25, 0x01, // report count
		0x75, 0x01, // input data variable absolute
		0x95, 0x08, // report count
		0x81, 0x02, // report size
		0x75, 0x08, 
		0x95, 0x01, 
		0x81, 0x01, 
		0x75, 0x01, 
		0x95, 0x05,
		0x05, 0x08,
		0x19, 0x01,
		0x29, 0x05, 
		0x91, 0x02,
		0x75, 0x03,
		0x95, 0x01,
		0x91, 0x01,
		0x75, 0x08,
		0x95, 0x06,
		0x15, 0x00,
		0x26, 0xff,
		0x00, 0x05,
		0x07, 0x19,
		0x00, 0x2a,
		0xff, 0x00,
		0x81, 0x00,
		0x75, 0x01,
		0x95, 0x01,
		0x15, 0x00,
		0x25, 0x01,
		0x05, 0x0c,
		0x09, 0xb8,
		0x81, 0x06,
		0x09, 0xe2,
		0x81, 0x06,
		0x09, 0xe9,
		0x81, 0x02,
		0x09, 0xea,
		0x81, 0x02,
		0x75, 0x01,
		0x95, 0x04,
		0x81, 0x01,
		0xc0         // end tag
	};


	if (!sdp_session) {
		LOGE("sdp_session invalid");
		exit(-1);
	}
	session = sdp_session;

	sdp_record = sdp_record_alloc();
	if (!sdp_record) {
		LOGE("add_keyboard sdp_record_alloc");
		exit(-1);
	}

	memset((void*)sdp_record, 0, sizeof(sdp_record_t));
	// handle ?????
	sdp_record->handle = 0xffffffff;

	// root uuid -> PUBLIC_BROWSE_GROUP
	sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
	root = sdp_list_append(0, &root_uuid);	
	sdp_set_browse_groups(sdp_record, root);

	// lang_attr : voir plus haut.
	add_lang_attr(sdp_record);
	
	// Service class UUID -> HID_SVCLASS_ID
	sdp_uuid16_create(&hidkb_uuid, HID_SVCLASS_ID);
	svclass_id = sdp_list_append(0, &hidkb_uuid);
	sdp_set_service_classes(sdp_record, svclass_id);

	// profile
	sdp_uuid16_create(&profile[0].uuid, HID_PROFILE_ID);
	profile[0].version = 0x0100;
	pfseq = sdp_list_append(0, profile);
	sdp_set_profile_descs(sdp_record, pfseq);

	// PROTO ///////////////////
	/*
	 * Possible liaison avec L2CAP ?
	 * Apparement, les listes chainé sont plus que trés présente.
	 * chaque nouveau paramètre est l'occasion d'en créer une.
	 */
	 
	 //APROTO
	     //APSEQ
	        // PROTO[1]
            // L2CAP uuid
            sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
            proto[1] = sdp_list_append(0, &l2cap_uuid);

            // L2CAP channel Control
            channel = sdp_data_alloc(SDP_UINT8, &ctrl);
            proto[1] = sdp_list_append(proto[1], channel);
        apseq = sdp_list_append(0, proto[1]);

            // PROTO[2]
            // HIDP uuid
            sdp_uuid16_create(&hidp_uuid, HIDP_UUID);
            proto[2] = sdp_list_append(0, &hidp_uuid);
        apseq = sdp_list_append(apseq, proto[2]);

    aproto = sdp_list_append(0, apseq);
    sdp_set_access_protos(sdp_record, aproto);

	// ATTR_ADD_PROTO  /////////
	
	// APROTO
	    // APSEQ
	        // PROTO[1]
	        // L2CAP uuid
	        proto[1] = sdp_list_append(0, &l2cap_uuid);
	        
	        // L2CAP channel Interruption
	        channel = sdp_data_alloc(SDP_UINT8, &intr);
	        proto[1] = sdp_list_append(proto[1], channel);
	    apseq = sdp_list_append(0, proto[1]);

            // PROTO[2]
	        sdp_uuid16_create(&hidp_uuid, HIDP_UUID);
	        proto[2] = sdp_list_append(0, &hidp_uuid);
	    apseq = sdp_list_append(apseq, proto[2]);

	aproto = sdp_list_append(0, apseq);
	sdp_set_add_access_protos(sdp_record, aproto);
	
	
	// Description
	sdp_set_info_attr(sdp_record, "droidHID", 
		"gravitezero & fenrhil", "original code writer : http://www.mulliner.org/bluetooth/");

	for (i = 0; i < sizeof(hid_attr)/2; i++) {
		sdp_attr_add_new(sdp_record, SDP_ATTR_HID_DEVICE_RELEASE_NUMBER+i, SDP_UINT16, &hid_attr[i]);
		/* SDP_ATTR_HID_DEVICE_RELEASE_NUMBER = 0x100
         * SDP_ATTR_HID_PARSER_VERSION = 0x111
         * SDP_ATTR_HID_DEVICE_SUBCLASS = 0x40
         * SDP_ATTR_HID_COUNTRY_CODE = 0x0d
         * SDP_ATTR_HID_VIRTUAL_CABLE = 0x01
         * SDP_ATTR_HID_RECONNECT_INITIATE = 0x01
         */
	}

	dtds[0] = &dtd2;
	values[0] = &hid_spec_type;
	dtds[1] = &dtd_data;
	values[1] = (uint8_t*)hid_spec;
	leng[0] = 0;
	leng[1] = sizeof(hid_spec);
	hid_spec_lst = sdp_seq_alloc_with_length(dtds, values, leng, 2);
	hid_spec_lst2 = sdp_data_alloc(SDP_SEQ8, hid_spec_lst);	
	sdp_attr_add(sdp_record, SDP_ATTR_HID_DESCRIPTOR_LIST, hid_spec_lst2);
	
	for (i = 0; i < sizeof(hid_attr_lang)/2; i++) {
		dtds2[i] = &dtd;
		values2[i] = &hid_attr_lang[i];
	}
	
	lang_lst = sdp_seq_alloc(dtds2, values2, sizeof(hid_attr_lang)/2);
	lang_lst2 = sdp_data_alloc(SDP_SEQ8, lang_lst);	
	sdp_attr_add(sdp_record, SDP_ATTR_HID_LANG_ID_BASE_LIST, lang_lst2);

	sdp_attr_add_new(sdp_record, SDP_ATTR_HID_SDP_DISABLE, SDP_UINT16, &hid_attr2[0]);
	for (i = 0; i < sizeof(hid_attr2)/2-1; i++) {
		sdp_attr_add_new(sdp_record, SDP_ATTR_HID_REMOTE_WAKEUP+i, SDP_UINT16, &hid_attr2[i+1]);
	}
	
	if (sdp_record_register(session, sdp_record, 0) < 0) {
		LOGE("HID Device Service Record registration failed");
		exit(-1);
	}
}

void sdp_remove()
{
	if (sdp_record && sdp_record_unregister(sdp_session, sdp_record)) {
		LOGE("HID Device Service Record unregistration failed");
	}
	
	sdp_close(sdp_session);
}

void sdp_open()
{
    LOGV("Open SDP record");
	if (!sdp_session) {
		sdp_session = sdp_connect(BDADDR_ANY, BDADDR_LOCAL, 0);
		LOGV("new sdp session\n");
	}
	if (!sdp_session) {
		sprintf ( logmsg, "sdp_session invalid : %d", sdp_session );
        LOGE( logmsg );
		exit(-1);
	}
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

    bdaddr_t dst, src;
    char keycode;
    int numrdy;
    struct pollfd pf[NUMFDS];
    int i, j, len;
    unsigned char *buf;
    
    buf = malloc(BUFLEN);
	if (!buf) {
		perror("can't allocate buffer");
		exit(1);
	}
    
    str2ba("00:10:60:A8:57:35", &dst);
    str2ba("C8:7E:75:51:46:D7", &src);

    LOGV("======================= Begin ====================");

    LOGV("Add SDP record");
    
    sdp_open();
    
    LOGV("Opened SDP record");
    
	sdp_add_keyboard();

    LOGV("connecting HID control channel to host");
    while(cs <= 0)
    {
        cs = l2cap_connect(&src, &dst, L2CAP_PSM_HIDP_CTRL);
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
    
    msg[0] = 0x10;
    
    
    ret = send(cs, msg, 1, 0);
    sprintf ( logmsg, "HID EMUL : sending NOP on ctrl chan, ret : %d", ret );
    LOGE( logmsg );
    
    
    LOGV("connecting HID interrupt channel to host");

    while(is <= 0)
    {
        is = l2cap_connect(&src, &dst, L2CAP_PSM_HIDP_INTR);
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

	pf[0].fd = cs;
	pf[0].events = POLLIN;
	pf[1].fd = is;
	pf[1].events = POLLIN;

    numrdy = poll(pf, NUMFDS, 1);
	if (numrdy < 0) {
		LOGE("poll failed");
		exit(1);
	}

	for (i = 0; i < NUMFDS; i++) {
		if (pf[i].revents & POLLIN) {

			LOGV("receive data");
			len = recv(pf[i].fd, buf, BUFLEN, 0);
			if (len < 0) {
				LOGE("recv failed");
				exit(1);
			} else if (len == 0) {
				LOGW("disconnected\n");
				continue;
			}

			/* print to screen */
			sprintf(logmsg, "%-13s:", FD_NAMES[i]);
			LOGV( logmsg );
			for (j = 0; j < len; j++)
				sprintf(logmsg[j], " %02x", buf[j]);
			LOGV(logmsg);

			/* ack it */
			if (send(pf[0].fd, ACK, 10, 0) <= 0) {
				LOGE("send failed");
				exit(1);
			}
		}
	}

    sleep(10);

    // from AndroHID : a=4, b=5, c= 6 ... 0=39
    
    for( keycode = 16 ; keycode < 100; keycode++ )
    {
	    sprintf ( logmsg, "sending keycode %d", keycode);
        LOGV( logmsg );
        
	    sendChar(keycode); // key down
	    sendChar(0); // key up
	    sleep(1);
    }
    
	close(is);
	sleep(2);
	close(cs);

    LOGV("======================== End =====================");

    return (*env)->NewStringUTF(env, "Hello from HID and JNI too !");
}
