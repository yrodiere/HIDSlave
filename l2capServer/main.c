/* Code from http://people.csail.mit.edu/albert/bluez-intro/x559.html
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

int main(int argc, char **argv)
{
	struct sockaddr_l2 loc_addr =
	{ 0 }, rem_addr =
	{ 0 };
	char buf[1024] =
	{ 0 };
	int s, status;
	socklen_t opt = sizeof(rem_addr);

	// allocate socket
	//s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_L2CAP);
	//s = socket(AF_BLUETOOTH, SOCK_DGRAM, BTPROTO_L2CAP); // NOT WORKING ("File descriptor in bad state") -> Needs ioctl ?
	if (s < 0)
	{
		perror("Error while getting socket");
		return -1;
	}

	// bind socket to port 0x1001 of the first available
	// bluetooth adapter
	loc_addr.l2_family = AF_BLUETOOTH;
	loc_addr.l2_bdaddr = *BDADDR_ANY;
	loc_addr.l2_psm = htobs(0x1001);

	status = bind(s, (struct sockaddr *) &loc_addr, sizeof(loc_addr));
	if (status < 0)
	{
		perror("Error while binding socket");
		return -1;
	}

	// put socket into listening mode
	status = listen(s, 1);
	if (status < 0)
	{
		perror("Error while listening on socket");
		return -1;
	}

	// read data from the client
	while (1)
	{
		int client, nBytes;
		// accept one connection
		client = accept(s, (struct sockaddr *) &rem_addr, &opt);
		if (client < 0)
		{
			perror("Error while accepting on socket");
			return -1;
		}

		ba2str(&rem_addr.l2_bdaddr, buf);
		fprintf(stderr, "accepted connection from %s\n", buf);

		memset(buf, 0, sizeof(buf));

		nBytes = read(client, buf, sizeof(buf));

		if (nBytes >= 0)
		{
			printf("received [%s]\n", buf);
			strcat(buf, ", you said? Hello, slave!");
			nBytes = write(client, buf, strlen(buf));
			if (nBytes >= 0)
			{
				printf("answered [%s]\n", buf);
			}
			else
			{
				perror("Error while writing back");
			}
		}
		else
		{
			perror("Error while reading");
		}

		close(client);
	}

	// close connection
	close(s);

	return 0;
}
