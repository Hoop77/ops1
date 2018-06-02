//
// Created by philipp on 24.05.18.
//

#ifndef REMOTE_SHELL_VECTOR_H
#define REMOTE_SHELL_VECTOR_H

#include <unistd.h>
#include <stdbool.h>

typedef void * VectorItem;
typedef size_t VectorIterator;
typedef void (* VectorItemDestroyer)(VectorItem);
typedef bool (* VectorItemComparator)(VectorItem, VectorItem);

typedef struct
{
    size_t size;
    size_t capacity;
    size_t itemSize;
    VectorItem * items;
    VectorItemDestroyer destroyer;
} Vector;

#define Vector_ForeachBegin(self, item, iterator) \
        VectorIterator iterator; \
        for (iterator = 0; i < Vector_Size(self); ++iterator) \
        { item = Vector_At(self, iterator);

#define Vector_ForeachEnd }

#define VectorItem_ToInt(item) (* (int *) item)
#define VectorItem_ToChar(item) (* (char *) item)

void Vector_Init(Vector * self, size_t itemSize, VectorItemDestroyer destroyer);

void Vector_InitCharVector(Vector * self);

void Vector_InitIntVector(Vector * self);

void Vector_Destroy(Vector * self);

void Vector_Copy(Vector * self, Vector * copy);

void Vector_Append(Vector * self, VectorItem item);

void Vector_Remove(Vector * self, size_t index);

void Vector_RemoveAndDestroy(Vector * self, size_t index);

void Vector_Insert(Vector * self, VectorItem item, size_t index);

VectorItem Vector_At(Vector * self, size_t index);

size_t Vector_Size(Vector * self);

bool Vector_IsEmpty(Vector * self);

bool Vector_Contains(Vector * self, VectorItem searchItem, VectorItemComparator comparator);

#endif //REMOTE_SHELL_VECTOR_H
