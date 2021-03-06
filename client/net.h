#pragma once

#define ACK_FLAG (1 << 0)

/*
 * A packet.
 */
typedef struct {
	unsigned char flags;
	unsigned short index;
	unsigned short key_length;
	char* key;
	unsigned short value_length;
	void* value;
} packet;

/*
 * Packet metadata.
 */
typedef struct {
	struct addrinfo* addr;
	int sockfd;
	int flags;
} packetmd;

/*
 * Remote hostname.
 */
extern char* HOSTNAME;

/*
 * Remote port.
 */
extern char* PORT;

/*
 * The packet metadata used in the current connection.
 */
extern packetmd METADATA;

/*
 * Get packets metadata.
 * If the hostname resolves only to one address (either IPv4 or IPv6), that
 * address is used, regardless of the use_ipv6 parameter.
 */
packetmd net_get_metadata(char* hostname, char* port, int use_ipv6);

/*
 * Initialize a packet passed as a pointer, which must not be NULL.
 * len is the value length in bytes.
 * Returns 1 on success, 0 on failure.
 * Remember to free the packet's content with net_free_packet() after use.
 */
int net_init_packet(packet* p, unsigned char flags, char* key, void* value, int len);

/*
 * Free a packet.
 */
void net_free_packet(packet* p);

/*
 * Send a packet.
 * Returns 1 on success, 0 on failure.
 */
int net_send_packet(packet* p);

/*
 * Receive a packet through the argument p.
 * The timeout argument is expressed in milliseconds and is considered only if
 * greater than 0.
 * If the received packet requires an ack, it is sent.
 * Returns 1 on success, 0 on failure.
 */
int net_receive_packet(packet* p, int timeout);

/*
 * Receive and check both the current packet's ack and the current packet's
 * expected response key, with an optional timeout (see net_receive_packet
 * function).
 */
int net_receive_and_check(char* packet_key, char* response_key, int timeout);

/*
 * Do the initial handshake.
 * Returns 1 on success, 0 on failure.
 */
int net_do_handshake();

/*
 * Do the update loop: send user inputs and receive updated window frames.
 * Returns 1 on success, 0 on failure.
 */
int net_update_loop();

/*
 * Close the connection.
 * Returns 1 on success, 0 on failure.
 */
int net_close();
