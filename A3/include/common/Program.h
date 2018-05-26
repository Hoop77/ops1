//
// Created by philipp on 26.05.18.
//

#ifndef REMOTESHELL_PROGRAM_H
#define REMOTESHELL_PROGRAM_H

#include "Vector.h"

typedef enum
{
    PROGRAM_OPERATOR_NONE,
    PROGRAM_OPERATOR_BACKGROUND,
    PROGRAM_OPERATOR_PIPE
} Program_Operator;

typedef struct
{
    Vector args;
    Program_Operator operator;
} Program;

void Program_Init(Program * self);

void Program_Destroy(Program * self);

Program_Operator Program_GetOperator(Program * self);

Vector * Program_GetArgs(Program * self);

bool Program_ParseCmdLine(Vector * programs, const char * cmdLine);

#endif //REMOTESHELL_PROGRAM_H
