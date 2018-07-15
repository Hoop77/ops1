//
// Created by philipp on 26.05.18.
//

#ifndef REMOTESHELL_PROGRAM_H
#define REMOTESHELL_PROGRAM_H

#include "Vector.h"
#include "String.h"

typedef enum
{
    PROGRAM_OPERATOR_NONE,
    PROGRAM_OPERATOR_BACKGROUND,
    PROGRAM_OPERATOR_PIPE
} ProgramOperator;

typedef struct
{
    Vector args;
    ProgramOperator operator;
} Program;

void Program_Init(Program * self);

void Program_Destroy(Program * self);

ProgramOperator Program_Operator(Program * self);

Vector * Program_Args(Program * self);

bool Program_ParseCmdLine(Vector * programs, const char * cmdLine);

#endif //REMOTESHELL_PROGRAM_H
