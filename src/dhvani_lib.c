/* dhvani_lib.c
 *
 * Copyright (C) 2007-2008
 *  Santhosh Thottingal <santhosh.thottingal@gmail.com>.
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
#include "debug.h"
#include "../config.h"
#include "dhvani_utils.h"

/*Return the dhvani version*/
const char* dhvani_version()
{
	return VERSION;
}

/*Return the supported language names*/
const char *dhvani_ListLanguage()
{
	return
	    "Malayalam (ml_IN)\nHindi (hi_IN)\nKannada (ka_IN)\nGujarati (gu_IN)\nOriya (or_IN)\nTelugu (te_IN)\nPanjabi (pa_IN)\nBengali (bn_IN)\nTamil (ta_IN)\nMarathi (mr_IN)\nPashto (ps_AF)\n";
}

/*Inititialise dhvani_options structure and synthesizer*/
dhvani_options *dhvani_init()
{
	dhvani_options *dhvani_handle;
	dhvani_handle = malloc(sizeof(dhvani_options));
	/*And set the default vales in options structure    */
	dhvani_handle->language = -1;
	dhvani_handle->isPhonetic = 0;
	dhvani_handle->speech_to_file = 0;
	dhvani_handle->pitch = 0.0;
	dhvani_handle->tempo = 0;
	dhvani_handle->rate = 16000;
	dhvani_handle->synth_callback_fn = NULL;
	dhvani_handle->audio_callback_fn = NULL;
#ifdef WITH_OGGVORBIS
	dhvani_handle->output_file_format = DHVANI_OGG_FORMAT;
#else
	dhvani_handle->output_file_format = DHVANI_WAV_FORMAT;
#endif
	/*Initialize the synthesizer */
	start_synthesizer();
	return dhvani_handle;
}

/*Read a given string*/
dhvani_ERROR dhvani_say(char *string, dhvani_options * options)
{
	if (text_to_speech(string, options) == DHVANI_OK) {
		return DHVANI_OK;
	} else {
		return DHVANI_INTERNAL_ERROR;
	}

}

/*Read a given filename*/

dhvani_ERROR dhvani_speak_file(FILE * fd, dhvani_options * options)
{
	return file_to_speech(fd, options);
}

/*Set the synthesizer callback*/
void
dhvani_set_synth_callback(t_dhvani_synth_callback * synthCallback,
			  dhvani_options * handle)
{
	handle->synth_callback_fn = synthCallback;
}

/*Set the audio callback*/
void
dhvani_set_audio_callback(t_dhvani_audio_callback * audioCallback,
			  dhvani_options * handle)
{
	handle->audio_callback_fn = audioCallback;
}

/*Experimental*/
dhvani_ERROR dhvani_speak_phonetic_file(FILE * fd, dhvani_options * options)
{
	phoneticfile_to_speech(fd, options);
	return DHVANI_OK;
}

/*Experimental*/
dhvani_ERROR
dhvani_speak_phonetic(char *phonetic_string, dhvani_options * options)
{
	phonetic_to_speech(phonetic_string, options);
	return DHVANI_OK;
}

/*stop the synthesizer*/
dhvani_ERROR dhvani_close()
{
	stop_synthesizer();
	return DHVANI_OK;
}
