#include "filesystem.h"

/*
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
*/

extern void loc2path(location_t *self, char* path)
{

}

extern void path2loc(location_t *self, char* path)
{

}

int fileSystemInit(fileSystem_t *self)
{
    self->dict2file = (dict_t*) malloc(sizeof(dict_t));
    if (!self->dict2file) { goto err0; }
    self->file2hash = (dict_t*) malloc(sizeof(dict_t));
    if (!self->file2hash) { goto err1; }
    self->hash2path = (dict_t*) malloc(sizeof(dict_t));
    if (!self->hash2path) { goto err2; }

    if (dictInit(self->dict2file)) { goto err3; }
    if (dictInit(self->file2hash)) { goto err3; }
    if (dictInit(self->hash2path)) { goto err3; }

    return 0;

err3: free(self->hash2path);
err2: free(self->file2hash);
err1: free(self->dict2file);
err0: fprintf(stderr, "No memory available!\n");
      exit(-1);
}

int fileSystemRelease(fileSystem_t *self)
{

    free(self->hash2path);
    free(self->file2hash);
    free(self->dict2file);
    return 0;
}

int fileSystemLookup(fileSystem_t *self, file_t *file)
{
    if (file->location.directoryPath != NULL && file->location.name != NULL)
    {
        // Search for hashval with filepath and OVERWRITES hashval
        char* path = (char*) malloc( (file->location.namelength + file->location.pathlength) * sizeof(char) +1);

        loc2path(&file->location, path);
        int *pHashVal = (int*) dictFind(self->file2hash, path);

        free(path);

        if (pHashVal == NULL) { return 1; }

        file->hashValue = *pHashVal;
        return 0;
    }
    if (file->hashValue != -1)
    {
        // Search for filepath with hashval and OVERWRITES path
        char tempHashValueBuffer [22];
        sprintf(tempHashValueBuffer, "%i", (int) file->hashValue);

        Vector *pathVector = (Vector*) dictFind(self->hash2path, tempHashValueBuffer);
        if (pathVector != NULL) { return 1; }

        path2loc(&file->location, Vector_At(pathVector, 0));
        return 0;
    }
    return -1;
}

void fileSystemInsert(fileSystem_t *self, file_t *file)
{
    location_t newEntry;
    newEntry.pathlength = file->location.pathlength;
    newEntry.namelength = file->location.namelength;
    new
    int result = fileSystemLookup(self, file);
    if (result == -1) { fprintf(stderr, "Can't insert an empty file!\n"); }
}

void fileSystemDelete(fileSystem_t *self, file_t *file)
{

}

void fileSystemWriteDown(fileSystem_t *self)
{

}

int main(int argc, char** argv)
{
    printf("-- Begin --\n");
    fileSystem_t fs;
    fileSystemInit(&fs);
    fileSystemRelease(&fs);
    printf("--  End  --\n");
    return 0;
}
