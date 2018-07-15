//
// Created by philipp on 27.05.18.
//

#include "../../include/common/Pipe.h"
#include "../../include/common/Utils.h"
#include <unistd.h>

void Pipe_Init(Pipe self)
{
    if (pipe(self) < 0)
        terminate();
}

void Pipe_Destroy(Pipe self)
{
    if (self[0] != -1)
        close(self[0]);
    if (self[1] != -1)
        close(self[1]);
}

void Pipe_Copy(Pipe self, Pipe copy)
{
    copy[0] = self[0];
    copy[1] = self[1];
}

int Pipe_ReadDescriptor(Pipe self)
{
    return self[0];
}

int Pipe_WriteDescriptor(Pipe self)
{
    return self[1];
}

void Pipe_CloseReadDescriptor(Pipe self)
{
    close(self[0]);
    self[0] = -1;
}

void Pipe_CloseWriteDescriptor(Pipe self)
{
    close(self[1]);
    self[1] = -1;
}