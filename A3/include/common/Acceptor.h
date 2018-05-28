//
// Created by philipp on 24.05.18.
//

#ifndef REMOTE_SHELL_ACCEPTOR_H
#define REMOTE_SHELL_ACCEPTOR_H

#include <stdbool.h>
#include "Socket.h"

typedef Socket Acceptor;

bool Acceptor_Open(Acceptor * self, Socket_Port port);

bool Acceptor_Accept(Acceptor * self, Socket * socket);

void Acceptor_Close(Acceptor * self);

#endif //REMOTE_SHELL_ACCEPTOR_H
