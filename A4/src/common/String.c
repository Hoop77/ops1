//
// Created by philipp on 25.05.18.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include "../../include/common/String.h"

const char NULL_CHAR = '\0';

static void StringItemDestroyer(VectorItem string)
{
    String_Destroy(string);
}

void String_Init(String * self)
{
    Vector_InitCharVector(self);
    Vector_Append(self, (VectorItem) &NULL_CHAR);
}

void String_InitFromCharArray(String * self, const char * str)
{
    Vector_InitCharVector(self);
    size_t len = strlen(str);
    for (size_t i = 0; i < len; ++i)
        Vector_Append(self, (VectorItem) &str[i]);
    Vector_Append(self, (VectorItem) &NULL_CHAR);
}

void String_Destroy(String * self)
{
    Vector_Destroy(self);
}

void String_Copy(String * self, String * copy)
{
    Vector_Copy(self, copy);
}

size_t String_Size(String * self)
{
    return Vector_Size(self) - 1;
}

bool String_IsEmpty(String * self)
{
    return Vector_Size(self) <= 1;
}

char String_CharAt(String * self, size_t index)
{
    return VectorItem_ToChar(Vector_At(self, index));
}

const char * String_CharArray(String * self)
{
    return (const char *) self->items;
}

void String_Append(String * self, String * other)
{
    // TODO: Improve performance!
    for (size_t i = 0; i < String_Size(other); ++i)
        String_AppendChar(self, String_CharAt(other, i));
}

void String_AppendChar(String * self, char c)
{
    char * end = Vector_At(self, String_Size(self));
    *end = c;
    Vector_Append(self, (VectorItem) &NULL_CHAR);
}

void String_AppendCharArray(String * self, const char * chars)
{
    // TODO: Improve performance!
    for (size_t i = 0; i < strlen(chars); ++i)
        String_AppendChar(self, chars[i]);
}

void String_Split(String * self, char delimiter, Vector * split)
{
    Vector_Init(split, sizeof(String), StringItemDestroyer);
    String str;
    String_InitFromCharArray(&str, "");
    for (size_t i = 0; i < String_Size(self); ++i)
    {
        char c = String_CharAt(self, i);
        if (c == delimiter)
        {
            Vector_Append(split, (VectorItem) &str);
            String_InitFromCharArray(&str, "");
        }
        else
        {
            String_AppendChar(&str, c);
        }
    }
    Vector_Append(split, (VectorItem) &str);
}

bool String_Equals(String * self, String * other)
{
    return strcmp(String_CharArray(self), String_CharArray(other)) == 0;
}

bool String_EqualsCharArray(String * self, const char * other)
{
    return strcmp(String_CharArray(self), other) == 0;
}

bool String_ToInt(String * self, int * value)
{
    // TODO: All C standard implementations of this functionality seem to SUCK! Implement your own!
    if (String_Size(self) == 1 && String_CharAt(self, 0) == '0')
        return true;

    char * end;
    long l = strtol(String_CharArray(self), &end, 10);
    if (l == 0 || l < INT_MIN || l > INT_MAX || errno == ERANGE)
        return false;

    *value = (int) l;
    return true;
}