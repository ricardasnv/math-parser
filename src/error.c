#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "error.h"

void warning(char* src, char* msg) {
	printf("WARNING: %s: %s\n", src, msg);
}

void append_log(char* path, char* src, char* msg) {
	FILE* file = fopen(path, "a");
	fprintf(file, "%s %s: %s\n", current_time(), src, msg);
	fclose(file);
}

void fatal(char* src, char* msg, int ret) {
	printf("FATAL: %s: %s\n", src, msg);
	append_log(LOGPATH, src, msg);
	exit(ret);
}

char* current_time() {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char* timestring = malloc(20 * sizeof(char)); // YYYY-MM-DD hh:mm:ss (20 chars)

	sprintf(timestring, "%04d-%02d-%02d %02d:%02d:%02d",
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec);

	return timestring;
}

