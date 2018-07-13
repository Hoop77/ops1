//
// Created by philipp on 28.05.18.
//

#include <stdio.h>
#include "../../include/common/RemoteShell.h"
#include "../../include/common/Utils.h"

#define BUFLEN 1024

static void SetupShellProgramMappings(RemoteShell * self);

static void ShellProgram_get(Shell * shell, Vector * args);

static void ShellProgram_put(Shell * shell, Vector * args);

void RemoteShell_Init(RemoteShell * self, Socket fileTransferSock)
{
    Shell_Init(&self->base);
    SetupShellProgramMappings(self);
    self->fileTransferSock = fileTransferSock;
}

void RemoteShell_Destroy(RemoteShell * self)
{
    Shell_Destroy(&self->base);
    close(self->fileTransferSock);
}

static void SetupShellProgramMappings(RemoteShell * self)
{
    Shell * base = (Shell *) self;
    ShellProgramMapping mapping;

    String_InitFromCharArray(&mapping.name, "get");
    mapping.program = ShellProgram_get;
    Vector_Append(&base->shellProgramMappings, &mapping);

    String_InitFromCharArray(&mapping.name, "put");
    mapping.program = ShellProgram_put;
    Vector_Append(&base->shellProgramMappings, &mapping);
}

static void ShellProgram_get(Shell * shell, Vector * args)
{
    RemoteShell * self = (RemoteShell * ) shell;
    Socket sock = self->fileTransferSock;

    if (Vector_Size(args) < 2)
    {
        perror("get: missing argument");
        return;
    }

    String * filename = Vector_At(args, 1);
    FILE * file = fopen(String_CharArray(filename), "r");
    if (!file)
    {
        fprintf(stderr, "'%s' not found", String_CharArray(filename));
        return;
    }

    size_t fileSize = GetFileSize(file);

    String cmd;
    String_InitFromCharArray(&cmd, "get ");
    String_Append(&cmd, filename);
    String_AppendChar(&cmd, ' ');
    char strFileSize[50];
    sprintf(strFileSize, "%d", (int) fileSize);
    String_AppendCharArray(&cmd, strFileSize);
    String_AppendChar(&cmd, '\n');

    if (!Write(sock, String_CharArray(&cmd), String_Size(&cmd)))
    {
        perror("get failed");
        fclose(file);
        String_Destroy(&cmd);
        return;
    }

    char buf[BUFLEN];
    size_t numBytesWritten = 0;
    while (numBytesWritten < fileSize)
    {
        size_t numBytesToWrite = min(fileSize - numBytesWritten, BUFLEN);

        if (!FileRead(file, buf, numBytesToWrite))
        {
            perror("get failed");
            fclose(file);
            String_Destroy(&cmd);
            return;
        }

        if (!Write(sock, String_CharArray(&cmd), String_Size(&cmd)))
        {
            perror("get failed");
            fclose(file);
            String_Destroy(&cmd);
            return;
        }

        numBytesWritten += numBytesToWrite;
    }

    fclose(file);
    String_Destroy(&cmd);
}

static void ShellProgram_put(Shell * shell, Vector * args)
{
    if (Vector_Size(args) < 2)
    {
        perror("Missing argument!");
        return;
    }

}