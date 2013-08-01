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

static FILE *fd_log = NULL;

static const char* gettimestamp(void)
{
  time_t currenttime = 0;
  struct tm *localtime_tm = NULL;
  static char timestamp[512];

  time(&currenttime);
  localtime_tm = localtime(&currenttime);
  strftime(timestamp, 512, "%Y-%m-%d %H:%M:%S", localtime_tm);

  return timestamp;
}
 
void debug_init()
{
#ifdef DEBUGFILENAME
  fd_log = (strcmp(DEBUGFILENAME, "yes") == 0 ? stderr : fopen(DEBUGFILENAME, "a"));
	if (fd_log <= 0) {
		fd_log = stderr;
	}
#else
	fd_log = stderr;
#endif
}

/*
 *Print the debug info - only if debug is enabled for the build
 */

void dhvani_debug(const char *srcfile, int lineno, const char *format, ...)
{
#ifdef DEBUGFILENAME
	/* 
         * time_t t;
	 * struct timeval tv;
	 * char *tstr;
	 * 	t = time(NULL);
	 * 	tstr = strdup(ctime(&t));
	 * 	tstr[strlen(tstr) - 1] = 0;
	 * 	gettimeofday(&tv, NULL);
	 * 	if (!fd_log) {
	 * 		debug_init();
	 * 	}
	 * 	va_list args;
	 * 	va_start(args, format);
	 * 	fprintf(fd_log, "\n %s [%d]", tstr, (int)tv.tv_usec);
	 * 	vfprintf(fd_log, format, args);
	 * 	va_end(args);
	 * 	free(tstr);
         */
         va_list args;
	 if(!fd_log) debug_init();
	 va_start(args, format);
	 fprintf(fd_log, "%s: %d: DEBUG: ", srcfile, lineno, gettimestamp());
	 vfprintf(fd_log, format, args);
	 va_end(args);
	 fputc('\n', fd_log);
#endif
}

/*
 *Print the processing information to stdout
 */
void dhvani_info(const char *srcfile, int lineno, const char *format, ...)
{
         va_list args;
	 if(!fd_log) debug_init();
	 va_start(args, format);
	 fprintf(fd_log, "%s: %d: INFO: ", srcfile, lineno, gettimestamp());
	 vfprintf(fd_log, format, args);
	 va_end(args);
	 fputc('\n', fd_log);
}

/*
 *Print the error information to stderr
 */
void dhvani_error(const char *srcfile, int lineno, const char *format, ...)
{
         va_list args;
	 if(!fd_log) debug_init();
	 va_start(args, format);
	 fprintf(fd_log, "%s: %d: ERROR: ", srcfile, lineno, gettimestamp());
	 vfprintf(fd_log, format, args);
	 va_end(args);
	 fputc('\n', fd_log);
}
