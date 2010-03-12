#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
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
    int in_port = 0;
    int out_port = 0;
    struct addrinfo self_hints;
    struct addrinfo out_hints;
    struct addrinfo *selfinfo;
    struct addrinfo *p;
    struct addrinfo *outinfo;
    
    int sockfd;
    int out_sock;
    int status;

    struct sockaddr_storage their_addr;
    size_t addr_len;
    int    numbytes;
    char   buf[MAXBUFLEN];
    char    s[INET6_ADDRSTRLEN];

    if (argc != 4) {
        fprintf(stderr, \
                "Usage: %s <listening port> <output addr> <output port>\n", \
                argv[0]);
        exit(1);
    }

    memset(&self_hints, 0, sizeof self_hints);
    self_hints.ai_family = AF_UNSPEC;
    self_hints.ai_socktype = SOCK_DGRAM;
    self_hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, argv[1], &self_hints, &selfinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    for (p = selfinfo; p != NULL; p = p->ai_next) {
        int yes = 1;
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) \
                == -1) {
            perror("socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("bind");
            continue;
        }

        printf("listen at %s:%d\n", inet_ntop(p->ai_family,get_in_addr(p->ai_addr) \
                    ,s, sizeof s), ntohs(((struct sockaddr_in *)(p->ai_addr))->sin_port));
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "failed to bind socket \n");
        exit(2);
    }

    freeaddrinfo(selfinfo);

    memset(&out_hints, 0, sizeof out_hints);
    out_hints.ai_family = AF_UNSPEC;
    out_hints.ai_socktype = SOCK_DGRAM;

    if ((status = getaddrinfo(argv[2], argv[3], &out_hints, &outinfo))!=0) {
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
    while(1) {
    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1, 0,\
                    (struct sockaddr *)&their_addr, &addr_len)) \
            == -1) {
        perror("recvfrom");
        exit(1);
    }

    if ((numbytes = sendto(out_sock, buf, numbytes, 0, \
                    p->ai_addr, p->ai_addrlen)) == -1) {
        perror("sendto");
        exit(1);
    }

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

    close(sockfd);
    return 0;
}




        
