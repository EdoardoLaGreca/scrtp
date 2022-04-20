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
 */
packetmd net_get_metadata(char* hostname, char* port, int use_ipv6);

/*
 * Create a new packet.
 * need_ack states whether the packet needs an acknowledgement.
 * key is the key.
 * value is the value.
 * len is the value length.
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
 * Wait for packet acknowledgement (blocking).
 * This function waits for packet acknowledgement.
 * If non-acknowledged packets arrive in the meantime, [TODO]
 */
int net_acknowledge(char* key);

/*
 * Close the connection.
 * Returns 1 on success, 0 on failure.
 */
int net_close(packetmd* pmd);
