/* SPDX-License-Identifier: MIT */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <ell/ell.h>

#include "linenoise.h"

#define __unused __attribute__((unused))

static struct linenoiseState ls;
static char line_buf[1024];
static struct l_io *stdin_io = NULL;

static bool stdin_callback(__unused struct l_io *io, __unused void *user_data)
{
	char *line;

	line = linenoiseEditFeed(&ls);

	if (line == linenoiseEditMore)
		return true;

	linenoiseEditStop(&ls);

	if (line == NULL) {
		l_main_quit();
		return false;
	}

	printf("%s\n", line);
	linenoiseFree(line);

	l_main_quit();
	return false;
}

static void start_linenoise(__unused void *user_data)
{
	linenoiseEditStart(&ls, STDIN_FILENO, -1,
				line_buf, sizeof(line_buf), "hello> ");

	stdin_io = l_io_new(STDIN_FILENO);
	l_io_set_read_handler(stdin_io, stdin_callback, NULL, NULL);
}

static void signal_handler(uint32_t signo, __unused void *user_data)
{
	switch (signo) {
	case SIGINT:
	case SIGTERM:
		l_info("Terminate");
		l_main_quit();
		break;
	}
}

int main(void)
{
	int exit_status;

	l_log_set_stderr();

	if (!l_main_init())
		return EXIT_FAILURE;

	l_idle_oneshot(start_linenoise, NULL, NULL);

	exit_status = l_main_run_with_signal(signal_handler, NULL);

	l_io_destroy(stdin_io);

	l_main_exit();

	return exit_status;
}
