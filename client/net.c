#include "net.h"

packet
net_create_packet(int need_ack, char* key, char* value)
{
	packet p;
	p.ack = need_ack;
	p.key = key;
	p.n = 
	p.value = value;
	return p;
}

int
net_send_packet(packet p, int sockfd)
{
	/*TODO*/
}

int
net_receive_packet(packet* p, int sockfd)
{
	/*TODO*/
}