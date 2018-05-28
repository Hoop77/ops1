#include <stdio.h>
#include <stdlib.h>
#include "../include/common/Socket.h"
#include "../include/common/Utils.h"
#include "../include/common/Pipe.h"
#include <unistd.h>
#include <signal.h>

#define PORT 9000
#define HOST "127.0.0.1"
#define BUFLEN 512

static pid_t ReadProcess(Socket sock, Pipe connectedPipe);

static pid_t WriteProcess(Socket sock);

static pid_t FileTransferProcess();

int main()
{
	Socket sock;
	if (!Socket_Connect(&sock, HOST, PORT))
    {
        printf("Could not connect to remote shell!\n");
        exit(EXIT_FAILURE);
    }

    Pipe connectedPipe = {-1};
    Pipe_Init(connectedPipe);

    pid_t readPid = ReadProcess(sock, connectedPipe);
    pid_t writePid = WriteProcess(sock);
    pid_t fileTransferPid = FileTransferProcess();

    char connectedFlag;
    if (read(Pipe_ReadDescriptor(connectedPipe), &connectedFlag, 1) <= 0)
        terminate();

    printf("Disconnected!\n");

    kill(writePid, SIGTERM);
    kill(fileTransferPid, SIGTERM);

    Pipe_Destroy(connectedPipe);
    close(sock);

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
        write(STDOUT_FILENO, buf, (size_t) len);

    char connectedFlag = 1;
    if (write(Pipe_WriteDescriptor(connectedPipe), &connectedFlag, 1) <= 0)
        terminate();

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
        write(sock, buf, (size_t) len);

    exit(EXIT_SUCCESS);
    return -1;
}

static pid_t FileTransferProcess()
{
    pid_t pid;
    if ((pid = fork()) < 0)
        terminate();

    if (pid != 0)
        return pid;

    for (;;);

    exit(EXIT_SUCCESS);
    return -1;
}
