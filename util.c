/* See LICENSE file for copyright and license details. */
#include <glob.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

void
die(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt)-1] == ':') {
		fputc(' ', stderr);
		perror(NULL);
	} else
		fputc('\n', stderr);

	exit(EXIT_FAILURE);
}

void *
ecalloc(size_t nmemb, size_t size)
{
	void *p;

	if (!(p = calloc(nmemb, size)))
		die("calloc:");
	return p;
}

void
eglob(const char *path, glob_t *muhglob)
{
	switch (glob(path, GLOB_NOSORT, NULL, muhglob)) {
	case GLOB_NOSPACE:
		die("glob failed: running out of memory");
		break;
	case GLOB_ABORTED:
		die("glob failed: read error");
		break;
	case GLOB_NOMATCH:
		fprintf(stderr, "glob: no matches\n");
		break;
	}
}

int
pscanf(const char *path, const char *fmt, ...)
{
	FILE *fp;
	va_list ap;
	int n;

	if (!(fp = fopen(path, "r")))
		die("fopen '%s':", path);

	va_start(ap, fmt);
	n = vfscanf(fp, fmt, ap);
	va_end(ap);
	fclose(fp);

	return (n == EOF) ? -1 : n;
}

int
pprintf(const char *path, const char *fmt, ...)
{
	FILE *fp;
	va_list ap;
	int n;

	if (!(fp = fopen(path, "w")))
		die("fopen '%s':", path);

	va_start(ap, fmt);
	n = vfprintf(fp, fmt, ap);
	va_end(ap);
	fclose(fp);

	return (n == EOF) ? -1 : n;
}
