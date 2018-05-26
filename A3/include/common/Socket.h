//
// Created by philipp on 24.05.18.
//

#ifndef REMOTE_SHELL_NETWORK_H
#define REMOTE_SHELL_NETWORK_H

#include <stdint.h>
#include <stdbool.h>

typedef int Socket;
typedef uint16_t Socket_Port;

bool Socket_Connect(const char * host, Socket_Port port, Socket * sock);

#endif //REMOTE_SHELL_NETWORK_H
