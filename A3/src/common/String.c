//
// Created by philipp on 25.05.18.
//

#include <string.h>
#include "../../include/common/String.h"

const char NULL_CHAR = '\0';

static void StringItemDestroyer(Vector_Item string)
{
    String_Destroy(string);
}

void String_Init(String * self, const char * str)
{
    Vector_InitCharVector(self);
    size_t len = strlen(str);
    for (size_t i = 0; i < len; ++i)
        Vector_Append(self, (Vector_Item) &str[i]);
    Vector_Append(self, (Vector_Item) &NULL_CHAR);
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
    return Vector_Item2Char(Vector_At(self, index));
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
    Vector_Append(self, (Vector_Item) &NULL_CHAR);
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
    String_Init(&str, "");
    for (size_t i = 0; i < String_Size(self); ++i)
    {
        char c = String_CharAt(self, i);
        if (c == delimiter)
        {
            Vector_Append(split, (Vector_Item) &str);
            String_Init(&str, "");
        }
        else
        {
            String_AppendChar(&str, c);
        }
    }
    Vector_Append(split, (Vector_Item) &str);
}

bool String_Equals(String * self, String * other)
{
    return strcmp(String_CharArray(self), String_CharArray(other)) == 0;
}