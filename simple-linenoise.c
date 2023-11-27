/* SPDX-License-Identifier: MIT */

#include <stdio.h>
#include <stdlib.h>

#include "linenoise.h"

int main(void)
{
	char *line;

	line = linenoise("hello> ");
	if (line) {
		printf("%s\n", line);
		free(line);
	}

	return EXIT_SUCCESS;
}
