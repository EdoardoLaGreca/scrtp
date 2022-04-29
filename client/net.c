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
#include <stdio.h>

#include "window.h"
#include "const.h"
#include "print.h"
#include "net.h"

#define ACK_FLAG (1 << 0)

/* internal structure for pending ack requests, as a list item */
typedef struct ack_request_s {
	char* key; /* key of the packet */
	struct ack_request_s* next;
} ack_request;

static ack_request* PENDING_ACKS = NULL;
char* HOSTNAME = NULL;
char* PORT = NULL;
packetmd METADATA = {NULL, -1, 0};
char* REMOTE_PUBKEY = NULL;

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

/* queue an ack after sending a request */
static void
queue_ack(char* key)
{
	ack_request* ar;
	ack_request* tmp;

	ar = malloc(sizeof(ack_request));
	ar->key = malloc(strlen(key) + 1);
	strcpy(ar->key, key);
	ar->next = NULL;

	tmp = PENDING_ACKS;

	/* go to last item in list */
	while (tmp->next != NULL) {
		tmp = tmp->next;
	}

	tmp->next = ar;
}

/* complete an ack after receiving the response */
static void
complete_ack(char* key)
{
	ack_request* tmp;
	ack_request* prev = NULL;
	tmp = PENDING_ACKS;

	while (tmp != NULL) {
		if (strcmp(tmp->key, key) == 0) {
			/* remove this item from the list */

			if (prev != NULL) {
				prev->next = tmp->next;
			} else {
				PENDING_ACKS = tmp->next;
			}

			free(tmp->key);
			free(tmp);

			return;
		}

		prev = tmp;
		tmp = tmp->next;
	}
}

/* reply to an ack request */
static void
reply_ack(char* key)
{
	packet ack;
	ack = net_create_packet(0, "ack", key, strlen(key) + 1);
	net_send_packet(&ack);
	net_free_packet(&ack);
}

/* serialize a packet, return the bytes as a parameter and the length */
static int
serialize_packet(packet* p, unsigned char* serialized)
{
	int length = 0;
	int idx = 0; /* index */

	/* calculate length */
	length += sizeof(p->flags);
	length += sizeof(p->key_length);
	length += p->key_length;
	length += sizeof(p->value_length);
	length += p->value_length;

	serialized = malloc(length);

	/* serialize the packet fields */
	memcpy((unsigned char*) serialized + idx, &p->flags, sizeof(p->flags));
	idx += sizeof(p->flags);

	memcpy((unsigned char*) serialized + idx, &p->key_length, sizeof(p->key_length));
	idx += sizeof(p->key_length);

	memcpy((unsigned char*) serialized + idx, &p->key, p->key_length);
	idx += p->key_length;

	memcpy((unsigned char*) serialized + idx, &p->value_length, sizeof(p->value_length));
	idx += sizeof(p->value_length);

	memcpy((unsigned char*) serialized + idx, &p->value, p->value_length);
	idx += p->value_length;

	return length;
}

/* deserialize a packet, return the packet as a parameter*/
static packet
deserialize_packet(unsigned char* serialized, int length)
{
	int idx = 0; /* index */
	packet p;

	p.flags = *(unsigned int*) (serialized + idx);
	idx += sizeof(p.flags);

	p.key_length = *(unsigned int*) (serialized + idx);
	idx += sizeof(p.key_length);

	p.key = malloc(p.key_length);
	memcpy(p.key, serialized + idx, p.key_length);
	idx += p.key_length;

	p.value_length = *(unsigned int*) (serialized + idx);
	idx += sizeof(p.value_length);

	p.value = malloc(p.value_length);
	memcpy(p.value, serialized + idx, p.value_length);
	idx += p.value_length;

	return p;
}

static int
choose_window(char* windows)
{
	int win_num, input = -1;

	printf("choose a window by its index:\n");
	win_num = print_windows(windows);

	while (1) {
		int values_read = 0;
		char c;

		printf(">>");

		values_read = scanf("%d", &input);

		while((c = getchar()) != '\n');

		if (values_read == 1 && input >= 0 && input <= win_num) {
			return input;
		}

		printf("invalid input, try again\n");
	}
}

/* receive bytes and return them as a parameter */
/* return the number of bytes actually received */
static int
receive_bytes(void* bytes_ptr, int num_bytes)
{
	return recvfrom(METADATA.sockfd, &bytes_ptr, num_bytes, METADATA.flags,
		METADATA.addr->ai_addr, &METADATA.addr->ai_addrlen);
}

/* encrypt the packet and return its length */
/* bytes is the pointer to the heap-allocated data */
static int
encrypt_packet(packet* p, unsigned char* bytes)
{
	/*TODO*/
	return 42;
}

/* decrypt the packet */
/* length is the length of the data pointed to by bytes */
static void
decrypt_packet(unsigned char* bytes, int length, packet* p)
{
	/*TODO*/
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

	/* create a socket based on the address family (IPv4 vs IPv6) */
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
net_create_packet(int flags, char* key, void* value, int len)
{
	packet p;
	p.flags = flags;
	p.key_length = strlen(key) + 1;
	p.value_length = len;

	p.key = malloc(p.key_length);
	strcpy(p.key, key);

	p.value = malloc(p.value_length);

	/* the caller may just want to allocate space for the value and add a value
	 * later, so we don't want to copy the value if it's NULL */
	if (value != NULL) {
		memcpy(p.value, value, len);
	}

	return p;
}

void
net_free_packet(packet* p)
{
	if (p->key != NULL) {
		free(p->key);
	}

	if (p->value != NULL) {
		free(p->value);
	}
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
		queue_ack(p->key);
	}

	return 1;
}

int
net_receive_packet(packet* p)
{
	unsigned char flags;
	unsigned int key_length = 0, value_length = 0;
	char* key = NULL;
	unsigned char* value = NULL;
	unsigned int recvbytes, i;
	void* buffer = NULL;

	/* sizes of the packet fields */
	int sizes[5] = { sizeof(flags), sizeof(key_length), sizeof(key),
		sizeof(value_length), sizeof(value) };

	/* pointers to the packet fields */
	void* ptrs[5];
	ptrs[0] = &flags;
	ptrs[1] = &key_length;
	ptrs[2] = &key;
	ptrs[3] = &value_length;
	ptrs[4] = &value;

	for (i = 0; i < 5; i++) {
		/* receive */
		recvbytes = receive_bytes(ptrs[i], sizes[i]);

		/* check for errors */
		if (recvbytes < 0) {
			print_err("call to recvfrom returned a negative number:");
			print_err(strerror(errno));

			if (key != NULL) {
				free(key);
			}

			if (value != NULL) {
				free(value);
			}

			return 0;
		}

		if (key_length > 0 && key == NULL) {
			/* alloc key */
			key = calloc(key_length, sizeof(char));
		}

		if (value_length > 0 && value == NULL) {
			/* alloc value */
			value = calloc(value_length, sizeof(unsigned char));
		}
	}

	/* receive the rest of the packet, but discard it */
	buffer = malloc(1024);
	while (receive_bytes(&buffer, 1024) == 1024);

	p->flags = flags;
	p->key_length = key_length;
	p->key = key;
	p->value_length = value_length;
	p->value = value;

	return 1;
}

int
net_do_handshake(packet* p)
{
	/*TODO*/
	return 42;
}

void
net_route_packet(packet* p)
{
	/* if the other side requested an ack, reply immediately */
	if (p->flags & ACK_FLAG) {
		reply_ack(p->key);
	}

	/* route the packet to the appropriate handler or handle directly */
	/* keys that should not be received are not handled */
	if (strcmp(p->key, "error") == 0) {

		print_err(p->value);

	} else if (strcmp(p->key, "ack") == 0) {

		complete_ack(p->value);

	} else if (strcmp(p->key, "pubkey") == 0) {

		if (REMOTE_PUBKEY == NULL) {
			REMOTE_PUBKEY = malloc(p->value_length);
			memcpy(REMOTE_PUBKEY, p->value, p->value_length);
		} else {
			print_err("received a remote pubkey but already got one");
		}

	} else if (strcmp(p->key, "wins") == 0) {

		packet newp;
		int chosen_win;
		chosen_win = choose_window(p->value);
		newp = net_create_packet(ACK_FLAG, "winid", &chosen_win, sizeof(int));
		net_send_packet(&newp);

	} else if (strcmp(p->key, "winsize") == 0) {

		unsigned short winsize[2];
		memcpy(winsize, p->value, sizeof(winsize));
		WINDOW_WIDTH = winsize[0];
		WINDOW_HEIGHT = winsize[1];

	} else if (strcmp(p->key, "compfr") == 0) {

		/*TODO*/

	} else if (strcmp(p->key, "end") == 0) {

		print_verb("connection closed.");

	} else {

		print_err("received an unexpected key:");
		print_err(p->key);

	}
}

void
net_close(packetmd* pmd)
{
	packet p;
	unsigned char value = 0x01;
	p = net_create_packet(1, "end", NULL, 1);

	/* add boolean value */
	memcpy(p.value, &value, 1);

	net_send_packet(&p);

	net_free_packet(&p);
}
