//
// Created by philipp on 24.05.18.
//

#include "../../include/common/Vector.h"
#include "../../include/common/Utils.h"
#include <string.h>

#define ITEM_ADDR(self, index) \
    ((Vector_Item) ((char *) self->items + (index * self->itemSize)))

#define SET(self, item, index) \
    memcpy(ITEM_ADDR(self, index), item, self->itemSize)

void Vector_Init(Vector * self, size_t itemSize, Vector_ItemDestroyer destroyer)
{
    memset(self, 0, sizeof(Vector));
    self->itemSize = itemSize;
    self->destroyer = destroyer;
}

void Vector_InitCharVector(Vector * self)
{
    Vector_Init(self, sizeof(char), NULL);
}

void Vector_InitIntVector(Vector * self)
{
    Vector_Init(self, sizeof(int), NULL);
}

void Vector_Destroy(Vector * self)
{
    Vector_Item item;
    Vector_ForeachBegin(self, item, i)
        if (self->destroyer)
            self->destroyer(item);
    Vector_ForeachEnd

    free(self->items);
}

void Vector_Copy(Vector * self, Vector * copy)
{
    memcpy(copy, self, sizeof(Vector));
    size_t len = copy->itemSize * copy->capacity;
    copy->items = malloc(len);
    if (!copy->items)
        terminate();
    memcpy(copy->items, self->items, len);
}

void Vector_Append(Vector * self, Vector_Item item)
{
    size_t insertIndex = self->size;
    if (self->size == 0)
    {
        self->items = malloc(self->itemSize);
        if (!self->items)
            terminate();
        self->capacity = 1;
    }

    if (self->size == self->capacity)
    {
        self->capacity *= 2;
        self->items = realloc(self->items, self->capacity * self->itemSize);
        if (!self->items)
            terminate();
    }

    SET(self, item, insertIndex);
    self->size++;
}

Vector_Item Vector_At(Vector * self, size_t index)
{
    return ITEM_ADDR(self, index);
}

size_t Vector_Size(Vector * self)
{
    return self->size;
}

bool Vector_IsEmpty(Vector * self)
{
    return self->size == 0;
}