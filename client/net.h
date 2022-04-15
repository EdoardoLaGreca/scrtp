#pragma once

/*
 * A packet.
 */
typedef struct {
	unsigned char ack;
	unsigned short n;
	char* key;
	unsigned short m;
	char* value;
} packet;

/*
 * Create a new packet.
 */
packet net_create_packet(int need_ack, char* key, char* value);

/*
 * Send a packet using using a given socket.
 */
int net_send_packet(packet p, int sockfd);

/*
 * Receive a packet using using a given socket.
 */
int net_receive_packet(packet* p, int sockfd);