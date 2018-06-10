#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../include/ult/Ult.h"
#include "../include/common/Socket.h"

static void threadC();

static void threadA()
{
    printf("A starts\n"); fflush(stdout);
	ult_yield();
	printf("A after 1. yield\n"); fflush(stdout);
	ult_spawn(threadC);
	printf("A spawned C\n"); fflush(stdout);
	ult_exit(0);
}

static void threadB()
{
    printf("B starts\n"); fflush(stdout);
	ult_yield();
	printf("B after 1. yield\n"); fflush(stdout);
	ult_yield();
	printf("B after 2. yield\n"); fflush(stdout);
	ult_exit(0);
}

static void threadC()
{
	printf("C starts\n"); fflush(stdout);
	ult_yield();
	printf("C after 1. yield\n"); fflush(stdout);
	ult_exit(0);
}

static void threadClient()
{
	Socket socket;
	if (!Socket_Connect(&socket, "127.0.0.1", 10000))
	{
		perror("could not connect to server");
		ult_exit(-1);
	}

	char msg[100];
	const size_t bytesToRead = strlen("Hello World") + 1;
	ssize_t bytesRead;
	if ((bytesRead = ult_read(socket, msg, bytesToRead)) < 0)
	{
		perror("ult_read failed");
		ult_exit(-1);
	}

	if (bytesRead != bytesToRead)
	{
		perror("received not correct number of bytes");
		ult_exit(-1);
	}

	printf("message received: %s\n", msg);
	close(socket);

	ult_exit(0);
}

static void myInit()
{
	int tids[3], i, status;

	printf("spawn first Client\n");
	tids[0] = ult_spawn(threadClient);
	printf("spawn second Client\n");
	tids[1] = ult_spawn(threadClient);
	printf("spawn A\n");
	tids[2] = ult_spawn(threadA);
	printf("spawn B\n");
	tids[3] = ult_spawn(threadB);
	printf("spawn C\n");
	tids[4] = ult_spawn(threadC);
	
	for (i = 0; i < 5; ++i)
	{
		printf("waiting for tids[%d] = %d\n", i, tids[i]);
		fflush(stdout);
		
		if (ult_join(tids[i], &status) < 0)
		{
			fprintf(stderr, "join for %d failed\n", tids[i]);
			ult_exit(-1);
		}
		
		printf("(status = %d)\n", status);
	}
	
	ult_exit(0);
}

int main()
{
	printf("Begin\n");
	ult_init(myInit);
	return 0;
}


