//
// Created by philipp on 24.05.18.
//

#include "../include/common/Vector.h"
#include "TestFramework.h"
#include "../include/common/String.h"
#include "../include/common/Path.h"
#include "../include/common/Shell.h"
#include "../include/common/Program.h"

TEST_INIT;

static void Vector_Test_Append()
{
    TEST_BEGIN("Vector_Test_Append");

    Vector vector;
    Vector_InitCharVector(&vector);
    Vector_Append(&vector, "a");
    Vector_Append(&vector, "b");
    Vector_Append(&vector, "c");

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

static void Vector_Test_Remove()
{
    TEST_BEGIN("Vector_Test_Remove");

    Vector vector;
    Vector_InitIntVector(&vector);
    int x;
    x = 5480;
    Vector_Append(&vector, &x);
    x = 5481;
    Vector_Append(&vector, &x);

    Vector_Remove(&vector, 0);
    TEST_EQ(Vector_Size(&vector), 1);

    x = VectorItem_ToInt(Vector_At(&vector, 0));
    TEST_EQ(x, 5481);

    Vector_Destroy(&vector);

    TEST_END;
}

static void Vector_Test_Insert()
{
    TEST_BEGIN("Vector_Test_Insert");

    Vector vector;
    Vector_InitIntVector(&vector);
    int x;
    x = 1;
    Vector_Append(&vector, &x);
    x = 3;
    Vector_Append(&vector, &x);
    x = 2;
    Vector_Insert(&vector, &x, 1);
    TEST_EQ(Vector_Size(&vector), 3);

    x = VectorItem_ToInt(Vector_At(&vector, 0));
    TEST_EQ(x, 1);
    x = VectorItem_ToInt(Vector_At(&vector, 1));
    TEST_EQ(x, 2);
    x = VectorItem_ToInt(Vector_At(&vector, 2));
    TEST_EQ(x, 3);

    Vector_Destroy(&vector);

    TEST_END;
}

static void String_Test_AppendChar()
{
    TEST_BEGIN("String_Test_AppendChar");

    String s;
    String_InitFromCharArray(&s, "Hello Worl");
    String_AppendChar(&s, 'd');
    TEST_STREQ(String_CharArray(&s), "Hello World");
    String_Destroy(&s);

    TEST_END;
}

static void String_Test_Split_Path()
{
    TEST_BEGIN("String_Test_Split_Path");

    String s;
    String_InitFromCharArray(&s, "/home/philipp");
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

static void String_Test_Split_NoDelimiter()
{
    TEST_BEGIN("String_Test_Split_Path");

    String s;
    String_InitFromCharArray(&s, "abc");

    Vector split;
    Vector_Init(&split, 0, NULL);

    String_Split(&s, ' ', &split);
    TEST_EQ(Vector_Size(&split), 1);
    TEST_STREQ(String_CharArray(Vector_At(&split, 0)), "abc");

    String_Destroy(&s);
    Vector_Destroy(&split);

    TEST_END;
}

static void Path_Test_InitRoot()
{
    TEST_BEGIN("Path_Test_InitRoot");

    Path path;
    TEST_TRUE(Path_InitFromCharArray(&path, "/"));
    TEST_EQ(Path_Depth(&path), 0);
    Path_Destroy(&path);

    TEST_END;
}

static void Path_Test_InitNoPath()
{
    TEST_BEGIN("Path_Test_InitNoPath");

    Path path;
    TEST_FALSE(Path_InitFromCharArray(&path, "abc"));
    Path_Destroy(&path);

    TEST_END;
}

static void Path_Test_InitValidPath()
{
    TEST_BEGIN("Path_Test_InitValidPath");

    Path path;
    String * dirName;

    TEST_TRUE(Path_InitFromCharArray(&path, "/valid/path"));
    TEST_EQ(Path_Depth(&path), 2);
    dirName = Vector_At(&path, 0);
    TEST_STREQ(String_CharArray(dirName), "valid");
    dirName = Vector_At(&path, 1);
    TEST_STREQ(String_CharArray(dirName), "path");

    Path_Destroy(&path);

    TEST_TRUE(Path_InitFromCharArray(&path, "/valid/path/"));
    TEST_EQ(Path_Depth(&path), 2);
    dirName = Vector_At(&path, 0);
    TEST_STREQ(String_CharArray(dirName), "valid");
    dirName = Vector_At(&path, 1);
    TEST_STREQ(String_CharArray(dirName), "path");

    Path_Destroy(&path);

    TEST_END;
}

static void Path_Test_RelativePathInside()
{
    TEST_BEGIN("Path_Test_RelativePathInside");

    Path home, tmp;
    String relativePath;
    Path_InitFromCharArray(&home, "/home/philipp");
    Path_InitFromCharArray(&tmp, "/home/philipp/tmp");
    Path_RelativePath(&home, &tmp, &relativePath);
    TEST_STREQ(String_CharArray(&relativePath), "./tmp/");

    Path_Destroy(&home);
    Path_Destroy(&tmp);
    String_Destroy(&relativePath);

    TEST_END;
}

static void Path_Test_RelativePathOutside()
{
    TEST_BEGIN("Path_Test_RelativePathOutside");

    Path home, tmp;
    String relativePath;
    Path_InitFromCharArray(&home, "/home/philipp/tmp");
    Path_InitFromCharArray(&tmp, "/home/horst/videos");
    Path_RelativePath(&home, &tmp, &relativePath);
    TEST_STREQ(String_CharArray(&relativePath), "../../horst/videos/");

    Path_Destroy(&home);
    Path_Destroy(&tmp);
    String_Destroy(&relativePath);

    TEST_END;
}

static void Program_Test_ParseCmdLine_SingleCmd()
{
    TEST_BEGIN("Program_Test_ParseCmdLine_SingleCmd");

    Vector programs;
    TEST_TRUE(Program_ParseCmdLine(&programs, "cd /tmp"));
    TEST_EQ(Vector_Size(&programs), 1);
    Program * program = Vector_At(&programs, 0);
    Vector * args = Program_Args(program);
    ProgramOperator operator = Program_Operator(program);
    TEST_EQ(Vector_Size(args), 2);
    TEST_STREQ(String_CharArray(Vector_At(args, 0)), "cd");
    TEST_STREQ(String_CharArray(Vector_At(args, 1)), "/tmp");
    TEST_EQ(operator, PROGRAM_OPERATOR_NONE);

    Vector_Destroy(&programs);

    TEST_END;
}

static void Program_Test_ParseCmdLine_MultipleCmds()
{
    TEST_BEGIN("Program_Test_ParseCmdLine_MultipleCmds");

    Vector programs;
    Program * program;
    Vector * args;
    ProgramOperator operator;

    TEST_TRUE(Program_ParseCmdLine(&programs, "cat test | grep bla & cd /tmp"));
    TEST_EQ(Vector_Size(&programs), 3);

    program = Vector_At(&programs, 0);
    args = Program_Args(program);
    operator = Program_Operator(program);
    TEST_EQ(Vector_Size(args), 2);
    TEST_STREQ(String_CharArray(Vector_At(args, 0)), "cat");
    TEST_STREQ(String_CharArray(Vector_At(args, 1)), "test");
    TEST_EQ(operator, PROGRAM_OPERATOR_PIPE);

    program = Vector_At(&programs, 1);
    args = Program_Args(program);
    operator = Program_Operator(program);
    TEST_EQ(Vector_Size(args), 2);
    TEST_STREQ(String_CharArray(Vector_At(args, 0)), "grep");
    TEST_STREQ(String_CharArray(Vector_At(args, 1)), "bla");
    TEST_EQ(operator, PROGRAM_OPERATOR_BACKGROUND);

    program = Vector_At(&programs, 2);
    args = Program_Args(program);
    operator = Program_Operator(program);
    TEST_EQ(Vector_Size(args), 2);
    TEST_STREQ(String_CharArray(Vector_At(args, 0)), "cd");
    TEST_STREQ(String_CharArray(Vector_At(args, 1)), "/tmp");
    TEST_EQ(operator, PROGRAM_OPERATOR_NONE);

    Vector_Destroy(&programs);

    TEST_END;
}

static void Program_Test_ParseCmdLine_InvalidCmd()
{
    TEST_BEGIN("Program_Test_ParseCmdLine_InvalidCmd");

    Vector programs;
    TEST_FALSE(Program_ParseCmdLine(&programs, "cat test & | grep"));
    Vector_Destroy(&programs);

    TEST_END;
}

static void Shell_Test()
{
    Shell shell;
    Shell_Init(&shell);
    for (;;) Shell_Prompt(&shell);
    Shell_Destroy(&shell);
}

int main(int argc, char ** argv)
{
    Vector_Test_Append();
    Vector_Test_Remove();
    Vector_Test_Insert();
    String_Test_AppendChar();
    String_Test_Split_Path();
    String_Test_Split_NoDelimiter();
    Path_Test_InitRoot();
    Path_Test_InitNoPath();
    Path_Test_InitValidPath();
    Path_Test_RelativePathInside();
    Path_Test_RelativePathOutside();
    Program_Test_ParseCmdLine_SingleCmd();
    Program_Test_ParseCmdLine_MultipleCmds();
    Program_Test_ParseCmdLine_InvalidCmd();

    Shell_Test();

    return 0;
}