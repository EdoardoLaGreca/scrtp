#define _POSIX_C_SOURCE 200112L

#include <sys/types.h>

#ifdef __linux__
#include <sys/socket.h>
#include <netdb.h>
#elif _WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "print.h"
#include "net.h"

#define ACK_FLAG (1 << 0)

char* HOSTNAME;
char* PORT;
packetmd METADATA;

static struct addrinfo*
get_addrinfo(char* hostname, char* port, int use_ipv6)
{
	struct addrinfo hints;
	struct addrinfo* res;
	struct addrinfo* tmp; /* used to go through the linked list */

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if (getaddrinfo(hostname, port, &hints, &res) != 0) {
		print_err("call to getaddrinfo did not return 0");
		return NULL;
	}

	if (res->ai_next == NULL) {
		/* only one result, no need to go through the list */
		return res;
	}

	/* choose a result based on whether we want ipv4 or ipv6 */
	for (tmp = res; tmp != NULL; tmp = tmp->ai_next) {
		if (use_ipv6 && tmp->ai_family == AF_INET6) {
			/* we want ipv6, and tmp points to an ipv6 address */
			break;
		} else if (!use_ipv6 && tmp->ai_family == AF_INET) {
			/* we want ipv4, and tmp points to an ipv4 address */
			break;
		}
	}

	/* free the rest of the results */
	for (; res != NULL; res = res->ai_next) {
		if (res != tmp) {
			freeaddrinfo(res);
		}
	}

	if (tmp == NULL) {
		/* no result was chosen */
		print_err("no suitable address found");
	}

	return tmp;
}

packetmd
net_get_metadata(char* hostname, char* port, int use_ipv6)
{
	struct addrinfo* ai;
	int sockfd;
	packetmd pmd;

	ai = get_addrinfo(hostname, port, use_ipv6);

	if (ai == NULL) {
		exit(EXIT_FAILURE);
	}

	memset(&pmd, 0, sizeof(pmd));

	if (ai->ai_family == AF_INET) {
		sockfd = socket(PF_INET, ai->ai_socktype, ai->ai_protocol);
	} else if (ai->ai_family == AF_INET6) {
		sockfd = socket(PF_INET6, ai->ai_socktype, ai->ai_protocol);
	} else {
		print_err("unknown address family");
		return pmd;
	}

	if (sockfd < 0) {
		print_err("call to socket returned a negative number");
		print_verb("next message is the socket error:");
		print_verb(strerror(errno));
		return pmd;
	}

	pmd.addr = ai;
	pmd.sockfd = sockfd;
	pmd.flags = 0x0;

	return pmd;
}

packet
net_create_packet(int need_ack, char* key, void* value, int len)
{
	packet p;
	p.flags = need_ack;
	p.n = strlen(key);
	p.key = key;
	p.m = len;
	p.value = value;
	return p;
}

int
net_send_packet(packet* p)
{
	char* buf;
	packetmd* md;
	int sentbytes;

	buf = malloc(sizeof(packet));
	if (buf == NULL) {
		print_err("call to malloc returned NULL");
		return 0;
	}

	/* serialize the packet */
	memcpy(buf, p, sizeof(packet));

	/* send the packet using data from METADATA */
	md = &METADATA;
	sentbytes = sendto(md->sockfd, buf, sizeof(packet), md->flags,
		md->addr->ai_addr, md->addr->ai_addrlen);

	free(buf);

	if (sentbytes < 0 || sentbytes != sizeof(packet)) {
		print_err("call to sendto returned a negative number or mismatched bytes");
		free(buf);
		return 0;
	}

	/* if packet has ack flag, wait for ack */
	if ((p->flags & ACK_FLAG) != 0) {
		net_acknowledge(p->key);
	}

	return 1;
}

int
net_receive_packet(packet* p)
{
	/*TODO*/
	return 42;
}

int
net_do_handshake(packet* p)
{
	/*TODO*/
	return 42;
}

int
net_acknowledge(char* key)
{
	/*TODO*/
	return 42;
}

int
net_close(packetmd* pmd)
{
	packet p;
	p = net_create_packet(1, "end", NULL, 0);

	p.value = malloc(sizeof(unsigned char));
	if (p.value == NULL) {
		print_err("call to malloc returned NULL");
		return 0;
	}

	/* boolean value */
	*(unsigned char*)p.value = 0x01;
	p.m = 1;

	net_send_packet(&p);

	free(p.value);

	return 1;
}
