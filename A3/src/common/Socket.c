//
// Created by philipp on 24.05.18.
//

#include "../../include/common/Socket.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

bool Socket_Connect(const char * host, Socket_Port port, Socket * sock)
{
    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = inet_addr(host)
    };

    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return false;

    if (connect(*sock, (struct sockaddr*) &address, sizeof(address)) < 0)
        return false;
}