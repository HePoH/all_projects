#include "../include/client_core.h"

int main() {
	int sd_cln = 0, header_size = 0, rtn = 0;
	unsigned short iphlen = 0, udpplen = 0;
	struct ethhdr* eth = NULL;
	struct iphdr*  iph = NULL;
	struct udphdr* udph = NULL;

	struct sockaddr_ll srv_addr;

	struct ifreq if_idx;
	struct ifreq if_mac;

	char dgram[MAX_DGRAM_SIZE], *data = NULL;
	socklen_t socket_len = 0;
	ssize_t bts = 0;

	socket_len = sizeof(struct sockaddr_ll);

	memset(&srv_addr, 0, socket_len);
	memset(&if_idx, 0, sizeof(struct ifreq));
	memset(&if_mac, 0, sizeof(struct ifreq));
	memset(dgram, 0, MAX_DGRAM_SIZE);

	eth = (struct ethhdr *)dgram;
	iph = (struct iphdr*)(dgram + sizeof(struct ethhdr));
	udph = (struct udphdr*)(dgram + sizeof(struct iphdr) + sizeof(struct ethhdr));
	header_size = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr);
	data = dgram + header_size;

	sd_cln = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sd_cln == -1) {
		perror("Client: socket(client)");

		close(sd_cln);
		exit(EXIT_FAILURE);
	}

	strncpy(if_idx.ifr_name, DEFAULT_IF, IFNAMSIZ-1);
	rtn = ioctl(sd_cln, SIOCGIFINDEX, &if_idx);
	if (rtn == -1) {
		perror("SIOCGIFINDEX");
		exit(EXIT_FAILURE);
	}

	strncpy(if_mac.ifr_name, DEFAULT_IF, IFNAMSIZ-1);
	rtn = ioctl(sd_cln, SIOCGIFHWADDR, &if_mac);
	if (rtn == -1) {
		perror("SIOCGIFHWADDR");
		exit(EXIT_FAILURE);
	}

	srv_addr.sll_family = AF_PACKET;
	srv_addr.sll_ifindex = if_idx.ifr_ifindex;
	srv_addr.sll_halen = ETH_ALEN;
	srv_addr.sll_addr[0] = DEST_MAC0;
	srv_addr.sll_addr[1] = DEST_MAC1;
	srv_addr.sll_addr[2] = DEST_MAC2;
	srv_addr.sll_addr[3] = DEST_MAC3;
	srv_addr.sll_addr[4] = DEST_MAC4;
	srv_addr.sll_addr[5] = DEST_MAC5;

	strncpy(data, "Hello Dmitry!", MAX_MSG_SIZE);
	udpplen = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(data);

	eth->h_source[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
	eth->h_source[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
	eth->h_source[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
	eth->h_source[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
	eth->h_source[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
	eth->h_source[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];

	eth->h_dest[0] = DEST_MAC0;
	eth->h_dest[1] = DEST_MAC1;
	eth->h_dest[2] = DEST_MAC2;
	eth->h_dest[3] = DEST_MAC3;
	eth->h_dest[4] = DEST_MAC4;
	eth->h_dest[5] = DEST_MAC5;
	eth->h_proto = htons(ETH_P_IP);

	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	iph->tot_len = htons(udpplen);
	iph->id = htonl(54321);
	iph->frag_off = 0;
	iph->ttl = 255;
	iph->protocol = IPPROTO_UDP;
	iph->check = 0;
	iph->saddr = inet_addr(SOURCE_IP);
	iph->daddr = inet_addr(DEST_IP);;

	udph->source = htons(SOURCE_PORT);
	udph->dest = htons(DEST_PORT);
	udph->len = htons(sizeof(struct udphdr) + strlen(data));
	udph->check = 0;

	print_udp_packet((u_char*)dgram, sizeof(dgram));

	bts = sendto(sd_cln, dgram, udpplen, 0, (struct sockaddr*)&srv_addr, socket_len);
	if (bts == -1) {
		perror("Client: sendto(server)");

		close(sd_cln);
		exit(EXIT_FAILURE);
	}

	printf("Client: send message to server: '%s' (%d bytes)\n", data, (int)bts);

	while(1) {
		memset(dgram, 0, MAX_DGRAM_SIZE);

		bts = recvfrom(sd_cln, dgram, MAX_MSG_SIZE, 0, NULL, NULL);
		if (bts == -1) {
			perror("Client: recvfrom(server)");

			close(sd_cln);
			exit(EXIT_FAILURE);
		}

		eth = (struct ethhdr *)dgram;
		iph = (struct iphdr*)(dgram + sizeof(struct ethhdr));
		iphlen = iph->ihl*4;
		udph = (struct udphdr*)(dgram + iphlen + sizeof(struct ethhdr));
		header_size = sizeof(struct ethhdr) + iphlen + sizeof(struct udphdr);
		data = dgram + header_size;

		if (udph->dest == htons(DEST_PORT)) {
			if (iph->protocol == 17)
				print_udp_packet((u_char*)dgram, sizeof(dgram));
			else
				continue;

		printf("\n\nClient: receive message from server: '%s' (%d bytes)\n\n", (dgram + header_size), (int)bts);

			/*break;*/
		}
	}

	exit(EXIT_SUCCESS);
}

