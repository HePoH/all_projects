#include "../include/core.h"

int main() {
	int rtn = 0;
	char *net_dev = NULL, err_buf[PCAP_ERRBUF_SIZE];
	pcap_t* s_hndl;
	struct bpf_program filter_prog;
	char filter_exp[] = "port 23";
	bpf_u_int32 mask;
	bpf_u_int32 net;

	list_net_devs();

	net_dev = pcap_lookupdev(err_buf);
	if (net_dev == NULL) {
		fprintf(stderr, "pcap_lookupdev failed: couldn't find default device: %s\n", err_buf);
		exit(EXIT_FAILURE);
	}

	printf("\nNetwork device: %s\n", net_dev);

	rtn = pcap_lookupnet(net_dev, &net, &mask, err_buf);
	if(rtn == -1) {
		fprintf(stderr, "pcap_lookupnet failed: couldn't get network address and mask of the device: %s\n", err_buf);
		exit(EXIT_FAILURE);
	}

	s_hndl = pcap_open_live(net_dev, MAX_SNAP_LEN, 1, 100, err_buf);
	if (s_hndl == NULL) {
		fprintf(stderr, "pcap_open_live failed: couldn't open device %s: %s\n", net_dev, err_buf);
		exit(EXIT_FAILURE);
	}

	/*rtn = pcap_compile(s_hndl, &filter_prog, filter_exp, 0, net);
	if (rtn == -1) {
		fprintf(stderr, "pcap_compile failed: couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
		exit(EXIT_FAILURE);
	}

	rtn = pcap_setfilter(s_hndl, &filter_prog);
	if (rtn == -1) {
		fprintf(stderr, "pcap_setfilter failed: couldn't install filter %s: %s\n", filter_exp, pcap_geterr(s_hndl));
		exit(EXIT_FAILURE);
	}*/

	printf("Pcap sniffer start\n");
	printf("Listening %s, filter: %s...\n", net_dev, filter_exp);

	rtn = pcap_loop(s_hndl, -1, pckt_hndl, NULL);
	printf("pcap_loop returned: %d\nCapture complete", rtn);

	pcap_freecode(&filter_prog);
	pcap_close(s_hndl);

	exit(EXIT_SUCCESS);
}
