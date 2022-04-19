#pragma once

/*
 * A packet.
 */
typedef struct {
	unsigned char flags;
	unsigned short n;
	char* key;
	unsigned short m;
	char* value;
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
 * The packet metadata used in the current connection.
 */
packetmd metadata;

/*
 * Get packets metadata.
 */
packetmd net_get_metadata(char* hostname, char* port, int use_ipv6);

/*
 * Create a new packet.
 */
packet net_create_packet(int need_ack, char* key, char* value);

/*
 * Send a packet.
 */
int net_send_packet(packet* p);

/*
 * Receive a packet.
 */
int net_receive_packet(packet* p);