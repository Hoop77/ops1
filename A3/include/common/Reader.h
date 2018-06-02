//
// Created by philipp on 28.05.18.
//

#ifndef REMOTESHELL_READER_H
#define REMOTESHELL_READER_H

#include <unistd.h>

typedef struct
{
    char * buffer;
    size_t bufferSize;
    size_t numBufferedBytes;
} Reader;

void Reader_Init(Reader * self, size_t bufferSize);

void Reader_Destroy(Reader * self);

bool Reader_Read(Reader * self, int fd, char * buf, size_t len);

bool Reader_ReadUntil(Reader * self, int fd, char ** buf, size_t * len, char delimiter);

#endif //REMOTESHELL_READER_H
