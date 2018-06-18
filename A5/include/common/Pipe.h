//
// Created by philipp on 27.05.18.
//

#ifndef REMOTESHELL_PIPE_H
#define REMOTESHELL_PIPE_H

#include <stdbool.h>

typedef int Pipe[2];

void Pipe_Init(Pipe self);

void Pipe_Destroy(Pipe self);

void Pipe_Copy(Pipe self, Pipe copy);

int Pipe_ReadDescriptor(Pipe self);

int Pipe_WriteDescriptor(Pipe self);

void Pipe_CloseReadDescriptor(Pipe self);

void Pipe_CloseWriteDescriptor(Pipe self);

#endif //REMOTESHELL_PIPE_H
