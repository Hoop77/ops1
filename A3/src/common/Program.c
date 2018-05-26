//
// Created by philipp on 26.05.18.
//

#include <string.h>
#include "../../include/common/Program.h"
#include "../../include/common/String.h"

static void StringItemDestroyer(Vector_Item item)
{
    String_Destroy(item);
}

static void ProgramItemDestroyer(Vector_Item item)
{
    Program_Destroy(item);
}

static void ArgsInit(Vector * args)
{
    Vector_Init(args, sizeof(String), StringItemDestroyer);
}

void Program_Init(Program * self)
{
    ArgsInit(&self->args);
    self->operator = PROGRAM_OPERATOR_NONE;
}

void Program_Destroy(Program * self)
{
    Vector_Destroy(&self->args);
}

Program_Operator Program_GetOperator(Program * self)
{
    return self->operator;
}

Vector * Program_GetArgs(Program * self)
{
    return &self->args;
}

bool Program_ParseCmdLine(Vector * programs, const char * cmdLine)
{
    Vector_Init(programs, sizeof(Program), ProgramItemDestroyer);

    Program currProgram;
    Program_Init(&currProgram);
    String currArg;
    String_Init(&currArg);

    size_t len = strlen(cmdLine);
    for (size_t i = 0; i < len; ++i)
    {
        char c = cmdLine[i];
        Program_Operator currOperator = PROGRAM_OPERATOR_NONE;

        if (c == ' ')
        {
            if (!String_IsEmpty(&currArg))
            {
                // move currArgs into currProgram.args
                Vector_Append(&currProgram.args, &currArg);
                String_Init(&currArg);
            }
        }
        else if (c == '&')
            currOperator = PROGRAM_OPERATOR_BACKGROUND;
        else if (c == '|')
            currOperator = PROGRAM_OPERATOR_PIPE;
        else
            String_AppendChar(&currArg, c);

        if (currOperator != PROGRAM_OPERATOR_NONE)
        {
            // operand without program -> error
            if (Vector_IsEmpty(&currProgram.args))
            {
                Program_Destroy(&currProgram);
                String_Destroy(&currArg);
                return false;
            }

            currProgram.operator = currOperator;
            // move currProgram into programs
            Vector_Append(programs, &currProgram);
            Program_Init(&currProgram);
        }
    }

    // the last program specified may have no operator
    // so we may need to add the last program+args to programs
    if (!String_IsEmpty(&currArg))
        Vector_Append(&currProgram.args, &currArg);

    if (!Vector_IsEmpty(&currProgram.args))
    {
        currProgram.operator = PROGRAM_OPERATOR_NONE;
        Vector_Append(programs, &currProgram);
    }

    return true;
}