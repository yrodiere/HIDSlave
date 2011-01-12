/*
 *  xkbd-bthid
 *
 *  Collin R. Mulliner <collin@betaversion.net>
 *
 *  http://www.mulliner.org/bluetooth/
 *
 *  License: GPL
 *
 */

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/input.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/sdp.h>
#include <bluetooth/hidp.h>



#include "hid.h"

#include "structs.h"
#include "kb.h"
#include "button.h"

// from xkbd.c
extern int bthid_pid;

//int cs;
//int is;
int es[2];

int bthid_open()
{
	pipe(es);
}

int bthid_close()
{
	close(es[0]);
	close(es[1]);
}

int bthid_send(int key, int updown)
{
	struct input_event e;

	bzero(&e, sizeof(e));
	e.code = key;
	e.value = updown;
	if (key == 0xffff) e.type = 0xff;
	else e.type = EV_KEY;
	write(es[1], &e, sizeof(e));
	return(1);
}

static int l2cap_listen(const bdaddr_t *bdaddr, unsigned short psm, int lm, int backlog)
{
	struct sockaddr_l2 addr;
	struct l2cap_options opts;
	int sk;

	if ((sk = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP)) < 0)
		return -1;

	memset(&addr, 0, sizeof(addr));
	addr.l2_family = AF_BLUETOOTH;
	bacpy(&addr.l2_bdaddr, bdaddr);
	addr.l2_psm = htobs(psm);

	if (bind(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		close(sk);
		return -1;
	}

	setsockopt(sk, SOL_L2CAP, L2CAP_LM, &lm, sizeof(lm));

	memset(&opts, 0, sizeof(opts));
	opts.imtu = HIDP_DEFAULT_MTU;
	opts.omtu = HIDP_DEFAULT_MTU;
	opts.flush_to = 0xffff;

	setsockopt(sk, SOL_L2CAP, L2CAP_OPTIONS, &opts, sizeof(opts));

	if (listen(sk, backlog) < 0) {
		close(sk);
		return -1;
	}

	return sk;
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

static int l2cap_accept(int sk, bdaddr_t *bdaddr)
{
	struct sockaddr_l2 addr;
	socklen_t addrlen;
	int nsk;

	memset(&addr, 0, sizeof(addr));
	addrlen = sizeof(addr);

	if ((nsk = accept(sk, (struct sockaddr *) &addr, &addrlen)) < 0)
		return -1;

	if (bdaddr)
		bacpy(bdaddr, &addr.l2_bdaddr);

	return nsk;
}

//int main(int argc, char **argv)
int bthid(int ip)
{
	unsigned char pkg[12];
	//int ip;
	int i = 0;
	struct input_event ie;
	bdaddr_t dst;
	struct pollfd pf[3];
	unsigned char ib[1024];
	unsigned char cb[1024];
	int state = 0;
	unsigned char modifiers = 0;
	unsigned char modifiers_old = 0;
	int press = 0;
	int press_old = 0;
	int bitmask[8] = {0};
	int is, iss, cs, css;

	
	css = l2cap_listen(BDADDR_ANY, L2CAP_PSM_HIDP_CTRL, 0, 1);
	perror("");
	iss = l2cap_listen(BDADDR_ANY, L2CAP_PSM_HIDP_INTR, 0, 1);
	perror("");

	for (;;) {
	
	cs = l2cap_accept(css, NULL);
	//cs = l2cap_connect(BDADDR_ANY, &dst, L2CAP_PSM_HIDP_CTRL);
	//perror("");
	is = l2cap_accept(iss, NULL);
	//is = l2cap_connect(BDADDR_ANY, &dst, L2CAP_PSM_HIDP_INTR);
	//perror("");

	pf[0].fd = ip;
	pf[0].events = POLLIN | POLLERR | POLLHUP;
	pf[1].fd = cs;
	pf[1].events = POLLIN | POLLERR | POLLHUP;
	pf[2].fd = is;
	pf[2].events = POLLIN | POLLERR | POLLHUP;
	
	while (1) {
		pf[0].revents = 0;
		pf[1].revents = 0;
		pf[2].revents = 0;
		
		if (poll(pf, 3, -1) <= 0) {
			goto out;	
		}
		
		bzero(pkg, 12);

		if (pf[0].revents) {
			if (read(ip, &ie, sizeof(ie)) <= 0) {
				perror("read ip");
				exit(-1);
			}
			if (ie.type == 0xff) {
				// quit key
				if (ie.value == 0xffff) {
					//close(is);
					//close(cs);
					//close(ip);
					//exit(0);
					goto out;
				}
			}
			else if (ie.type == 0x01) {
				if (state == 1) {

					modifiers_old = modifiers;
					press_old = press;

					switch (ie.code) {
					case KEY_LEFTSHIFT:
						bitmask[1] = (ie.value ? 1 : 0);
						break;
					case KEY_RIGHTSHIFT:
						bitmask[5] = (ie.value ? 1 : 0);
						break;
					case KEY_LEFTALT:
						bitmask[2] = (ie.value ? 1 : 0);
						break;
					case KEY_RIGHTALT:
						bitmask[6] = (ie.value ? 1 : 0);
						break;
					case KEY_LEFTCTRL:
						bitmask[0] = (ie.value ? 1 : 0);
						break;
					case KEY_RIGHTCTRL:
						bitmask[4] = (ie.value ? 1 : 0);
						break;
					default:
						if (ie.value > 0) {
							pkg[4] = keycode2hidp[ie.code];
							//printf("keycode=%d, hidp=%d\n",ie.code,pkg[4]);
							press++;
						}
						else {
							press--;
						}
						break;
					}
					
					modifiers = 0;
					for (i = 0; i < 8; i++) {
						modifiers |= (bitmask[i] << i);
					}
					//fprintf(stderr, "modifiers: 0x%02x\n", modifiers);
					
					if (press != press_old || modifiers != modifiers_old) {
						pkg[0] = 0xa1;
						pkg[1] = 0x01;
						pkg[2] = modifiers;
						pkg[3] = 0x00;
						//pkg[4] = 0x00; // the key code
						pkg[5] = 0x00;
						pkg[6] = 0x00;
						pkg[7] = 0x00;
						pkg[8] = 0x00;
						pkg[9] = 0x00;
					
					if (write(is, pkg, 10) <= 0) {
							perror("write");
							exit(-1);
						}
					}
				}
			}
		}
		if (pf[1].revents) {
			int size;
			int i;
			if ((size = read(cs, cb, sizeof(cb))) <= 0) {
				perror("read cs");
				//exit(-1);
				goto out;
			}
			//printf("cs(%d)\n", size);
			
/*			for (i = 0; i < size; i++)
				printf("%02x",cb[i]);
			printf("\n");
*/
			if (state == 0 && size == 1 && cb[0] == 0x70) {
				//printf("got set_protocol BOOT\n");
				pkg[0] = 0;
				write(cs, pkg, 1);
				state = 1;
			}
		}
		if (pf[2].revents) {
			int size;
			int i;
			if ((size = read(is, ib, sizeof(ib))) <= 0) {
				perror("read is");
				//exit(-1);
				goto out;
			}
/*			printf("is(%d): ", size);
			for (i = 0; i < size; i++)
				printf("%02x",ib[i]);
			printf("\n");
*/		
		}
	}
out:
	printf("disconnected\n");
	close(cs);
	close(is);
	}
}

int bthid_process_keypress(button *active_but, int updown)
{
	int new_state = active_but->kb->state;


	if (active_but->modifier/* is a shift / ctrl / mod pressed */
			&& !(active_but->modifier & BUT_CAPS) ) {
		if (active_but->kb->state_locked & active_but->modifier) {
			/* was locked then unset & unlock */
			active_but->kb->state_locked ^= active_but->modifier;
			new_state ^= active_but->modifier;
		}
		else if (new_state & active_but->modifier) {
			/* was set then lock */
			active_but->kb->state_locked ^= active_but->modifier;
		}
		else {
			/* was unset then set */
			new_state ^= active_but->modifier;
		}
	}
	/* deal with caps key - maybe this can go above now ?*/
	else if (active_but->modifier & BUT_CAPS) {
		new_state ^= KB_STATE_CAPS; /* turn caps on/off */
	}
	else if ((active_but->kb->state & KB_STATE_SHIFT)
			|| (active_but->kb->state & KB_STATE_MOD)
			|| (active_but->kb->state & KB_STATE_CTRL)
			|| (active_but->kb->state & KB_STATE_META)
			|| (active_but->kb->state & KB_STATE_ALT) ) {
		/* check if the kbd is already in a state and reset it
		 *        leaving caps key state alone */
		new_state &= KB_STATE_CAPS;
		new_state |= active_but->kb->state_locked;
	}

/*
	if (active_but->kb->state & KB_STATE_SHIFT) {
		bthid_send(42, 1);
	}
	else if (active_but->kb->state & KB_STATE_CTRL) {
		bthid_send(29, 1);
	}
	else if (active_but->kb->state & KB_STATE_ALT) {
		bthid_send(56, 1);
	}
*/

	// ALT_L
	if (active_but->scancode == 56) {
		if (updown) {
			if (active_but->kb->bthid_state & KB_STATE_ALT_L) {
				bthid_send(56, 0);
				active_but->kb->bthid_state &= (!KB_STATE_ALT_L);
			}
			else {
				bthid_send(56, 1);
				active_but->kb->bthid_state |= KB_STATE_ALT_L;
			}
		}
	}
	// CTRL_L 
	else if (active_but->scancode == 29) {
		if (updown) {
			if (active_but->kb->bthid_state & KB_STATE_CTRL_L) {
				bthid_send(29, 0);
				active_but->kb->bthid_state &= (!KB_STATE_CTRL_L);
			}
			else {
				bthid_send(29, 1);
				active_but->kb->bthid_state |= KB_STATE_CTRL_L;
			}
		}
	}
	// SHIFT_L
	else if (active_but->scancode == 42) {
		if (updown) {
			if (active_but->kb->bthid_state & KB_STATE_SHIFT_L) {
				bthid_send(42, 0);
				active_but->kb->bthid_state &= (!KB_STATE_SHIFT_L);
			}
			else {
				bthid_send(42, 1);
				active_but->kb->bthid_state |= KB_STATE_SHIFT_L;
			}
		}
	}
	// release shift/ctrl/alt when pressing other key
	else {
		// first send key
		bthid_send(active_but->scancode, updown);
		if (active_but->scancode == 0xffff) {
			close(es[1]);
			kill(bthid_pid, SIGTERM);
			exit(0);
		}

		if (active_but->kb->bthid_state & KB_STATE_ALT_L) {
			bthid_send(56, 0);
			active_but->kb->bthid_state &= (!KB_STATE_ALT_L);
		}
		if (active_but->kb->bthid_state & KB_STATE_CTRL_L) {
			bthid_send(29, 0);
			active_but->kb->bthid_state &= (!KB_STATE_CTRL_L);
		}
		if (active_but->kb->bthid_state & KB_STATE_SHIFT_L) {
			bthid_send(42, 0);
			active_but->kb->bthid_state &= (!KB_STATE_SHIFT_L);
		}
	}

	return new_state;
}
