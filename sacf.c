/* See LICENSE file for copyright and license details. */
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include <dirent.h>
#include <errno.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"
/* macros */
#define LENGTH(a)               (sizeof(a) / sizeof(a)[0])
#define ABS(a)			((a) > 0 ? (a) : -(a))

void
usage(void)
{
	die("usage: sacf");
}

void
daemonize(void)
{
	pid_t id = 0;

	// child process
	id = fork();

	if (id < 0)
		die("fork failed.\n");

	// parent process
	if (id > 0) {
		//printf("child process %d\n", id);
		//kill the parent
		exit(0);
	}

	// unmask the file mode
	umask(0);

	//set new session
	if (setsid() < 0)
		exit(1);

	// close stdin, stdout and stderr
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

}

int
main(int argc, char *argv[])
{
	FILE *fp = NULL;
	//chdir("/tmp");

	// log file in write mode
	fp = fopen ("Log.txt", "w+");

	daemonize();
	while (1) {
		//Dont block context switches, let the process sleep for some time
		sleep(1);

		//logging
		fprintf(fp, "Logging info...\n");
		fflush(fp);

		// work here!
	}

	fclose(fp);

	return 0;
}
