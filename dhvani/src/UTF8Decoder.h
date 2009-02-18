/* UTF8Decoder.h
 *
 * Copyright (C) 2007-2008
 *  Santhosh Thottingal <santhosh.thottingal@gmail.com>, 
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

#ifndef UTF8DECODER_H_
#define UTF8DECODER_H_
#include <stdio.h>

/* structure to hold unicode input */
struct code {
	int type;
	unsigned short alpha;
	unsigned char beta;
};
struct code utf8_to_utf16_text(unsigned char *, int *);
struct code utf8_to_utf16_file(FILE *);
#endif				/*UTF8DECODER_H_ */
