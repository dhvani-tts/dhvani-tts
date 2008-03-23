/* synthesizer.h
 *
 * Copyright (C) 2007-2008
 *  Santhosh Thottingal <santhosh00@gmail.com>, Swathanthra Malayalam Computing.
 *  Ramesh Hariharan <ramesh@csa.iisc.ernet.in> 
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

#ifndef _SYNTHESIZER_H
#define _SYNTHESIZER_H
#include <stdlib.h>
#include <stdio.h>
/*------------------------------------------------------------------------
	This wii read phonetic script for various Indian languages.
	It will generate speech from the sound database and will play it.	

Authors: 
	Speech Synthesis from phonetic script algorithm & programming:
		 Dr. Ramesh Hariharan,
		 Indian Institute of Science, Bangalore, 
		 ramesh@csa.iisc.ernet.in
	Sound playback using ALSA:
	    	 Santhosh Thottingal,
	    	 Swathanthra Malayalam Computing,
	    	 santhosh.thottingal@gmail.com
---------------------------------------------------------------------------*/
/* structure to hold unicode input */
struct code
{
  int type;
  unsigned short alpha;
  unsigned char beta;
};

int file_to_speech (FILE *, int, int, char *);
int text_to_speech (char *, int, int, char *);
int phonetic_to_speech (FILE * fd);
#endif //end of _SYNTHESIZER_H
