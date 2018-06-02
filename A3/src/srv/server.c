#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../include/common/Acceptor.h"
#include "../../include/common/Utils.h"
#include "../../include/common/Shell.h"
#include "../../include/common/RemoteShell.h"

#define SHELL_PORT 9000
#define FILE_TRANSFER_PORT 9001

void ShellProcess(Socket, Socket);

int main()
{
	Acceptor shellAcceptor;
    Acceptor fileTransferAcceptor;

	if (!Acceptor_Open(&shellAcceptor, SHELL_PORT))
        terminate();

    if (!Acceptor_Open(&fileTransferAcceptor, FILE_TRANSFER_PORT))
        terminate();

	while (1)
	{
        Socket shellSock;
        Socket fileTransferSock;

        if (!Acceptor_Accept(&shellAcceptor, &shellSock))
            terminate();

        if (!Acceptor_Accept(&fileTransferAcceptor, &fileTransferSock))
            terminate();

        printf("Client connected!\n");

        ShellProcess(shellSock, fileTransferSock);
	}

	Acceptor_Close(&shellAcceptor);
	return 0;
}

void ShellProcess(Socket sock, Socket fileTransferSocket)
{
    pid_t pid;
    if ((pid = fork()) < 0)
        terminate();

    if (pid != 0)
        return;

    if (dup2(sock, STDIN_FILENO) < 0 ||
        dup2(sock, STDOUT_FILENO) < 0 ||
        dup2(sock, STDERR_FILENO) < 0)
        terminate();

    RemoteShell remoteShell;
    RemoteShell_Init(&remoteShell, fileTransferSocket);
    for (;;)
        Shell_Prompt((Shell *) &remoteShell);
    RemoteShell_Destroy(&remoteShell);
}