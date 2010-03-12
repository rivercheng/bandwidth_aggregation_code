#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(void) {
    struct ifaddrs *list;
    struct ifaddrs *p;
    char ipstr[INET6_ADDRSTRLEN];
    if (getifaddrs(&list) < 0) {
        perror("getifaddrs");
        exit(1);
    }

    for (p=list; p!=NULL; p=p->ifa_next) {
        void *addr;
        printf("%s\t", p->ifa_name);
        printf("flag: %d\t", p->ifa_flags);
        printf("family: %d\t", p->ifa_addr->sa_family);
        if (p->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ifa_addr;
            addr = &(ipv4->sin_addr);
        } else if (p->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ifa_addr;
            addr = &(ipv6->sin6_addr);
        } else {
            printf("other address\n");
            continue;
        }
        inet_ntop(p->ifa_addr->sa_family, addr, ipstr, sizeof ipstr);
        printf("IP address: %s\n", ipstr);
    }
    freeifaddrs(list);
    return 0;
}
