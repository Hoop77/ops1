//
// Created by philipp on 28.05.18.
//

#include <stdbool.h>
#include <string.h>
#include "../../include/common/Reader.h"
#include "../../include/common/Utils.h"

static void Release(Reader * self, char * buf, size_t len);

static size_t FindDelimiter(const char * buffer, size_t len, char delimiter);

static bool TryFind(Reader * self, char ** buf, size_t *len, char delimiter);

static void Grow(char ** buf, size_t currSize, size_t growBytes);

void Reader_Init(Reader * self, size_t bufferSize)
{
    self->buffer = malloc(bufferSize);
    if (!self->buffer)
        terminate();
    self->bufferSize = bufferSize;
    self->numBufferedBytes = 0;
}

void Reader_Destroy(Reader * self)
{
    free(self->buffer);
}

bool Reader_Read(Reader * self, int fd, char * buf, size_t len)
{
    len -= min(len, self->numBufferedBytes);
    Release(self, buf, len);
    if (len == 0)
        return true;
    return Read(fd, buf, len);
}

bool Reader_ReadUntil(Reader * self, int fd, char ** buf, size_t * len, char delimiter)
{
    *len = 0;

    if (TryFind(self, buf, len, delimiter))
        return true;

    for (;;)
    {
        ssize_t result = read(fd, self->buffer, self->bufferSize);
        if (result <= 0)
        {
            if (*len > 0)
                free(*buf);
            return false;
        }
        self->numBufferedBytes += (size_t) result;

        if (TryFind(self, buf, len, delimiter))
            return true;
    }
}

static void Release(Reader * self, char * buf, size_t len)
{
    memcpy(buf, self->buffer, len);
    memcpy(self->buffer, self->buffer + len, self->bufferSize - len);
    self->numBufferedBytes -= len;
}

static size_t FindDelimiter(const char * buffer, size_t len, char delimiter)
{
    for (size_t i = 0; i < len; ++i)
    {
        if (buffer[i] == delimiter)
            return i + 1;
    }
    return 0;
}

static bool TryFind(Reader * self, char ** buf, size_t *len, char delimiter)
{
    size_t size = FindDelimiter(self->buffer, self->numBufferedBytes, delimiter);
    if (size > 0)
    {
        Grow(buf, *len, size);
        Release(self, *buf + *len, size);
        *len += size;
        (*buf)[*len - 1] = '\0';
        return true;
    }

    size = self->numBufferedBytes;
    Grow(buf, *len, size);
    Release(self, *buf + *len, size);
    *len += size;
    return false;
}

static void Grow(char ** buf, size_t currSize, size_t growBytes)
{
    if (currSize == 0)
    {
        *buf = malloc(growBytes);
        if (!*buf)
            terminate();
        return;
    }

    *buf = realloc(buf, currSize + growBytes);
    if (!*buf)
        terminate();
}