//
// Created by philipp on 24.05.18.
//

#ifndef REMOTE_SHELL_ACCEPTOR_H
#define REMOTE_SHELL_ACCEPTOR_H

#include <stdbool.h>
#include "Socket.h"

typedef struct
{
    Socket socket;
    const char * bindAddress;
    Socket_Port port;
} Acceptor;

void Acceptor_Init(Acceptor * self, const char * bindAddress, Socket_Port port);

bool Acceptor_Open(Acceptor * self);

bool Acceptor_Accept(Acceptor * self, Socket * socket);

void Acceptor_Close(Acceptor * self);

#endif //REMOTE_SHELL_ACCEPTOR_H
