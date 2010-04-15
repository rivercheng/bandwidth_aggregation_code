#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAXBUFLEN 2000

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char*argv[])
{
    struct addrinfo out_hints;
    struct addrinfo *p;
    struct addrinfo *outinfo;
    
    int out_sock;
    int status;

    struct sockaddr_storage their_addr;
    size_t addr_len;
    int    numbytes;
    char   buf[MAXBUFLEN];

    if (argc != 3) {
        fprintf(stderr, \
                "Usage: %s <output addr> <output port>\n", \
                argv[0]);
        exit(1);
    }


    memset(&out_hints, 0, sizeof out_hints);
    out_hints.ai_family = AF_UNSPEC;
    out_hints.ai_socktype = SOCK_DGRAM;

    if ((status = getaddrinfo(argv[1], argv[2], &out_hints, &outinfo))!=0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    for (p=outinfo; p != NULL; p = p->ai_next) {
        if ((out_sock = socket(p->ai_family, p->ai_socktype, \
                        p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "failed to bind socket\n");
        return 2;
    }

    addr_len = sizeof their_addr;
    int i = 0;
    int total = 0;
    struct timeval tv, tv2;
    gettimeofday(&tv, 0);
    for (i=0; i< 10000; i++) {
        numbytes = 1360;
        if ((numbytes = sendto(out_sock, buf, numbytes, 0, \
                    p->ai_addr, p->ai_addrlen)) == -1) {
            perror("sendto");
            exit(1);
        }
        total += numbytes;
    /*
    printf("got packet from %s\n", \
            inet_ntop(their_addr.ss_family, \
                get_in_addr((struct sockaddr *)&their_addr),
                s, sizeof s));
    printf("packet is %d bytes long\n", numbytes);
    buf[numbytes] = '\0';
    printf("packet contains \"%s\"\n", buf);
    */
    }
    gettimeofday(&tv2, 0);
    int s = tv2.tv_sec - tv.tv_sec;
    int us = tv2.tv_usec - tv.tv_usec;
    if (us < 0) {
        s--;
        us += 1000000;
    }
    double t = s + us / 1000000. ;
    printf("speed %lfKbps\n", total * 8 / t /1000);
    return 0;
}




        
