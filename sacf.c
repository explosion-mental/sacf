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
#include <stdarg.h>
#include <stdlib.h>

#ifdef __OpenBSD__
#include <sys/param.h>
#include <sys/sched.h>
#include <sys/sysctl.h>
#endif

#ifdef __FreeBSD__
#include <sys/param.h>
#include <sys/sysctl.h>
#include <devstat.h>
#endif


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

#include "config.h"

static void
usage(void)
{
	die("usage: sacf [-ctv]");
}

static int
pscanf(const char *path, const char *fmt, ...)
{
	FILE *fp;
	va_list ap;
	int n;

	if (!(fp = fopen(path, "r"))) {
		fprintf(stderr, "fopen '%s':", path);
		return -1;
	}
	va_start(ap, fmt);
	n = vfscanf(fp, fmt, ap);
	va_end(ap);
	fclose(fp);

	return (n == EOF) ? -1 : n;
}

static float
avgload(void)
{
	double avg[1];

	/* get the average load over 1 minute */
	if (getloadavg(avg, 1) < 0) {
		printf("getloadavg: Failed to obtain load average");
		return -1;
	}

	return avg[0];
}


static int
cpuperc(void)
{
	#ifdef __linux__
	static long double a[7];
	long double b[7], sum;

	memcpy(b, a, sizeof(b));
	/* cpu user nice system idle iowait irq softirq */
	if (pscanf("/proc/stat", "%*s %Lf %Lf %Lf %Lf %Lf %Lf %Lf",
	           &a[0], &a[1], &a[2], &a[3], &a[4], &a[5], &a[6])
	    != 7)
		return -1;

	if (b[0] == 0)
		return -1;

	sum = (b[0] + b[1] + b[2] + b[3] + b[4] + b[5] + b[6]) -
	      (a[0] + a[1] + a[2] + a[3] + a[4] + a[5] + a[6]);

	if (sum == 0)
		return -1;

	return (int)(100 *
	               ((b[0] + b[1] + b[2] + b[5] + b[6]) -
	                (a[0] + a[1] + a[2] + a[5] + a[6])) / sum);
	#endif /* __linux__ */

	#ifdef __OpenBSD__
	//TODO openbsd support
	#endif /* __OpenBSD__ */

	#ifdef __FreeBSD__
	//TODO freebsd support
	#endif
}


static void
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

static char
ischarging()
{
	char online;
	FILE *fp;
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

static unsigned int
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

static int
temperature(void)
{
#ifdef __linux__
	//uintmax_t temp;
	unsigned int temp;
	char file[] = "/sys/class/thermal/thermal_zone0/temp";

	if (pscanf(file, "%u", &temp) != 1) {
		return -1;
	}

	/* value in celsius */
	return temp / 1000;
#endif /* __linux__ */

	#ifdef __OpenBSD__
	//TODO openbsd support
	#endif /* __OpenBSD__ */

	#ifdef __FreeBSD__
	//TODO freebsd support
	#endif
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
	int cpuload, temp;
	float sysload;
	int load_threshold = (75 * nproc()) / 100;


	if (access(pp, F_OK) != -1 && access(intel, F_OK) == -1) {
		//printf("Using 'balance_power' governor.");
		setgovernor("balance_power");
	}

	cpuload = cpuperc();
	sysload = avgload();
	temp = temperature();

	if (cpuload >= minperc
	|| temp >= mintemp
	|| sysload >= load_threshold)
		turbo(1);
	else
		turbo(0);
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
