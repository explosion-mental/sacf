/* See LICENSE file for copyright and license details. */
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include <dirent.h>
#include <errno.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
/* macros */
#define LENGTH(a)               (sizeof(a) / sizeof(a)[0])
#define ABS(a)			((a) > 0 ? (a) : -(a))

static const char *governors[] = {
    "performance",
    "ondemand",
    "conservative",
    "schedutil",
    "userspace",
    "powersave",
};

//static const long cpus = sysconf(_SC_NPROCESSORS_ONLN);
//static const int powersave_load_threshold = (75 * cpus) / 100;
//static const int performance_load_threshold = (50 * cpus) / 100;

void
usage(void)
{
	die("usage: sacf [-ctv]");
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

void
charge()
{
	FILE *fp;
	char online[2];
	char *power;

	power = "/sys/class/power_supply/";
	//A*/online
	//cat

	fp = popen("/bin/grep . /sys/class/power_supply/A*/online", "r");

	if (fp == NULL) {
		printf("Failed to run grep.\n" );
		exit(1);
	}

	/* Read the output a line at a time - output it. */
	while (fgets(online, sizeof(online), fp) != NULL) {
		printf("%s", online);
	}

	/* close */
	pclose(fp);
}

int
nproc(void)
{
	unsigned int cores, threads;
	FILE *fp = fopen("/proc/cpuinfo", "r");

//	while (!fscanf(fp, "siblings\t: %u", &threads))
//		fscanf(fp, "%*[^s]");
	while (!fscanf(fp, "cpu cores\t: %u", &cores))
		fscanf(fp, "%*[^c]");
	fclose(fp);

	return cores;

}

int
main(int argc, char *argv[])
{
	FILE *fp = NULL;
	int i;
	//chdir("/tmp");

	// log file in write mode
	//fp = fopen ("Log.txt", "w+");

	for (i = 1; i < argc; i++)
		/* these options take no arguments */
		if (!strcmp(argv[i], "-v")) {      /* prints version information */
			puts("sacf-"VERSION);
			exit(0);
		} else if (!strcmp(argv[i], "-t")) { /* number of cores */
			printf("Cores: %i\n", nproc());
			exit(0);
		} else if (!strcmp(argv[i], "-c")) {
			charge();
			exit(0);
		}
		//else if (i + 1 == argc)
		//	usage();
		else
			usage();

	daemonize();
	while (1) {
		//Dont block context switches, let the process sleep for some time
		sleep(1);

		//logging
		//fprintf(fp, "Logging info...\n");
		//fflush(fp);

		// work here!
	}

	//fclose(fp);

	return 0;
}
