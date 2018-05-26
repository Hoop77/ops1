//
// Created by philipp on 25.05.18.
//

#include "../../include/common/Shell.h"
#include "../../include/common/String.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

void Shell_Init(Shell * self)
{

}

void Shell_Destroy(Shell * self)
{

}

void Shell_Prompt(Shell * self)
{

}

void Shell_ChangeDirectory(Shell * self, Vector * args)
{
    if (Vector_Size(args) <= 1)
        return;

    String * dir = Vector_At(args, 1);

    int fd = open(String_CharArray(dir), O_RDONLY | O_DIRECTORY);
    if (fd < 0 || fchdir(fd) < 0)
        perror("cd failed");
}