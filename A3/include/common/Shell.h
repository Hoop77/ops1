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
} Shell;

void Shell_Init(Shell * self);

void Shell_Destroy(Shell * self);

void Shell_Prompt(Shell * self);

#endif //REMOTESHELL_SHELL_H
