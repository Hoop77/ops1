//
// Created by philipp on 24.05.18.
//

#include "../../include/common/Acceptor.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

bool Acceptor_Open(Acceptor * self, Socket_Port port)
{
    struct sockaddr_in address;
    Socket sock;

    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return false;

    if (bind(sock, (struct sockaddr*) &address, sizeof(struct sockaddr_in)) < 0)
        return false;

    if (listen(sock, 1) < 0)
        return false;

    *self = sock;
    return true;
}

bool Acceptor_Accept(Acceptor * self, Socket * socket)
{
    struct sockaddr_in address;
    socklen_t len;
    *socket = accept(*self, (struct sockaddr*) &address, &len);
    if (*socket < 0)
        return false;
    return true;
}

void Acceptor_Close(Acceptor * self)
{
    close(*self);
}