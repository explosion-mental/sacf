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
//TODO function that reads this enums and returns the desired value (fscanf /proc)
enum { POWERSAVE, TURBO, CPUS };

static const char *governors[] = {
    "performance",
    "ondemand",
    "conservative",
    "schedutil",
    "userspace",
    "powersave",
};
static int turbostate;

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

char
ischarging()
{
	FILE *fp;
	char online;
	//TODO handle multiple AC online ?

	//there has to be a better way?
	fp = popen("/bin/grep . /sys/class/power_supply/A*/online", "r");
	if (fp == NULL) {
		fprintf(stderr, "Failed to run grep.\n");
		exit(1);
	}

	//while (fgets(online, sizeof(online), fp) != NULL) {
	//online = fgetc(fp);

	online = getc(fp);
	pclose(fp);

	return online;
}

unsigned int
nproc(void)
{
	unsigned int cores, threads;
	FILE *fp = fopen("/proc/cpuinfo", "r");

	while (!fscanf(fp, "siblings\t: %u", &threads))
		fscanf(fp, "%*[^s]");
	//while (!fscanf(fp, "cpu cores\t: %u", &cores))
	//	fscanf(fp, "%*[^c]");
	fclose(fp);

	//return cores;
	return threads;

}

//#include <time.h>
static int
cpuload(void)
{
	//TODO get the cpu load over 1 second interval
	float up1, up2, idle1, idle2;
	FILE *fp = NULL;
	//clock

	/* first reading */
	fp = fopen("/proc/uptime", "r");
	fscanf(fp, "%f %f", &up1, &idle1);
	printf("uptime1: %f\nidle1: %f\n", up1, idle1);
	fclose(fp);

	sleep(1);

	/* second reading */
	fp = fopen("/proc/uptime", "r");
	fscanf(fp, "%f %f", &up2, &idle2);
	printf("uptime2: %f\nidle2: %f\n", up2, idle2);
	fclose(fp);



	//% CPU usage = (CPU time) / (# of cores) / (wall time)
	//CPU usage =  100 - 100 * (idle2 - idle1) / (uptime2 - uptime1)
	return 0;
}

//this should return an exit status.
static void
turbo(int on)
{
	FILE *fp = NULL;
	const char intel[] = "/sys/devices/system/cpu/intel_pstate/no_turbo";
	const char boost[] = "/sys/devices/system/cpu/cpufreq/boost";

	/* figure what path to use */
	if (access(intel, F_OK) != -1) {
		//fp = intel;
		fp = fopen(intel, "w");
	} else if (access(boost, F_OK) != -1) {
		//fp = boost;
		fp = fopen(boost, "w");
	} else {
		fprintf(stderr, "CPU turbo is not available.\n");
		return;
	}

	if (fp == NULL) {
	    fprintf(stderr, "Error opening file. Are you root?\n");
	    exit(1);
	}

	/* change state of the turbo */
	fprintf(fp, "%d\n", on);
	turbostate = on;

	fclose(fp);
}

static void
setgovernor(char *governor)
{
	FILE *fp = NULL;
	const char path[] = "/sys/devices/system/cpu/cpu";
	const char end[] = "/cpufreq/scaling_governor";
	char tmp[LENGTH(path) + sizeof(char) + LENGTH(end)];
	int i;

	for (i = 0; i < nproc(); i++) {
		/* store the path of cpu i on tmp */
		snprintf(tmp, LENGTH(tmp), "%s%d", path, i);
		strncat(tmp, end, LENGTH(end));
		printf("%s\n", tmp);

		/* set the governor of cpu i */
		fp = fopen(tmp, "w");
		if (fp == NULL) {
			fprintf(stderr, "Error opening file. Are you root?\n");
			exit(1);
		} else
		fprintf(fp, "%s\n", governor);
		//perror(""); //use perror and output whether they are missing privileges
		fclose(fp);
	}

}

static void
powersave()
{
	const char pp[] = "/sys/devices/system/cpu/cpu0/cpufreq/energy_performance_preference";
	const char intel[] = "/sys/devices/system/cpu/intel_pstate/hwp_dynamic_boost";

	if (access(pp, F_OK) != -1 && access(intel, F_OK) == -1) {
		printf("Using 'balance_power' governor.");
		setgovernor("balance_power");
	}
	//TODO depending on the cpuload() set different settings

}

int
main(int argc, char *argv[])
{
	int i;
	//FILE *fp = NULL;
	//chdir("/tmp");

	// log file in write mode
	//fp = fopen ("Log.txt", "w+");

	for (i = 1; i < argc; i++)
		/* these options take no arguments */
		if (!strcmp(argv[i], "-v")) {      /* prints version information */
			puts("sacf-"VERSION);
			exit(0);
		} else if (!strcmp(argv[i], "-l")) { /* number of cores */
			printf("Cores: %u\n", nproc());
			exit(0);
		} else if (!strcmp(argv[i], "-t")) { /* turbo on */
			turbo(1);
			exit(0);
		} else if (!strcmp(argv[i], "-T")) { /* turbo off */
			turbo(0);
			exit(0);
		} else if (!strcmp(argv[i], "-c")) { /* AC adapter status */
			printf("AC adapter status: %c\n", ischarging());
			exit(0);
		} else if (!strcmp(argv[i], "-p")) { /* set gorvenor to powersave */
			setgovernor("powersave");
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
