#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>

#include "net.h"

static struct addrinfo*
get_addrinfo(char* hostname, char* port, int use_ipv6)
{
	struct addrinfo hints;
	struct addrinfo* res;
	struct addrinfo* tmp;
	int status;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((status = getaddrinfo(hostname, port, &hints, res)) != 0) {
		print_err("call to getaddrinfo did not return 0\n");
		return NULL;
	}

	/* choose a result based on whether we want ipv4 or ipv6 */
	for (tmp = res; tmp != NULL; tmp = tmp->ai_next) {
		if (tmp->ai_family == AF_INET && !use_ipv6 ||
		tmp ->ai_family == AF_INET6 && use_ipv6) {
			return tmp;
		}
	}

	/* no result was chosen */
	print_err("no suitable address found\n");
	return NULL;
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

	sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

	if (sockfd < 0) {
		print_err("call to socket returned a negative number\n");
		memset(&pmd, 0, sizeof(pmd));
		return pmd;
	}

	pmd.addr = ai;
	pmd.sockfd = sockfd;
	pmd.flags = 0x0;

	return pmd;
}

packet
net_create_packet(int need_ack, char* key, char* value)
{
	packet p;
	p.flags = need_ack;
	p.n = strlen(key);
	p.key = key;
	p.m = strlen(value);
	p.value = value;
	return p;
}

int
net_send_packet(packet* p)
{
	/*TODO*/
}

int
net_receive_packet(packet* p)
{
	/*TODO*/
}