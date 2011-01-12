/*
 *  xkbd-bthid-wrapper (for Nokia770)
 *
 *  (c) Collin R. Mulliner <collin@betaversion.net>
 *  http://www.mulliner.org/nokia770/
 * 
 *  License: GPL
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define USAGE "\nxkbd-bthid-wrapper is wrapper for xkbd-bthid\n"\
              " it reconfigures the hci0 device to be usable as a keyboard\n"\
              " and also starts the sdpd service, to do this it needs to run with\n"\
              " root privileges. these will be dropped before executing xkbd-bthid\n"\
              " also the hci0 settings will be reverted to default values on exit\n"

int main(int argc, char **argv)
{
	int pid, status;
				
	if (geteuid() != 0) {
		fprintf(stderr, "%s: needs root privileges\n", argv[0]);
		fprintf(stderr, USAGE);
		exit(-1);
	}
	else {
		if ((pid = fork())) {
			waitpid(pid, &status, 0);
		}
		else {
			execl("/usr/sbin/hciconfig", "hciconfig", "hci0", "piscan");
		}
		if ((pid = fork())) {
			waitpid(pid, &status, 0);
		}
		else {
			execl("/usr/sbin/hciconfig", "hciconfig", "hci0", "class", "0x002540");
		}
		if ((pid = fork())) {
			waitpid(pid, &status, 0);
		}
		else {
			execl("/usr/sbin/sdpd", "sdpd");
		}

		if ((pid = fork())) {
			waitpid(pid, &status, 0);

			if ((pid = fork())) {
				waitpid(pid, &status, 0);
			}
			else {
				execl("/usr/sbin/hciconfig", "hciconfig", "hci0", "pscan");
			}
			if ((pid = fork())) {
				waitpid(pid, &status, 0);
			}
			else {
				execl("/usr/sbin/hciconfig", "hciconfig", "hci0", "class", "0x00100");
			}
		}
		else {
			setegid(getgid());
			seteuid(getuid());
			execl("/var/lib/install/usr/bin/xkbdbthid", "xkbdbthid", argv[1], argv[2]);
			exit(0);
		}
	}
}
