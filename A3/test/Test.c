//
// Created by philipp on 24.05.18.
//

#include "../include/common/Vector.h"
#include "TestFramework.h"
#include "../include/common/String.h"
#include "../include/common/Path.h"

TEST_INIT;

void Vector_Test_Append()
{
    TEST_BEGIN("Vector_Test_Append");

    Vector vector;
    Vector_InitCharVector(&vector);
    Vector_Append(&vector, (Vector_Item) "a");
    Vector_Append(&vector, (Vector_Item) "b");
    Vector_Append(&vector, (Vector_Item) "c");

    TEST_EQ(Vector_Size(&vector), 3);

    char * item;
    char items[4];
    items[3] = '\0';
    Vector_ForeachBegin(&vector, item, i)
        items[i] = *item;
    Vector_ForeachEnd

    TEST_STREQ(items, "abc");

    Vector_Destroy(&vector);

    TEST_END;
}

void String_Test_AppendChar()
{
    TEST_BEGIN("String_Test_AppendChar");

    String s;
    String_Init(&s, "Hello Worl");
    String_AppendChar(&s, 'd');
    TEST_STREQ(String_CharArray(&s), "Hello World");
    String_Destroy(&s);

    TEST_END;
}

void String_Test_Split_Path()
{
    TEST_BEGIN("String_Test_Split_Path");

    String s;
    String_Init(&s, "/home/philipp");
    Vector split;
    String_Split(&s, '/', &split);
    TEST_EQ(Vector_Size(&split), 3);
    const char * item;
    item = String_CharArray(Vector_At(&split, 0));
    TEST_STREQ(item, "");
    item = String_CharArray(Vector_At(&split, 1));
    TEST_STREQ(item, "home");
    item = String_CharArray(Vector_At(&split, 2));
    TEST_STREQ(item, "philipp");
    String_Destroy(&s);
    Vector_Destroy(&split);

    TEST_END;
}

void String_Test_Split_NoDelimiter()
{
    TEST_BEGIN("String_Test_Split_Path");

    String s;
    String_Init(&s, "abc");
    Vector split;
    String_Split(&s, ' ', &split);
    TEST_EQ(Vector_Size(&split), 1);
    TEST_STREQ(String_CharArray(Vector_At(&split, 0)), "abc");
    String_Destroy(&s);
    Vector_Destroy(&split);

    TEST_END;
}

void Path_Test_InitRoot()
{
    TEST_BEGIN("Path_Test_InitRoot");

    String strPath;
    String_Init(&strPath, "/");
    Path path;
    TEST_TRUE(Path_FromString(&path, &strPath));
    TEST_EQ(Path_Depth(&path), 0);
    Path_Destroy(&path);
    String_Destroy(&strPath);

    TEST_END;
}

void Path_Test_InitNoPath()
{
    TEST_BEGIN("Path_Test_InitNoPath");

    String strPath;
    String_Init(&strPath, "abc");
    Path path;
    TEST_FALSE(Path_FromString(&path, &strPath));
    Path_Destroy(&path);
    String_Destroy(&strPath);

    TEST_END;
}

void Path_Test_InitValidPath()
{
    TEST_BEGIN("Path_Test_InitValidPath");

    String strPath;
    Path path;
    String * dirName;

    String_Init(&strPath, "/valid/path");
    TEST_TRUE(Path_FromString(&path, &strPath));
    TEST_EQ(Path_Depth(&path), 2);
    dirName = Vector_At(&path, 0);
    TEST_STREQ(String_CharArray(dirName), "valid");
    dirName = Vector_At(&path, 1);
    TEST_STREQ(String_CharArray(dirName), "path");

    Path_Destroy(&path);
    String_Destroy(&strPath);

    String_Init(&strPath, "/valid/path/");
    TEST_TRUE(Path_FromString(&path, &strPath));
    TEST_EQ(Path_Depth(&path), 2);
    dirName = Vector_At(&path, 0);
    TEST_STREQ(String_CharArray(dirName), "valid");
    dirName = Vector_At(&path, 1);
    TEST_STREQ(String_CharArray(dirName), "path");
    Path_Destroy(&path);
    String_Destroy(&strPath);

    TEST_END;
}

void Path_Test_RelativePathInside()
{
    TEST_BEGIN("Path_Test_RelativePathInside");

    Path home, tmp;
    String strHome, strTmp;
    String relativePath;
    String_Init(&strHome, "/home/philipp");
    String_Init(&strTmp, "/home/philipp/tmp");
    Path_FromString(&home, &strHome);
    Path_FromString(&tmp, &strTmp);
    Path_RelativePath(&home, &tmp, &relativePath);
    TEST_STREQ(String_CharArray(&relativePath), "./tmp/");

    Path_Destroy(&home);
    Path_Destroy(&tmp);
    String_Destroy(&strHome);
    String_Destroy(&strTmp);
    String_Destroy(&relativePath);

    TEST_END;
}

void Path_Test_RelativePathOutside()
{
    TEST_BEGIN("Path_Test_RelativePathOutside");

    Path home, tmp;
    String strHome, strTmp;
    String relativePath;
    String_Init(&strHome, "/home/philipp/tmp");
    String_Init(&strTmp, "/home/horst/videos");
    Path_FromString(&home, &strHome);
    Path_FromString(&tmp, &strTmp);
    Path_RelativePath(&home, &tmp, &relativePath);
    TEST_STREQ(String_CharArray(&relativePath), "../../horst/videos/");

    Path_Destroy(&home);
    Path_Destroy(&tmp);
    String_Destroy(&strHome);
    String_Destroy(&strTmp);
    String_Destroy(&relativePath);

    TEST_END;
}

int main(int argc, char ** argv)
{
    Vector_Test_Append();
    String_Test_AppendChar();
    String_Test_Split_Path();
    String_Test_Split_NoDelimiter();
    Path_Test_InitRoot();
    Path_Test_InitNoPath();
    Path_Test_InitValidPath();
    Path_Test_RelativePathInside();
    Path_Test_RelativePathOutside();
    return 0;
}