//
// Created by philipp on 10.06.18.
//

#include <stdio.h>
#include "../include/common/Acceptor.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
	Acceptor acceptor;
	if (!Acceptor_Open(&acceptor, 10000))
	{
		perror("failed to bind to port 10000");
		exit(-1);
	}

	while (1)
	{
		Socket socket;
		if (!Acceptor_Accept(&acceptor, &socket))
			continue;

		printf("client connected\n");

		pid_t pid;
		if ((pid = fork()) < 0)
			continue;

		if (pid != 0)
			continue;

		sleep(3);
		char msg[] = "Hallo World!";
		if (write(socket, msg, strlen(msg) + 1) < 0)
		{
			perror("write failed");
			exit(-1);
		}
		close(socket);
		sleep(1);
		exit(0);
	}

	Acceptor_Close(&acceptor);
	return 0;
}
