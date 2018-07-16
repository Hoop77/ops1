//
// Created by philipp on 13.07.18.
//

#include "../include/filesystem/DedupLayer.h"

int main(int argc, char** argv)
{
	printf("-- Begin --\n");
	DedupLayer fs;
	DedupLayer_Init(&fs);
	Filesystem_Destroy(&fs);
	printf("--  End  --\n");
	return 0;
}
