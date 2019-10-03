/*
Author: Erik Rivas Contreras
Date: April 16, 2019
Purpose: read atto-c source file from command line, hooks it up to lexical analyzer
figures out name of output assembly file, wrtites out assembly preamble. Closes assembly
output file.
*/

#define _CRT_SECURE_NO_WARNINGS
#include "atto-C.h"

FILE *sourcefile;
FILE *asmfile;

int print_flag;

main(int argc, char **argv)
{
	//looking for options
	while ((argv[1] != NULL) && argv[1][0] == '-')
	{
		switch (argv[1][1])
		{
		case 'd':
			print_flag = 1; //debug mode on
			break;
		default: printf("Unknown option: %c\n", argv[1][1]);
			exit(1);
		}
		argc--;  // reduce the argument count
		argv++;  // move the argument pointer one word over
	}

	//checking for source file
	if (argv[1] == NULL)
	{
		printf("Missing source file on command line.\n");
		exit(2);
	}

	if (argc != 2) {
		printf("ERROR: Missing source file on command line.\n");
		exit(3);
	}

	sourcefile = fopen(argv[1], "r");

	if (sourcefile == NULL) {
		perror(argv[1]);
		exit(4);
	}
	
	char asmfilename[100];

	//copying source name file 
	strncpy(asmfilename, argv[1], 95); //95 is max length to copy

	//replace the last two characters (presumably ".c") with ".asm"
	int length;
	length = strlen(asmfilename);
	if (strcmp(asmfilename + length - 2, ".c") != 0)
	{
		//Error message about source file not having a.c extension, and exit.
	}
	strcpy(asmfilename + length - 2, ".asm");

	//Now open it for writing and test for success,
	asmfile = fopen(asmfilename, "w");
	if (asmfile == NULL)
	{
		perror(asmfilename);
		exit(5);
	}

	//call to preamble
	preamble(argv[1]);

	//activating parser
	program();
	postamble();

}