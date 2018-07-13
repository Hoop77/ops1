//
// Created by philipp on 13.07.18.
//

#include "../include/filesystem/Filesystem.h"

int main(int argc, char** argv)
{
	printf("-- Begin --\n");
	fileSystem_t fs;
	fileSystemInit(&fs);
	fileSystemRelease(&fs);
	printf("--  End  --\n");
	return 0;
}
