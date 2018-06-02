#include <stdio.h>
#include <stdlib.h>
#include "../../include/common/Socket.h"
#include "../../include/common/Utils.h"
#include "../../include/common/Pipe.h"
#include "../../include/common/Reader.h"
#include "../../include/common/String.h"
#include <unistd.h>
#include <signal.h>

#define SHELL_PORT 9000
#define FILE_TRANSFER_PORT 9001
#define HOST "127.0.0.1"
#define BUFLEN 512

static pid_t ReadProcess(Socket sock, Pipe connectedPipe);

static pid_t WriteProcess(Socket sock);

static pid_t FileTransferProcess(Socket sock);

static void FileTransfer_Run(Socket sock, Reader * reader);

static void FileTransfer_Get(Socket sock, Reader * reader, String * filename, int fileSize);

int main()
{
	Socket shellSock;
    Socket fileTransferSock;
	if (!Socket_Connect(&shellSock, HOST, SHELL_PORT))
    {
        printf("Could not connect to remote shell!\n");
        exit(EXIT_FAILURE);
    }

    if (!Socket_Connect(&fileTransferSock, HOST, FILE_TRANSFER_PORT))
    {
        printf("Could not connect to remote shell!\n");
        exit(EXIT_FAILURE);
    }

    Pipe connectedPipe = {-1};
    Pipe_Init(connectedPipe);

    ReadProcess(shellSock, connectedPipe);
    pid_t writePid = WriteProcess(shellSock);
    pid_t fileTransferPid = FileTransferProcess(fileTransferSock);

    close(shellSock);
    close(fileTransferSock);

    char connectedFlag;
    if (read(Pipe_ReadDescriptor(connectedPipe), &connectedFlag, 1) <= 0)
        terminate();

    printf("Disconnected!\n");

    kill(writePid, SIGTERM);
    kill(fileTransferPid, SIGTERM);

    Pipe_Destroy(connectedPipe);

	return 0;
}

static pid_t ReadProcess(Socket sock, Pipe connectedPipe)
{
    pid_t pid;
    if ((pid = fork()) < 0)
        terminate();

    if (pid != 0)
    {
        Pipe_CloseWriteDescriptor(connectedPipe);
        return pid;
    }

    char buf[BUFLEN];
    ssize_t len;
    while ((len = read(sock, buf, BUFLEN)) > 0)
    {
        if (write(STDOUT_FILENO, buf, (size_t) len) <= 0)
            terminate();
    }

    char connectedFlag = 1;
    if (write(Pipe_WriteDescriptor(connectedPipe), &connectedFlag, 1) <= 0)
        terminate();

    close(sock);
    exit(EXIT_SUCCESS);
    return -1;
}

static pid_t WriteProcess(Socket sock)
{
    pid_t pid;
    if ((pid = fork()) < 0)
        terminate();

    if (pid != 0)
        return pid;

    char buf[BUFLEN];
    ssize_t len;
    while ((len = read(STDIN_FILENO, buf, BUFLEN)) > 0)
    {
        if (write(sock, buf, (size_t) len) <= 0)
            break;
    }

    close(sock);
    exit(EXIT_SUCCESS);
    return -1;
}

static pid_t FileTransferProcess(Socket sock)
{
    pid_t pid;
    if ((pid = fork()) < 0)
        terminate();

    if (pid != 0)
        return pid;

    Reader reader;
    Reader_Init(&reader, 1024);
    for (;;)
        FileTransfer_Run(sock, &reader);

    Reader_Destroy(&reader);
    close(sock);

    exit(EXIT_SUCCESS);
    return -1;
}

static void FileTransfer_Run(Socket sock, Reader * reader)
{
    char * buf;
    size_t len;
    if (!Reader_ReadUntil(reader, sock, &buf, &len, '\n'))
    {
        perror("connection error");
        return;
    }

    String cmd;
    String_InitFromCharArray(&cmd, buf);
    Vector split;
    String_Split(&cmd, ' ', &split);
    if (Vector_Size(&split) != 3)
    {
        perror("file transfer parsing error");
        free(buf);
        String_Destroy(&cmd);
        Vector_Destroy(&split);
        return;
    }

    String * cmdName = Vector_At(&split, 0);
    if (String_EqualsCharArray(cmdName, "get"))
    {
        String * filename = Vector_At(&split, 1);
        int fileSize = 0;
        String_ToInt(Vector_At(&split, 2), &fileSize);
        FileTransfer_Get(sock, reader, filename, fileSize);
    }

    free(buf);
    String_Destroy(&cmd);
    Vector_Destroy(&split);
}

static void FileTransfer_Get(Socket sock, Reader * reader, String * filename, int fileSize)
{
    FILE * file = fopen(String_CharArray(filename), "w");

    char buf[BUFLEN];
    size_t numBytesRead = 0;
    while (numBytesRead < fileSize)
    {
        size_t numBytesToRead = min(fileSize - numBytesRead, BUFLEN);

        if (!Reader_Read(reader, sock, buf, numBytesToRead))
        {
            perror("get failed");
            fclose(file);
            return;
        }

        if (!FileWrite(file, buf, numBytesToRead))
        {
            perror("get failed");
            fclose(file);
            return;
        }

        numBytesRead += numBytesToRead;
    }

    fclose(file);
}
