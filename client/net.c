#define _POSIX_C_SOURCE 200112L

#include <sys/types.h>

#ifdef __unix__
	#include <sys/socket.h>
	#include <netdb.h>
	#include <unistd.h>
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

	if (PENDING_ACKS == NULL) {
		/* list is empty */
		PENDING_ACKS = ar;
		return;
	}

	tmp = PENDING_ACKS;

	/* go to last item in list */
	while (tmp->next != NULL) {
		tmp = tmp->next;
	}

	tmp->next = ar;
}

/* complete an ack after receiving the response */
/* if the key is not present in the list, it does nothing */
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
	net_init_packet(&ack, 0, "ack", key, strlen(key) + 1);
	net_send_packet(&ack);
	net_free_packet(&ack);
}

/* serialize a packet, return the bytes as a parameter and the length */
static int
serialize_packet(packet* p, unsigned char** serialized)
{
	int length = 0;
	int idx = 0; /* index */

	/* calculate length */
	length += sizeof(p->flags);
	length += sizeof(p->key_length);
	length += p->key_length;
	length += sizeof(p->value_length);
	length += p->value_length;

	*serialized = malloc(length);

	/* serialize the packet fields */
	memcpy((unsigned char*) *serialized + idx, &p->flags, sizeof(p->flags));
	idx += sizeof(p->flags);

	memcpy((unsigned char*) *serialized + idx, &p->key_length, sizeof(p->key_length));
	idx += sizeof(p->key_length);

	memcpy((unsigned char*) *serialized + idx, p->key, p->key_length);
	idx += p->key_length;

	memcpy((unsigned char*) *serialized + idx, &p->value_length, sizeof(p->value_length));
	idx += sizeof(p->value_length);

	memcpy((unsigned char*) *serialized + idx, p->value, p->value_length);
	idx += p->value_length;

	return length;
}

/* deserialize a packet */
static packet
deserialize_packet(unsigned char* serialized, int length)
{
	int idx = 0; /* index */
	packet p;

	p.flags = *(unsigned char*) (serialized + idx);
	idx += sizeof(unsigned char);

	p.key_length = *(unsigned short*) (serialized + idx);
	idx += sizeof(unsigned short);

	p.key = malloc(p.key_length);
	memcpy(p.key, serialized + idx, p.key_length);
	idx += p.key_length;

	p.value_length = *(unsigned short*) (serialized + idx);
	idx += sizeof(unsigned short);

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

/* send bytes */
/* return the number of bytes actually sent (return value of sendto function) */
static int
send_bytes(void* bytes_ptr, int length)
{
	return sendto(METADATA.sockfd, bytes_ptr, length, METADATA.flags,
		METADATA.addr->ai_addr, METADATA.addr->ai_addrlen);
}

/* receive bytes from the network and return them as a parameter */
/* return the number of bytes actually received (return value of recvfrom function) */
/* return -2 if the packet came from a wrong address */
static int
receive_bytes(void* bytes_ptr, int length)
{
	int ret_code;

	/* fill the buffer with a packet coming from the METADATA address */
	ret_code = recvfrom(METADATA.sockfd, bytes_ptr, length, METADATA.flags,
		METADATA.addr->ai_addr, &METADATA.addr->ai_addrlen);

	return ret_code;
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

int
net_init_packet(packet* p, unsigned char flags, char* key, void* value, int len)
{
	p->flags = flags;
	p->key_length = strlen(key) + 1;
	p->value_length = len;

	p->key = malloc(p->key_length);
	if (p->key == NULL) {
		return 0;
	}

	strcpy(p->key, key);

	p->value = malloc(p->value_length);
	if (p->value == NULL) {
		return 0;
	}

	/* the caller may just want to allocate space for the value and add a value
	 * later, so we don't want to copy the value if it's NULL */
	if (value != NULL) {
		memcpy(p->value, value, len);
	}
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
	unsigned char* serialized = NULL;
	int sentbytes, serialized_len;

	/* serialize the packet */
	serialized_len = serialize_packet(p, &serialized);

	/* send the packet */
	sentbytes = send_bytes(serialized, serialized_len);

	free(serialized);

	if (sentbytes < 0) {
		print_err("call to sendto returned a negative number");
	} else if (sentbytes != serialized_len) {
		print_err("call to sendto returned mismatched bytes");
		return 0;
	}

	/* if packet has ack flag, wait for ack */
	if (p->flags & ACK_FLAG) {
		queue_ack(p->key);
	}

	return 1;
}

int
net_receive_packet(packet* p)
{
	const unsigned int chunk_length = 1024;
	unsigned int recvbytes = 0, buffer_length = chunk_length;
	unsigned char* buffer = NULL;

	buffer = malloc(buffer_length);
	if (buffer == NULL) {
		print_err("call to malloc returned NULL");
		return 0;
	}

	/* receive the packet */
	do {
		recvbytes = receive_bytes(buffer + buffer_length - chunk_length,
			chunk_length);

		if (recvbytes == -1) {
			print_err("call to recvfrom returned -1");
			return 0;
		} else if (recvbytes == chunk_length) {
			/* add space for the next chunk of bytes */
			buffer_length += chunk_length;
			buffer = realloc(buffer, buffer_length);

			if (buffer == NULL) {
				print_err("call to realloc returned NULL");
				return 0;
			}
		} else {
			/* resize the buffer to fit the actual content */
			buffer_length = buffer_length - chunk_length + recvbytes;
			buffer = realloc(buffer, buffer_length);

			if (buffer == NULL) {
				print_err("call to realloc returned NULL");
				return 0;
			}
		}
	} while (recvbytes == chunk_length);


	/* deserialize the packet */
	*p = deserialize_packet(buffer, buffer_length);

	return 1;
}

int
net_do_handshake()
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
		/* add null terminator in case there isn't already */
		((char*) p->value)[p->value_length - 1] = '\0';

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
		net_init_packet(&newp, ACK_FLAG, "winid", &chosen_win, sizeof(int));
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
net_close()
{
	packet p;
	unsigned char value = 0x01;
	net_init_packet(&p, 1, "end", &value, 1);

	net_send_packet(&p);

	net_free_packet(&p);

	/* close the socket */
	close(METADATA.sockfd);
}
