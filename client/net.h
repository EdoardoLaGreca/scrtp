#pragma once

/*
 * A packet.
 */
typedef struct {
	unsigned char flags;
	unsigned short n;
	char* key;
	unsigned short m;
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
char* HOSTNAME;

/*
 * Remote port.
 */
char* PORT;

/*
 * The packet metadata used in the current connection.
 */
packetmd METADATA;

/*
 * Get packets metadata.
 */
packetmd net_get_metadata(char* hostname, char* port, int use_ipv6);

/*
 * Create a new packet.
 */
packet net_create_packet(int need_ack, char* key, void* value);

/*
 * Send a packet.
 */
int net_send_packet(packet* p);

/*
 * Receive a packet.
 */
int net_receive_packet(packet* p);

/*
 * Do the initial handshake.
 */
int net_do_handshake(packetmd* pmd);

/*
 * Close the connection.
 */
int net_close(packetmd* pmd);
