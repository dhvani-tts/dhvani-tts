/* dhvani_lib.c
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

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "languages.h"
#include "synthesizer.h"
#include "dhvani_lib.h"
const char *
dhvani_Info ()
{				//=======================================
  return (VERSION);
}

const char *
dhvani_ListLanguage ()
{
  return
    "Malayalam\nHindi\nKannada\nGujarati\nOriya\nTelugu\nPanjabi\nBengali\nTamil\n";
}

int
dhvani_Initialize (dhvani_AUDIO_OUTPUT output, int buflength,
		   const char *path, int options)
{
  start_sythesizer ();
}

int
dhvani_SetParameter (dhvani_PARAMETER parameter, int value, int relative)
{				//=============================================================================================
  //ENTER("dhvani_SetParameter");
  dhvani_ERROR a_error;
  //SetParameter(parameter,value,relative);
  return 0;
}

int
dhvani_say (char *string, int langauge, int speed_factor, char *outputfile)
{
  text_to_speech (string, langauge, speed_factor, outputfile);
}

int
dhvani_speak_file (FILE * fd, int language, int speed, char *outputfile)
{
  file_to_speech (fd, language, speed, outputfile);
}

int
dhvani_speak_phonetic_file (FILE * fd)
{
  phonetic_to_speech (fd);
}

int
dhvani_IsPlaying (void)
{
  return isPlaying ();
}
