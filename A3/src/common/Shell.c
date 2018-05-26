//
// Created by philipp on 25.05.18.
//

#include "../../include/common/Shell.h"
#include "../../include/common/Utils.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

static void GetCwdPath(Path * path);

static void ChangeDirectory(Vector * args);

static void ShowPrompt(Shell * self);

void Shell_Init(Shell * self)
{
    GetCwdPath(&self->initialCwd);
}

void Shell_Destroy(Shell * self)
{
    Path_Destroy(&self->initialCwd);
}

void Shell_Prompt(Shell * self)
{
    ShowPrompt(self);

}

static void GetCwdPath(Path * path)
{
    char * cwd = getcwd(NULL, 0);
    if (!cwd)
        terminate();
    Path_InitFromCharArray(path, cwd);
    free(cwd);
}

static void ChangeDirectory(Vector * args)
{
    if (Vector_Size(args) <= 1)
        return;

    String * dir = Vector_At(args, 1);

    int fd = open(String_CharArray(dir), O_RDONLY | O_DIRECTORY);
    if (fd < 0 || fchdir(fd) < 0)
        perror("cd failed");
}

static void ShowPrompt(Shell * self)
{
    Path cwd;
    String relativePath;

    GetCwdPath(&cwd);
    Path_RelativePath(&self->initialCwd, &cwd, &relativePath);
    printf("%s> ", String_CharArray(&relativePath));

    String_Destroy(&relativePath);
    Path_Destroy(&cwd);
}