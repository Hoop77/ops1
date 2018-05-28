//
// Created by philipp on 25.05.18.
//

#include "../../include/common/Shell.h"
#include "../../include/common/Utils.h"
#include "../../include/common/Program.h"
#include "../../include/common/Pipe.h"
#include <fcntl.h>
#include <wait.h>
#include <string.h>

#define VectorItem_ToPid(item) (* (pid_t *) item)

static void PidItemDestroyer(VectorItem item);

static void ProgramItemDestroyer(VectorItem item);

static void ShellProgramMappingDestroyer(VectorItem);

static bool PidItemComparartor(VectorItem, VectorItem);

static void SetupShellProgramMappings(Shell * self);

static void GetCwdPath(Path * path);

static void ShowPrompt(Shell * self);

static void ReadCmdLine(Vector * programs);

static void ExecutePrograms(Shell * self, Vector * programs);

static bool ExecuteShellProgram(Shell * self, Program * program);

static void ExecuteProgram(Program * program);

static void ConnectPipes(Pipe inPipe, Pipe outPipe, bool shouldPipeInput, bool shouldPipeOutput);

static void WaitFor(Vector * pids);

static bool GetWaitPids(Shell * self, Vector * args, Vector * pids);

static void ShellProgram_cd(Shell * shell, Vector * args);

static void ShellProgram_exit(Shell * shell, Vector * args);

static void ShellProgram_wait(Shell * shell, Vector * args);

static void ShowProcessInfo(pid_t pid, int status);

void Shell_Init(Shell * self)
{
    GetCwdPath(&self->initialCwd);
    Vector_Init(&self->backgroundProcesses, sizeof(pid_t), PidItemDestroyer);
    Vector_Init(&self->shellProgramMappings, sizeof(ShellProgramMapping), ShellProgramMappingDestroyer);
    SetupShellProgramMappings(self);
}

void Shell_Destroy(Shell * self)
{
    Path_Destroy(&self->initialCwd);
    Vector_Destroy(&self->backgroundProcesses);
    Vector_Destroy(&self->shellProgramMappings);
}

void Shell_Prompt(Shell * self)
{
    ShowPrompt(self);
    Vector programs;
    ReadCmdLine(&programs);
    ExecutePrograms(self, &programs);
    Vector_Destroy(&programs);
}

static void ProgramItemDestroyer(VectorItem item)
{
    Program_Destroy(item);
}

static void PidItemDestroyer(VectorItem item)
{
    // nothing to do
}

static void ShellProgramMappingDestroyer(VectorItem item)
{
    ShellProgramMapping * mapping = (ShellProgramMapping *) item;
    String_Destroy(&mapping->name);
}

static bool PidItemComparartor(VectorItem item1, VectorItem item2)
{
    return VectorItem_ToPid(item1) == VectorItem_ToPid(item2);
}

static void SetupShellProgramMappings(Shell * self)
{
    ShellProgramMapping mapping;

    String_InitFromCharArray(&mapping.name, "cd");
    mapping.program = ShellProgram_cd;
    Vector_Append(&self->shellProgramMappings, &mapping);

    String_InitFromCharArray(&mapping.name, "exit");
    mapping.program = ShellProgram_exit;
    Vector_Append(&self->shellProgramMappings, &mapping);

    String_InitFromCharArray(&mapping.name, "wait");
    mapping.program = ShellProgram_wait;
    Vector_Append(&self->shellProgramMappings, &mapping);
}

static void GetCwdPath(Path * path)
{
    char * cwd = getcwd(NULL, 0);
    if (!cwd)
        terminate();
    Path_InitFromCharArray(path, cwd);
    free(cwd);
}

static void ShowPrompt(Shell * self)
{
    Path cwd;
    String relativePath;

    GetCwdPath(&cwd);
    Path_RelativePath(&self->initialCwd, &cwd, &relativePath);
    printf("%s> ", String_CharArray(&relativePath));
    fflush(stdout);

    String_Destroy(&relativePath);
    Path_Destroy(&cwd);
}

static void ReadCmdLine(Vector * programs)
{
    Vector_Init(programs, sizeof(Program), ProgramItemDestroyer);

    char * line = NULL;
    size_t bufferSize = 0;
    ssize_t lineSize;
    if ((lineSize = getline(&line, &bufferSize, stdin)) < 0)
    {
        perror("unable to read command line");
        free(line);
        return;
    }

    // remove '\n' character
    line[lineSize - 1] = '\0';
    if (!Program_ParseCmdLine(programs, line))
        perror("parse error - invalid command");

    free(line);
}

static void ExecutePrograms(Shell * self, Vector * programs)
{
    Program * program = NULL;
    Pipe inPipe = {-1};
    Pipe outPipe = {-1};
    pid_t pid = -1;
    int status;
    bool isShellProgram = false;

    if (Vector_IsEmpty(programs))
        return;

    Pipe_Init(inPipe);
    Pipe_Init(outPipe);

    Vector_ForeachBegin(programs, program, i)
        // first we try to check whether there's a dedicated shell program that we can execute
        if ((isShellProgram = ExecuteShellProgram(self, program)))
            continue;

        if ((pid = fork()) < 0)
            terminate();

        if (pid == 0)
        {
            // child
            signal(SIGINT, SIG_IGN);
            bool shouldPipeInput =
                i == 0 ? false
                       : Program_Operator(Vector_At(programs, i - 1)) == PROGRAM_OPERATOR_PIPE;
            bool shouldPipeOutput = Program_Operator(program) == PROGRAM_OPERATOR_PIPE;
            ConnectPipes(inPipe, outPipe, shouldPipeInput, shouldPipeOutput);
            ExecuteProgram(program);
        }
        else
        {
            // parent
            switch (Program_Operator(program))
            {
                case PROGRAM_OPERATOR_BACKGROUND:
                    Vector_Append(&self->backgroundProcesses, &pid);
                    printf("[%d]\n", pid);
                    fflush(stdout);
                    break;

                case PROGRAM_OPERATOR_PIPE:
                    // The output pipe becomes the input pipe of the next process.
                    Pipe_Destroy(inPipe);
                    Pipe_Copy(outPipe, inPipe);
                    Pipe_Init(outPipe);
                    break;

                case PROGRAM_OPERATOR_NONE:
                    break;
            }
        }
    Vector_ForeachEnd

    Pipe_Destroy(inPipe);
    Pipe_Destroy(outPipe);

    if (Program_Operator(program) != PROGRAM_OPERATOR_BACKGROUND && !isShellProgram)
        waitpid(pid, &status, 0);
}

static bool ExecuteShellProgram(Shell * self, Program * program)
{
    Vector * args = Program_Args(program);
    String * name = Vector_At(args, 0);
    ShellProgramMapping * mapping;
    Vector_ForeachBegin(&self->shellProgramMappings, mapping, i)
        if (String_Equals(name, &mapping->name))
        {
            mapping->program(self, Program_Args(program));
            return true;
        }
    Vector_ForeachEnd
    return false;
}

static void ExecuteProgram(Program * program)
{
    // convert argument vector as Vector of String objects to char ** argument array
    Vector * argVector = Program_Args(program);
    size_t numArgs = Vector_Size(argVector);

    char ** args = malloc((numArgs + 1) * sizeof(char *));
    if (!args)
        terminate();
    args[numArgs] = NULL;

    String * argStr;
    Vector_ForeachBegin(argVector, argStr, i)
        size_t argSize = String_Size(argStr);
        args[i] = malloc((argSize + 1) * sizeof(char));
        if (!args[i])
            terminate();
        strcpy(args[i], String_CharArray(argStr));
    Vector_ForeachEnd

    char * filename = args[0];
    if (execvp(filename, args) < 0)
    {
        fprintf(stderr, "failed to execute '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
}

static void ConnectPipes(Pipe inPipe, Pipe outPipe, bool shouldPipeInput, bool shouldPipeOutput)
{
    if (shouldPipeInput)
    {
        Pipe_CloseWriteDescriptor(inPipe);
        if (dup2(Pipe_ReadDescriptor(inPipe), STDIN_FILENO) < 0)
            terminate();
    }
    else Pipe_Destroy(inPipe);

    if (shouldPipeOutput)
    {
        Pipe_CloseReadDescriptor(outPipe);
        if (dup2(Pipe_WriteDescriptor(outPipe), STDOUT_FILENO) < 0)
            terminate();
    }
    else Pipe_Destroy(outPipe);
}

static void WaitFor(Vector * pids)
{
    pid_t sentinel;

    if ((sentinel = fork()) == 0)
    {
        pause();
        exit(EXIT_SUCCESS);
    }

    signal(SIGINT, SIG_IGN);
    while (!Vector_IsEmpty(pids))
    {
        int status;
        pid_t returnedPid = wait(&status);

        if (returnedPid == sentinel)
        {
            printf("[wait canceled]\n");
            fflush(stdout);
            break;
        }

        pid_t * waitPid;
        Vector_ForeachBegin(pids, waitPid, i)
            if (*waitPid == returnedPid)
            {
                Vector_Remove(pids, i);
                ShowProcessInfo(returnedPid, status);
                break;
            }
        Vector_ForeachEnd
    }
    signal(SIGINT, SIG_DFL);

    if (Vector_IsEmpty(pids))
        kill(sentinel, SIGINT);
}

static void ShowProcessInfo(pid_t pid, int status)
{
    printf("[%d] exited with status: %d\n", pid, status);
    fflush(stdout);
}

static bool GetWaitPids(Shell * self, Vector * args, Vector * pids)
{
    Vector_Init(pids, sizeof(pid_t), PidItemDestroyer);

    size_t numArgs = Vector_Size(args);
    for (size_t i = 1; i < numArgs; ++i)
    {
        pid_t pid;
        if (!String_ToInt(Vector_At(args, i), &pid))
        {
            perror("wait: parse error");
            return false;
        }

        if (!Vector_Contains(&self->backgroundProcesses, &pid, PidItemComparartor))
        {
            fprintf(stderr, "PID [%d] is not a waiting process\n", pid);
            return false;
        }

        Vector_Append(pids, &pid);
    }
    return true;
}

static void ShellProgram_cd(Shell * shell, Vector * args)
{
    if (Vector_Size(args) <= 1)
        return;

    String * dir = Vector_At(args, 1);

    int fd = open(String_CharArray(dir), O_RDONLY | O_DIRECTORY);
    if (fd < 0 || fchdir(fd) < 0)
        perror("cd failed");
}

static void ShellProgram_exit(Shell * shell, Vector * args)
{
    exit(EXIT_SUCCESS);
}

static void ShellProgram_wait(Shell * shell, Vector * args)
{
    Vector pids;
    if (GetWaitPids(shell, args, &pids))
        WaitFor(&pids);
    Vector_Destroy(&pids);
}