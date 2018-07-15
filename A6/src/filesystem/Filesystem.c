#include "../../include/filesystem/Filesystem.h"

/*
typedef struct file_s {
    Vector *location;
    uint64_t  hashValue;
} file_t;

typedef struct fileSystem_s {
    dict_t dict2file;
    dict_t file2hash;
    dict_t hash2path;
} fileSystem_t;
*/

void vec2path(Vector *self, char* path)
{
    // TODO: convert Vector* to char*
}

void vec2dic(Vector *self, char* path)
{
    // TODO: convert Vector* to char*
}

void path2vec(Vector *self, char* path)
{
    // TODO: convert char* to Vector*
    // Function already defined
}

int fileSystemInit(fileSystem_t *self)
{
    if (dictInit(&self->dict2file)) { return 1; }
    if (dictInit(&self->file2hash)) { return 1; }
    if (dictInit(&self->hash2path)) { return 1; }

    fileSystemWriteDown(self);
    return 0;

}

int fileSystemRelease(fileSystem_t *self)
{
    // TODO: All vectors savely destroyed?
   
    dictRelease(&self->dict2file);
    dictRelease(&self->file2hash);
    dictRelease(&self->hash2path);

    return 0;
}

int fileSystemLookup(fileSystem_t *self, const file_t *file, dict_entry_t *entry)
{
    if (Vector_Size(file->location) > 0)
    {
        // Search for hashval with filepath and OVERWRITES hashval
        char* path = (char*) malloc( sizeof(char) );
        
        vec2path(file->location, path);
        int *pHashVal = (int*) malloc( sizeof(int) );
        *pHashVal = *( (int*) dictFind(&self->file2hash, path) );

        if (pHashVal == NULL) { return 1; }
        
        entry->key = path;
        entry->val = pHashVal;
        return 0;
    }
    if (file->hashValue != -1)
    {
        // Search for filepath with hashval and OVERWRITES path
        char *hashValueBuffer = (char*) malloc(22 * sizeof(char));
        sprintf(hashValueBuffer, "%i", (int) file->hashValue);

        Vector *pathVector = (Vector*) dictFind(&self->hash2path, hashValueBuffer);
        if (pathVector != NULL) { return 1; }

        entry->key = hashValueBuffer;
        entry->val = pathVector;
        
        return 0;
    }
    return -1;
}

int fileSystemInsert(fileSystem_t *self, const file_t *file)
{
    int result;
    unsigned int errno = 1;
    dict_entry_t d2fEntry;
    dict_entry_t f2hEntry;
    dict_entry_t h2pEntry;

    if (Vector_Size(file->location) == 0)
    {
        fprintf(stderr, "Can't insert an empty file!\n");
        return 2;
    }

    char* filepath = (char*) malloc( sizeof(char) );
    vec2path(file->location, filepath);
    int* intResult = (int*) dictFind(&self->file2hash, filepath);

    if (intResult !=  NULL)
    {
        fprintf(stderr, "File already exists!\n");
        return 2;
    }

    f2hEntry.key = filepath;
    f2hEntry.val = (int*) malloc (sizeof(int));
    *( (int*) f2hEntry.val ) = file->hashValue;
    dictInsert(&self->file2hash, f2hEntry.key, f2hEntry.val);

    char *hashValueBuffer = (char*) malloc(22 * sizeof(char));
    sprintf(hashValueBuffer, "%i", (int) file->hashValue);
    Vector* hashResult = (Vector*) dictFind(&self->hash2path, hashValueBuffer);

    h2pEntry.key = hashValueBuffer;
    h2pEntry.val = hashResult;

    if (hashResult == NULL)
    {
        // Kein Eintrag vorhanden
        h2pEntry.val = (Vector*) malloc(sizeof(Vector));
        Vector_Init(h2pEntry.val, sizeof(Vector), NULL);
        errno = 0;
    }

    // TODO: Copy Vector
    Vector_Append(h2pEntry.val, file->location);
    dictInsert(&self->hash2path, h2pEntry.key, h2pEntry.val);

    char* dictpath = (char*) malloc( sizeof(char) );
    vec2dic(file->location, dictpath);
    Vector* dictResult = (Vector*) dictFind(&self->dict2file, dictpath);

    d2fEntry.key = dictpath;
    d2fEntry.val = dictResult;

    if (dictResult == NULL)
    {
        // Kein Eintrag vorhanden
        d2fEntry.val = (Vector*) malloc(sizeof(Vector));
        Vector_Init(d2fEntry.val, sizeof(Vector), NULL);
    }

    // TODO: Copy Vector
    Vector_Append(d2fEntry.val, file->location);
    dictInsert(&self->dict2file, d2fEntry.key, d2fEntry.val);

    fileSystemWriteDown(self);
    return errno;
}

int fileSystemDelete(fileSystem_t *self, const file_t *file)
{
    bool isRealFile = false;

    if (Vector_Size(file->location) == 0)
    {
        return 0;
    }

    char* filepath = (char*) malloc( sizeof(char) );
    char* dictpath = (char*) malloc( sizeof(char) );
    char* hashBuff = (char*) malloc( sizeof(char) * 22);
    vec2path(file->location, filepath);
    vec2dic( file->location, dictpath);


    int* intResult = (int*) dictFind(&self->file2hash, filepath);

    if (intResult ==  NULL)
    {
        free(filepath);
        free(dictpath);
        free(hashBuff);
        return -1;
    }

    dictRemove(&self->file2hash, filepath);

    sprintf(hashBuff, "%i", *intResult);
    Vector* hashVec = (Vector*) dictFind(&self->hash2path, hashBuff);

    if (Vector_Size(hashVec) == 1)
    {
        // TODO: Delete File at "filepath"
        // Vector_Destroy(Vector_At(hashVec, 0));
        Vector_RemoveAndDestroy(hashVec, 0);
        dictRemove(&self->hash2path, hashBuff);
    }
    else
    {
        // TODO: Compare
        // isRealFile = hashVec[0] == filepath
        
        if (isRealFile)
        {
            // TODO: Move real file to hashVec[1]
            // Vector_Destroy(Vector_at(hashVec, 0));
            Vector_Remove(hashVec, 0);
        }
        else
        {
        /* TODO:
         * size_t index = Vector_Find(hashVec, file->location);
         * Vector_Destroy(Vector_at(hashVec, index));
         * Vector_Remove(hashVec, index);
         */
        }
    }

    Vector* dictVec = (Vector*) dictFind(&self->dict2file, dictpath);

    if (Vector_Size(dictVec) == 1)
    {
        // TODO: Needs to Remove everything from the path vector the given position
        // Vector_Destroy(Vector_At(dictVec, 0));
        Vector_RemoveAndDestroy(dictVec, 0);
        dictRemove(&self->dict2file, dictpath);
    }
    else
    {
        /* TODO:
         * size_t index = Vector_Find(dictVec, file->location);
         * Vector_Destroy(Vector_At(dictVec, index));
         * Vector_Remove(dictVec, index);
         */
    }

    free(filepath);
    free(dictpath);
    free(hashBuff);
    
    fileSystemWriteDown(self);
    return 0;
}

void fileSystemReadWriteDown(fileSystem_t *self)
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

    file_t newFile;
    Vector location;
    Vector_Init(&location, sizeof(Vector), NULL);
    char* dict = "dict";
    char* name = "test";
    Vector_Append(&location, dict);
    Vector_Append(&location, name);

    newFile.location = &location;
    newFile.hashValue = 1;

    fileSystemInsert(&fs, &newFile);
    fileSystemDelete(&fs, &newFile);

    fileSystemRelease(&fs);
    printf("--  End  --\n");
    return 0;
}
