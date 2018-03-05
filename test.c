#include <stdio.h>
#include <stdlib.h>
#include "read_line.h"

#define MAX_LINE	256

int  main(int argc, char **argv){
	char buffer[MAX_LINE];
	int err = 0;
	int n;

	while (err != -1) { /*while there is no error*/
		n = readLine(0, buffer, MAX_LINE); /*read from the standard input*/
		if (n!=-1) /*check errors*/
			write(1, buffer, n); /*write what was written*/
	}
	exit(0);
}
