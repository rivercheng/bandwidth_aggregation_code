#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
//#include <arpa/inet.h>

#define SERVER_PORT "3490"
#define BACKLOG 10

int main() {
    int status;
    int s; //the socket
    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    int new_fd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, SERVER_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    s = socket(servinfo->ai_family, servinfo->ai_socktype, \
            servinfo->ai_protocol);
    if (s == -1) {
        perror("socket");
    }

    if (bind(s, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        perror("bind");
    }

    if (listen(s, BACKLOG) == -1) {
        perror("listen");
    }

    addr_size = sizeof their_addr;
    new_fd = accept(s, (struct sockaddr *)&their_addr, &addr_size);

    freeaddrinfo(servinfo);
    return(0);
}
