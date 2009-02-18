
/* dhvani_lib.h
 *
 * Copyright (C) 2007-2008
 *  Santhosh Thottingal <santhosh.thottingal@gmail.com>, Swathanthra Malayalam Computing.
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
/*********************************************************************/
/* A simple ogg encoder. contains source code from oggvorbis source  */
/*                                                                   */
/*********************************************************************/

#define READ 4096

typedef struct {
	int channels;
	int sampling_rate;
	/*   Encoding using a VBR quality mode.  The usable range is -.1
	   (lowest quality, smallest file) to 1. (highest quality, largest file).
	   Example quality mode .4: 44kHz stereo coupled, roughly 128kbps VBR */
	float quality;
} dhvani_ogg_parameters;
#ifdef WITH_OGGVORBIS
int oggenc(char *inputfile, char *outputfile, dhvani_ogg_parameters *);
#endif /*WITH_OGGVORBIS*/
