//
// Created by philipp on 25.05.18.
//

#ifndef REMOTESHELL_STRING_H
#define REMOTESHELL_STRING_H

#include "Vector.h"

typedef Vector String;

void String_Init(String * self);

void String_InitFromCharArray(String * self, const char * str);

void String_Destroy(String * self);

void String_Copy(String * self, String * copy);

size_t String_Size(String * self);

bool String_IsEmpty(String * self);

char String_CharAt(String * self, size_t index);

const char * String_CharArray(String * self);

void String_Append(String * self, String * other);

void String_AppendChar(String * self, char c);

void String_AppendCharArray(String * self, const char * chars);

void String_Split(String * self, char delimiter, Vector * split);

bool String_Equals(String * self, String * other);

#endif //REMOTESHELL_STRING_H
