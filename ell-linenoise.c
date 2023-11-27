/* SPDX-License-Identifier: MIT */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <ell/ell.h>

#include "linenoise.h"

#define __unused __attribute__((unused))

#define UPDATE_RATE (5)

static struct linenoiseState ls;
static char line_buf[1024];
static struct l_io *stdin_io = NULL;
static struct l_timeout *update_to = NULL;

static void update_callback(struct l_timeout *timeout,
						__unused void *user_data)
{
	time_t rawtime;
	struct tm *tm;
	char str[80];

	linenoiseHide(&ls);

	time(&rawtime);
	tm = localtime(&rawtime);

	strftime(str, sizeof(str), "%H:%M:%S", tm);
	printf("Time is %s\n", str);

	linenoiseShow(&ls);

	l_timeout_modify(timeout, UPDATE_RATE);
}

static void setup_linenoise(void)
{
	linenoiseEditStart(&ls, STDIN_FILENO, STDOUT_FILENO,
				line_buf, sizeof(line_buf), "hello> ");
}

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

	if (line[0] != '\0') {
		linenoiseHistoryAdd(line);
		printf("%s\n", line);
	}

	linenoiseFree(line);

	setup_linenoise();

	return true;
}

static void completion_callback(const char *buf, linenoiseCompletions *lc)
{
	linenoiseAddCompletion(lc, buf);
}

static void start_linenoise(void)
{
	setup_linenoise();

	stdin_io = l_io_new(STDIN_FILENO);
	l_io_set_read_handler(stdin_io, stdin_callback, NULL, NULL);

	update_to = l_timeout_create(UPDATE_RATE, update_callback, NULL, NULL);

	linenoiseSetCompletionCallback(completion_callback);
}

static void stop_linenoise(void)
{
	l_timeout_remove(update_to);
	l_io_destroy(stdin_io);
}

static void start_callback(__unused void *user_data)
{
	start_linenoise();
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

	l_idle_oneshot(start_callback, NULL, NULL);

	exit_status = l_main_run_with_signal(signal_handler, NULL);

	stop_linenoise();

	l_main_exit();

	return exit_status;
}
