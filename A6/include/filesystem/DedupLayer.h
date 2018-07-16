#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "../common/Vector.h"
#include "../common/Dict.h"
#include "../common/Path.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	dict_t dir2entries;
	dict_t filePath2fileHash;
	dict_t fileHash2fileReferences;
} DedupLayer;

typedef uint64_t FileHash;

typedef int FileDescriptor;

void DedupLayer_Init(DedupLayer * self);

Vector * DedupLayer_GetDirectoryEntries(DedupLayer * self, String * dirPath);

String * DedupLayer_GetTrueFilePath(DedupLayer * self, String * origFilePath);

bool DedupLayer_Write(DedupLayer * self,
                      String * filePath,
                      FileDescriptor fd,
                      const char * buffer,
                      size_t size,
                      off_t offset);

bool DedupLayer_Unlink(DedupLayer * self, String * filePath);

bool DedupLayer_RemoveDirectory(DedupLayer * self, String * dirPath);

#endif
