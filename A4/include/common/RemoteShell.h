//
// Created by philipp on 28.05.18.
//

#ifndef REMOTESHELL_REMOTESHELL_H
#define REMOTESHELL_REMOTESHELL_H

#include "Shell.h"
#include "Socket.h"

typedef struct
{
    Shell base;
    Socket fileTransferSock;
} RemoteShell;

void RemoteShell_Init(RemoteShell * self, Socket fileTransferSock);

void RemoteShell_Destroy(RemoteShell * self);

#endif //REMOTESHELL_REMOTESHELL_H
