/* Code from http://people.csail.mit.edu/albert/bluez-intro/x559.html
 *
 */

#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <errno.h>

jstring Java_net_hidroid_L2capSocket_test(JNIEnv* env, jobject thiz)
{
	char result[1000];
    struct sockaddr_l2 addr = { 0 };
    int s, status;
    char *message = "hello!";
    char dest[18] = "00:02:72:A4:7D:1F";

    /*if(argc < 2)
    {
        fprintf(stderr, "usage: %s <bt_addr>\n", argv[0]);
        exit(2);
    }

    strncpy(dest, argv[1], 18);*/

    // allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    if (s < 0)
    {
        sprintf(result, "Socket error: %d. ", errno);
    }
    else
    {
        strcpy(result, "Got socket. ");

        // set the connection parameters (who to connect to)
        addr.l2_family = AF_BLUETOOTH;
        addr.l2_psm = htobs(0x1001);
        str2ba( dest, &addr.l2_bdaddr );

        // connect to server
        status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

        // send a message
        if( status == 0 )
        {
        	strcat(result,"Connected. ");
            status = write(s, "hello!", 6);
            if (status == 0)
            	strcat(result,"Written. ");
            else
            	strcat(result,"Write error. ");
        }
        else
        {
        	strcat(result,"Connection error. ");
        }
    }

    close(s);

    return (*env)->NewStringUTF(env, result);
}

