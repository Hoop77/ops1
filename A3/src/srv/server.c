#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../include/common/Acceptor.h"
#include "../../include/common/Utils.h"
#include "../../include/common/Shell.h"

#define PORT 9000

void RunShell(Socket);

int main()
{
	Acceptor acceptor;
	if (!Acceptor_Open(&acceptor, PORT))
        terminate();

	while (1)
	{
        Socket sock;
        if (!Acceptor_Accept(&acceptor, &sock))
            terminate();

        printf("Client connected!\n");

        pid_t pid;
        if ((pid = fork()) < 0)
            terminate();

        if (pid == 0)
            RunShell(sock);
	}

	Acceptor_Close(&acceptor);
	return 0;
}

void RunShell(Socket sock)
{
    if (dup2(sock, STDIN_FILENO) < 0 ||
        dup2(sock, STDOUT_FILENO) < 0 ||
        dup2(sock, STDERR_FILENO) < 0)
        terminate();

    Shell shell;
    Shell_Init(&shell);
    for (;;) Shell_Prompt(&shell);
    Shell_Destroy(&shell);
}
