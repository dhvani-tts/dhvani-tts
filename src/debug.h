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

#ifndef _DHVANI_DEBUG_H
#define _DHVANI_DEBUG_H
#include <stdarg.h>

#define DHVANI_DEBUG(...) dhvani_debug(__FILE__, __LINE__, __VA_ARGS__)
#define DHVANI_INFO(...) dhvani_info(__FILE__, __LINE__, __VA_ARGS__)
#define DHVANI_ERROR(...) dhvani_error(__FILE__, __LINE__, __VA_ARGS__)

/*
 *Print the debug information to stderr
 */
void dhvani_debug(const char *srcfile, int lineno, const char *format, ...);
/*
 *Print the information to stdout
 */
void dhvani_info(const char *srcfile, int lineno, const char *format, ...);
/*
 *Print the error information to stderr
 */
void dhvani_error(const char *srcfile, int lineno, const char *format, ...);

#endif /* _DHVANI_DEBUG_H */
