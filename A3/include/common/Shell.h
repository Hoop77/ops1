//
// Created by philipp on 25.05.18.
//

#ifndef REMOTESHELL_SHELL_H
#define REMOTESHELL_SHELL_H

#include <stdbool.h>
#include "Vector.h"

typedef struct
{

} Shell;

void Shell_Init(Shell * self);

void Shell_Destroy(Shell * self);

void Shell_Prompt(Shell * self);

void Shell_ChangeDirectory(Shell * self, Vector * args);

#endif //REMOTESHELL_SHELL_H
