//
// Created by philipp on 28.05.18.
//

#include <stdbool.h>
#include "../../include/common/Utils.h"

size_t GetFileSize(FILE * f)
{
    fseek(f, 0L, SEEK_END);
    size_t size = (size_t) ftell(f);
    rewind(f);
    return size;
}

bool Write(int fd, const char * buf, size_t len)
{
    size_t numBytesWritten = 0;
    ssize_t result;
    do
    {
        result = write(fd, buf, len - numBytesWritten);
        if (result <= 0)
            return false;
        numBytesWritten += (size_t) result;
    } while (numBytesWritten < len);
    return true;
}

bool Read(int fd, char * buf, size_t len)
{
    size_t numBytesRead = 0;
    ssize_t result;
    do
    {
        result = read(fd, buf, len - numBytesRead);
        if (result <= 0)
            return false;
        numBytesRead += (size_t) result;
    } while (numBytesRead < len);
    return true;
}

bool FileRead(FILE * file, const char * buf, size_t len)
{
    return fread((void *) buf, sizeof(char), len, file) == len;
}

bool FileWrite(FILE * file, const char * buf, size_t len)
{
    return fwrite (buf, sizeof(char), len, file) == len;
}