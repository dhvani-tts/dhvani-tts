/* debug.c
 *
 * Copyright (C) 2007-2008
 *  Santhosh Thottingal <santhosh.thottingal@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "debug.h"
static const char *FILENAME = "/tmp/dhvani.log";
static FILE *fd_log = NULL;
 
 
void debug_init()
{
	fd_log = fopen(FILENAME, "a");
	if (fd_log <= 0) {
		fd_log = stderr;
	}
}

/*
 *Print the debug info - only if debug is enabled for the build
 */

void dhvani_debug(const char *format, ...)
{
#ifdef WITH_DEBUG
	time_t t;
	struct timeval tv;
	char *tstr;
		t = time(NULL);
		tstr = strdup(ctime(&t));
		tstr[strlen(tstr) - 1] = 0;
		gettimeofday(&tv, NULL);
		if (!fd_log) {
			debug_init();
		}
		va_list args;
		va_start(args, format);
		fprintf(fd_log, "\n %s [%d]", tstr, (int)tv.tv_usec);
		vfprintf(fd_log, format, args);
		va_end(args);
		free(tstr);
#endif
}

/*
 *Print the processing information to stdout
 */
void dhvani_info(const char *format, ...)
{

	va_list args;
	va_start(args, format);
	vfprintf(stdout, format, args);
	va_end(args);
}

/*
 *Print the error information to stderr
 */
void dhvani_error(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	fprintf(stderr, "ERROR:");
	vfprintf(stderr, format, args);
	va_end(args);
}
