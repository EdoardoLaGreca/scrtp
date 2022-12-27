/*
Send raw IP packet. Requires superuser privileges.
Usage for IPv4: sendraw 4 [file]
Usage for IPv6: sendraw 6 [file]
If no file is provided, use stdin.
*/

#include <sys/socket.h>
#include <netdb.h>

/* maximum IP payload length */
#define MAX_PL_LEN 65535

/* build and return IP header */
void
ipheader()
{
 /*TODO*/
}

/* send the payload p of length plen, return 0 on success and 1 or exit on failure */
int
sendpkt(char* progname, int isipv4, void* p, int plen)
{
	int sockfd, ret;
	struct addrinfo hints, *res;
	char packet[MAX_PACKET_SIZE];

	/* set up hints for getaddrinfo */
	memset(&hints, 0, sizeof(hints));
	
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_RAW;

	if (isipv4) {
		hints.ai_family = AF_INET;
	} else {
		hints.ai_family = AF_INET6;
	}

	/* get address information for the destination */
	ret = getaddrinfo(DEST_IP_ADDRESS, NULL, &hints, &res);
	if (ret != 0) {
		fprintf(stderr, "%s: getaddrinfo: %s\n", progname, gai_strerror(ret));
		exit(EXIT_FAILURE);
	}

	/* set up IP packet */
	/* TODO */

	/* send packet */
	int bytes_sent = sendto(sockfd, packet, sizeof(packet), 0,
		res->ai_addr, res->ai_addrlen);

	if (bytes_sent < 0) {
		perror("sendto failed");
		exit(EXIT_FAILURE);
	}

	/* free address information */
	freeaddrinfo(res);
}

/* fill buffer with error handling, return 0 on success and 1 or exit on failure */
int
fillbuf(FILE* f, char* buf, int buflen)
{
	if (fgets(buf, buflen, f) == NULL) {
		if (ferror(f)) {
			fprintf(stderr, "%s: ferror set\n", argv[0]);
			exit(EXIT_FAILURE);
		} else if (feof(f)) {
			fprintf(stderr, "%s: eof reached\n", argv[0]);
		}
		return 1;
	}

	return 0
}

int
main(int argc, char** argv)
{
	int isipv4;
	FILE* f;
	void payload[MAX_PL_LEN + 1];
	char* res;

	if (argc < 2) {
		fprintf(stderr, "%s: not enough arguments\n", argv[0]);
		exit(EXIT_FAILURE);
	} else if (argc > 3) {
		fprintf(stderr, "%s: too many arguments\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	switch (argv[1][0]) {
	case '4':
		isipv4 = 1;
		break;
	case '6':
		isipv4= 0;
		break;
	default:
		fprintf(stderr, "%s: unknown second argument\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (argc == 3) {
		f = fopen(argv[2], "r");
		if (f == NULL) {
			fprintf(stderr, "%s: unable to open file %s\n", argv[0], argv[2]);
			exit(EXIT_FAILURE);
		}
	} else {
		f = stdin;
	}

	while (1) {
		/* fill payload */
		if (fillbuf(f, payload, MAX_PL_LEN + 1)) {
			break;
		}

		sendpkt(argv[0], isipv4, payload, MAX_PL_LEN);
	}

	return 0;
}
