#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "dict.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct location_s{
    char *directoryPath;
    char *name;
    size_t pathlength;
    size_t namelength;
} location_t;

typedef struct file_s {
    location_t location;
    uint64_t  hashValue;
} file_t;

typedef struct fileSystem_s {
    dict_t *dict2file;
    dict_t *file2hash;
    dict_t *hash2path;
} fileSystem_t;

extern void loc2path(location_t *self, char* path);

extern void path2loc(location_t *self, char* path);

extern int fileSystemInit(fileSystem_t *self);

extern int fileSystemRelease(fileSystem_t *self);

extern int fileSystemLookup(fileSystem_t *self, file_t *file);

extern void fileSystemInsert(fileSystem_t *self, file_t *file);

extern void fileSystemDelete(fileSystem_t *self, file_t *file);

#endif
