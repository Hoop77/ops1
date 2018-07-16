//
// Created by philipp on 11.07.18.
//

#ifndef CSPIELWIESE_DICT_H
#define CSPIELWIESE_DICT_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

typedef void (*dictKeyDestroyer_t)(void * key);
typedef bool (*dictKeyComparator_t)(const void * key1, const void * key2);

typedef struct
{
	void * key;
	void * val;
} dict_entry_t;

typedef struct dict_s
{
	dict_entry_t * data;
	unsigned int left;
	unsigned int bits;
	size_t keySize;
	dictKeyDestroyer_t destroyer;
	dictKeyComparator_t comparator;
} dict_t;

extern int
dictInit(dict_t * self, size_t keySize, dictKeyDestroyer_t destroyer, dictKeyComparator_t comparator);

extern void
dictRelease(dict_t * self);

extern void
dictInsert(dict_t * self, const void * key, void * val);

extern void *
dictFind(const dict_t * self, const void * key);

extern void
dictRemove(dict_t * self, const void * key);

extern void
grow(dict_t * self);

extern int
locate(const dict_t * self, const void * key, unsigned int * result);

extern uint64_t
hash_cont(const void * key, size_t size, uint64_t hval);

extern uint64_t
hash(const void * key, size_t size);

#endif //CSPIELWIESE_DICT_H
