//
// Created by philipp on 24.05.18.
//

#ifndef REMOTE_SHELL_VECTOR_H
#define REMOTE_SHELL_VECTOR_H

#include <unistd.h>
#include <stdbool.h>

typedef void * Vector_Item;
typedef size_t Vector_Iterator;
typedef void (* Vector_ItemDestroyer)(Vector_Item);

typedef struct
{
    size_t size;
    size_t capacity;
    size_t itemSize;
    Vector_Item * items;
    Vector_ItemDestroyer destroyer;
} Vector;

#define Vector_ForeachBegin(self, item, iterator) \
        Vector_Iterator iterator; \
        for (iterator = 0; i < Vector_Size(self); ++iterator) \
        { item = Vector_At(self, iterator);

#define Vector_ForeachEnd }

#define Vector_Item2Int(item) (* (int *) item)
#define Vector_Item2Char(item) (* (char *) item)

void Vector_Init(Vector * self, size_t itemSize, Vector_ItemDestroyer destroyer);

void Vector_InitCharVector(Vector * self);

void Vector_InitIntVector(Vector * self);

void Vector_Destroy(Vector * self);

void Vector_Copy(Vector * self, Vector * copy);

void Vector_Append(Vector * self, Vector_Item item);

Vector_Item Vector_At(Vector * self, size_t index);

size_t Vector_Size(Vector * self);

bool Vector_IsEmpty(Vector * self);

#endif //REMOTE_SHELL_VECTOR_H
