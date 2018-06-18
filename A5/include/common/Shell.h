//
// Created by philipp on 25.05.18.
//

#ifndef REMOTESHELL_SHELL_H
#define REMOTESHELL_SHELL_H

#include <stdbool.h>
#include "Vector.h"
#include "Path.h"

typedef struct
{
    Path initialCwd;
    Vector backgroundProcesses;
    Vector shellProgramMappings;
} Shell;

// A shell program represents program which is part of the shell (e.g. cd, exit or wait).
typedef void (*ShellProgram)(Shell * shell, Vector * args);

// Maps a name to a shell program function pointer.
typedef struct
{
    String name;
    ShellProgram program;
} ShellProgramMapping;

void Shell_Init(Shell * self);

void Shell_Destroy(Shell * self);

void Shell_Prompt(Shell * self);

#endif //REMOTESHELL_SHELL_H
