//
// Created by philipp on 11.07.18.
//

#ifndef CSPIELWIESE_DICT_H
#define CSPIELWIESE_DICT_H

#include <stdint.h>

typedef struct {
	char* key;
	void* val;
} dict_entry_t;

typedef struct dict_s {
	dict_entry_t* data;
	unsigned int left;
	unsigned int bits;
} dict_t;

extern int
dictInit(dict_t* self);

extern void
dictRelease(dict_t* self);

extern void
dictInsert(dict_t* self, const char* key, void* val);

extern void*
dictFind(const dict_t* self, const char* key);

extern void
dictRemove(dict_t* self, const char* key);

extern uint64_t
hash_cont(const void* key, size_t size, uint64_t hval);

extern uint64_t
hash(const void* key, size_t size);

#endif //CSPIELWIESE_DICT_H
