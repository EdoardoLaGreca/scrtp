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
 * Create a new packet.
 * need_ack states whether the packet needs an acknowledgement.
 * key is the key.
 * value is the value.
 * len is the value length in bytes.
 * Remember to free the packet's content with net_free_packet() after use.
 */
packet net_create_packet(int need_ack, char* key, void* value, int len);

/*
 * Send a packet.
 * Returns 1 on success, 0 on failure.
 */
int net_send_packet(packet* p);

/*
 * Receive a packet.
 * Returns 1 on success, 0 on failure.
 */
int net_receive_packet(packet* p);

/*
 * Do the initial handshake.
 * Returns 1 on success, 0 on failure.
 */
int net_do_handshake(packet* p);

/*
 * Route incoming packets.
 */
void net_route_packets();

/*
 * Close the connection.
 * Returns 1 on success, 0 on failure.
 */
int net_close(packetmd* pmd);
