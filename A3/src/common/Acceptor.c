//
// Created by philipp on 24.05.18.
//

#include "../../include/common/Acceptor.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void Acceptor_Init(Acceptor * self, const char * bindAddress, Socket_Port port)
{
    self->bindAddress = bindAddress;
    self->port = port;
}

bool Acceptor_Open(Acceptor * self)
{
    struct sockaddr_in address;
    Socket sock;

    address.sin_family = AF_INET;
    address.sin_port = htons(self->port);
    address.sin_addr.s_addr = INADDR_ANY;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return false;

    if (bind(sock, (struct sockaddr*) &address, sizeof(struct sockaddr_in)) < 0)
        return false;

    if (listen(sock, 1) < 0)
        return false;

    self->socket = sock;
    return true;
}

bool Acceptor_Accept(Acceptor * self, Socket * socket)
{
    struct sockaddr_in address;
    socklen_t len;
    *socket = accept(self->socket, (struct sockaddr*) &address, &len);
    if (*socket < 0)
        return false;
    return true;
}

void Acceptor_Close(Acceptor * self)
{
    close(self->socket);
}