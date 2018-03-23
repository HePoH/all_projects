#include "../include/core.h"

int main(int argc, char **argv) {
	int res = 0, opt = 0, pck_num_lim = -1;
	char *net_dev = NULL, *filter_exp = NULL, *search_str = NULL, err_buf[PCAP_ERRBUF_SIZE];
	pcap_t* pcap_hndl;
	struct bpf_program filter_prog;
	bpf_u_int32 mask;
	bpf_u_int32 net;

    /*net_dev = pcap_lookupdev(err_buf);
    if (net_dev == NULL) {
        fprintf(stderr, "Couldn't find default device: %s\n", err_buf);
        return(2);
    }

    res = pcap_lookupnet(net_dev, &net, &mask, err_buf);
	if(res == -1) {
		fprintf(stderr, "pcap_lookupnet() failed: couldn't get network address and mask of the device: %s\n", err_buf);
		exit(EXIT_FAILURE);
	}*/	

    while((opt = getopt(argc, argv, "hli:f:s:c:")) != -1) {
        switch (opt) {
            case 'h':
                print_usage_help();
                print_filter_help();
                exit(EXIT_SUCCESS);
            case 'l':
                list_net_devs();
                exit(EXIT_SUCCESS);
            case 'i':
                net_dev = malloc(strlen(optarg) * sizeof(char));
                if (net_dev == NULL) {
                    perror("malloc() failed");
                    exit(EXIT_FAILURE);
                }

                strncpy(net_dev, optarg, strlen(optarg));
                res = pcap_lookupnet(net_dev, &net, &mask, err_buf);
                if(res == -1) {
                    printf("pcap_lookupnet() failed: couldn't find network device: %s\n", err_buf);
                    exit(EXIT_FAILURE);
                }

                pcap_hndl = pcap_open_live(net_dev, MAX_SNAP_LEN, 1, 100, err_buf);
                if (pcap_hndl == NULL) {
                    fprintf(stderr, "pcap_open_live() failed: couldn't open device %s: %s\n", net_dev, err_buf);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'f':
                filter_exp = malloc(strlen(optarg) * sizeof(char));
                if (filter_exp == NULL) {
                    perror("malloc() failed");
                    exit(EXIT_FAILURE);
                }

                strncpy(filter_exp, optarg, strlen(optarg));

                res = pcap_compile(pcap_hndl, &filter_prog, filter_exp, 0, net);
                if (res == -1) {
                    printf("pcap_compile() failed: couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(pcap_hndl));
                    exit(EXIT_FAILURE);
                }

                res = pcap_setfilter(pcap_hndl, &filter_prog);
                if (res == -1) {
                    printf("pcap_setfilter() failed: couldn't install filter %s: %s\n", filter_exp, pcap_geterr(pcap_hndl));
                    exit(EXIT_FAILURE);
                }
                break;
            case 's':
                search_str = malloc(strlen(optarg) * sizeof(char));
                if (search_str == NULL) {
                    perror("malloc() failed");
                    exit(EXIT_FAILURE);
                }

                strncpy(search_str, optarg, strlen(optarg));
                break;
            case 'c':
                pck_num_lim = strtol(optarg, NULL, 10);
                if (errno == EINVAL || errno == ERANGE)
                    pck_num_lim = 0;

                break;
        }
    }	

    /* sudo ./sniffer -if ens33 -f "tcp port 80" -s "body" -l 10 */
    printf("Sniffer settings\n");
    printf("   |-Device              : %s\n", net_dev);
	printf("   |-Filter expression   : %s\n", (filter_exp ? filter_exp : "empty"));
	printf("   |-Search string       : %s\n", (search_str ? search_str : "empty"));
    printf("   |-Packet number limit : "); (pck_num_lim >= 1) ? printf("%d\n", pck_num_lim) : printf("unlimited\n");
	printf("Pcap based sniffer start...\n");

	res = pcap_loop(pcap_hndl, pck_num_lim, pckt_hndl, search_str);
	printf("Capture complet\nepcap_loop returned: %d\n", res);

    free(net_dev);
    free(filter_exp);

	pcap_freecode(&filter_prog);
	pcap_close(pcap_hndl);

	exit(EXIT_SUCCESS);
}
