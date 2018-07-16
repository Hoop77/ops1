//
// Created by philipp on 24.05.18.
//

#include "../../include/common/Vector.h"
#include "../../include/common/Utils.h"
#include <string.h>

#define ITEM_ADDR(self, index) \
    ((VectorItem) ((char *) self->items + ((index) * self->itemSize)))

#define COPY_TO_INDEX(self, item, index) \
    memcpy(ITEM_ADDR(self, index), item, self->itemSize)

#define REALLOC(self) \
    self->items = realloc(self->items, self->capacity * self->itemSize); \
    if (!self->items) \
        terminate();

void Vector_Init(Vector * self, size_t itemSize, VectorItemDestroyer destroyer)
{
    self->size = 0;
    self->itemSize = itemSize;
    self->destroyer = destroyer;
    self->items = malloc(self->itemSize);
    if (!self->items)
        terminate();
    self->capacity = 1;
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
    VectorItem item;
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

void Vector_Append(Vector * self, VectorItem item)
{
    size_t insertIndex = self->size;

    if (self->size == self->capacity)
    {
        self->capacity *= 2;
        REALLOC(self);
    }

    COPY_TO_INDEX(self, item, insertIndex);
    self->size++;
}

void Vector_Remove(Vector * self, size_t index)
{
    for (size_t i = index + 1; i < self->size; ++i)
        COPY_TO_INDEX(self, ITEM_ADDR(self, i), i - 1);

    self->size--;
    if (self->size < self->capacity / 2)
    {
        self->capacity /= 2;
        REALLOC(self);
    }
}

void Vector_RemoveAndDestroy(Vector * self, size_t index)
{
    if (self->destroyer)
        self->destroyer(ITEM_ADDR(self, index));

    Vector_Remove(self, index);
}

void Vector_Insert(Vector * self, VectorItem item, size_t index)
{
    self->size++;
    if (self->size > self->capacity)
    {
        self->capacity *= 2;
        REALLOC(self);
    }

    size_t i = self->size - 1;
    while (i > index)
    {
        i--;
        COPY_TO_INDEX(self, ITEM_ADDR(self, i), i + 1);
    }

    COPY_TO_INDEX(self, item, index);
}

VectorItem Vector_At(Vector * self, size_t index)
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

bool Vector_Contains(Vector * self, VectorItem searchItem, VectorItemComparator comparator)
{
    return Vector_Find(self, searchItem, comparator, NULL);
}

bool Vector_Find(Vector * self, VectorItem searchItem, VectorItemComparator comparator, VectorIterator * result)
{
	VectorItem currItem;
	Vector_ForeachBegin(self, currItem, i)
		if (comparator(currItem, searchItem))
		{
			if (result != NULL)
				*result = i;
			return true;
		}
	Vector_ForeachEnd;
	return false;
}