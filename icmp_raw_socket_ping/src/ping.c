#include "../include/core.h"

/* https://cboard.cprogramming.com/networking-device-communication/107801-linux-raw-socket-programming.html */

int main(int argc, char **argv) {
    uint32_t sock_opt_on = 1, res = 0;
    uint8_t ipv4_addr[INET_ADDRSTRLEN];
    pthread_t icmp_reqs_hndl_tid = 0, icmp_repl_hndl_tid = 0;
    ping_sets_t ping_sets;
    uid_t user_id = 0;

    user_id = getuid();
    if (user_id != 0) {
        printf("%s: root privelidges needed\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&ping_sets, 0, sizeof(ping_sets_t));

    res = parse_argvs(argc, argv, &ping_sets);
    if (res == -1) {
        printf("parse_argvs() failed\n");
        exit(EXIT_FAILURE);
    }

    ping_sets.reqs_id = getpid();
    init_signal();
    
    ping_sets.diff_time = malloc(ping_sets.reqs_count * sizeof(diff_time_t));
    if (ping_sets.diff_time == NULL) {
        perror("malloc() failed");
        exit(EXIT_FAILURE);
    }

    ping_sets.sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (ping_sets.sock == -1) {
        perror("socket() failed");

        close(ping_sets.sock);
        exit(EXIT_FAILURE);
    }

    res = setsockopt(ping_sets.sock, IPPROTO_IP, IP_HDRINCL, &sock_opt_on, sizeof(sock_opt_on));
    if (res == -1) {
        perror("setsockopt() failed");

        close(ping_sets.sock);
        exit(EXIT_FAILURE);
    }

    printf("%s configuration\n", argv[0]);
	printf("   |- Socket descriptor    : #%d\n", ping_sets.sock);
	printf("   |- Request ID (pid)     : #%d\n", ping_sets.reqs_id);
    inet_ntop(AF_INET, (const void*)&(ping_sets.dst_addr.sin_addr), (char*)ipv4_addr, INET_ADDRSTRLEN);
	printf("   |- Destination ip       : %s\n", ipv4_addr);
    inet_ntop(AF_INET, (const void*)&(ping_sets.src_addr.sin_addr), (char*)ipv4_addr, INET_ADDRSTRLEN);
	printf("   |- Source ip            : %s\n", ipv4_addr);
	printf("   |- Request packet count : %d\n", ping_sets.reqs_count);
	printf("   |- Request timeout      : %d (seconds)\n", ping_sets.reqs_timeout);
	printf("   |- Debug option         : %s\n", ping_sets.debug_opt == 1 ? "enable" : "disable");
    
    res = pthread_create(&icmp_reqs_hndl_tid, NULL, icmp_reqs_hndl, (void*)&ping_sets);
    if(res != 0) {
        printf("pthread_create() failed");

        close(ping_sets.sock);
        exit(EXIT_FAILURE);
    }

    res = pthread_create(&icmp_repl_hndl_tid, NULL, icmp_repl_hndl, (void*)&ping_sets);
    if(res != 0) {
        printf("pthread_create() failed");

        close(ping_sets.sock);
        exit(EXIT_FAILURE);
    }

    /*pthread_join(icmp_reqs_hndl_tid, NULL);*/
    pthread_join(icmp_repl_hndl_tid, NULL);

    free(ping_sets.diff_time);
    close(ping_sets.sock);
    exit(EXIT_SUCCESS);
}
