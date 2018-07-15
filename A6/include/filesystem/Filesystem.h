#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "../common/Vector.h"
#include "../common/Dict.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct file_s {
    Vector  *location;
    uint64_t hashValue;
} file_t;

typedef struct fileSystem_s {
    dict_t dict2file;
    dict_t file2hash;
    dict_t hash2path;
} fileSystem_t;

extern void vec2path(Vector *self, char* path);

extern void vec2dic(Vector *self, char* path);

extern void path2vec(Vector *self, char* path);

extern int fileSystemInit(fileSystem_t *self);

extern int fileSystemRelease(fileSystem_t *self);

extern int fileSystemLookup(fileSystem_t *self, const file_t *file, dict_entry_t *entry);

extern int fileSystemInsert(fileSystem_t *self, const file_t *file);

extern int fileSystemDelete(fileSystem_t *self, const file_t *file);

extern void fileSystemReadWriteDown(fileSystem_t *self);

extern void fileSystemWriteDown(fileSystem_t *self);

#endif
