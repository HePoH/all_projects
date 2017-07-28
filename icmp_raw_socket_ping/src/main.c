#include "../include/core.h"

int main() {
	int sd = 0, seq = 0, header_size = 0, on = 1, rtn = 0;
	pid_t pid = 0;
	unsigned short iphlen = 0, icmpplen = 0;
	struct sockaddr_in dest_addr;
	struct hostent* dest_hst = NULL;
	struct iphdr* iph = NULL;
	struct icmphdr* icmph = NULL;
	char packet[MAX_PACKET_SIZE], *data = NULL;
	socklen_t socket_len = 0;
	ssize_t bts = 0;

	pid = getpid();

	socket_len = sizeof(struct sockaddr_in);

	memset(&dest_addr, 0, socket_len);
	memset(packet, 0, MAX_PACKET_SIZE);

	iph = (struct iphdr*)packet;
	icmph = (struct icmphdr*)(packet + sizeof(struct iphdr));
	header_size = sizeof(struct iphdr) + sizeof(struct icmphdr);
	data = packet + header_size;
	icmpplen = sizeof(struct iphdr) + sizeof(struct icmphdr) + strlen(data);

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);

	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	iph->tot_len = htons(icmpplen);
	iph->id = htonl(54321);
	iph->frag_off = 0;
	iph->ttl = 255;
	iph->protocol = IPPROTO_ICMP;
	iph->check = 0;
	iph->saddr = inet_addr(SOURCE_IP);
	iph->daddr = inet_addr(DEST_IP);

	icmph->type = ICMP_ECHO;
	icmph->code = 0;
	icmph->checksum = 0;
	icmph->un.echo.id = pid;
	icmph->un.echo.sequence = seq;

	sd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sd == -1) {
		perror("socket");

		close(sd);
		exit(EXIT_FAILURE);
	}

	rtn = setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));
	if (rtn == -1) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	/*dest_hst = gethostbyaddr((char *)&dest_addr.sin_addr.s_addr, 4, AF_INET);
	printf("Client: icmp echo server %s:%d (hs: %s)\n", inet_ntoa(dest_addr.sin_addr), ntohs(dest_addr.sin_port),
	((dest_hst != NULL) ? dest_hst->h_name : ""));*/

	/*while(1) {*/
		bts = sendto(sd, packet, icmpplen, 0, (struct sockaddr*)&dest_addr, socket_len);
		if (bts == -1) {
			perror("sendto");

			close(sd);
			exit(EXIT_FAILURE);
		}

		printf("Send ICMP echo request to %s\n", DEST_IP);

		memset(packet, 0, MAX_PACKET_SIZE);

		bts = recvfrom(sd, packet, MAX_PACKET_SIZE, 0, NULL, NULL);
		if (bts == -1) {
			perror("recvfrom");

			close(sd);
			exit(EXIT_FAILURE);
		}

		iph = (struct iphdr*)packet;
		iphlen = iph->ihl*4;
		icmph = (struct icmphdr*)(packet + iphlen + sizeof(struct iphdr));
		header_size = iphlen + sizeof(icmph);

		print_icmp_packet((u_char*)packet, sizeof(packet));
		printf("\n\nReceive ICMP reply from %s \n", DEST_IP);
	/*}*/

	exit(EXIT_SUCCESS);
}
