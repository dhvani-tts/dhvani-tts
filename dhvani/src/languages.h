/* languages.h
 *
 * Copyright (C) 2007-2008
 *  Santhosh Thottingal <santhosh00@gmail.com>, Swathanthra Malayalam Computing.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef LANGUAGES_H
#define LANGUAGES_H
#define HINDI 1
#define MALAYALAM 2
#define TAMIL 3
#define KANNADA 4
#define ORIYA 5
#define PANJABI 6
#define GUJARATI 7
#define TELUGU 8
#define BENGALI 9
#define MARATHI 10
int detect_language(unsigned short);
int get_language_code(char *);
#endif
