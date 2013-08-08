#ifndef _DHVANI_LIB_H
#define _DHVANI_LIB_H
/********************************************************************************** 
 * dhvani_lib.h
 *
 * Copyright (C) 2007-2008
 *  Santhosh Thottingal <santhosh.thottingal@gmail.com>
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

/*-------------------------------------------------------------------------*
 * This is the header file for the library version of dhvani.
 * Read the documentation at http://dhvani.sourceforge.net for  details.
 *-------------------------------------------------------------------------*/

#define DHVANI_API_REVISION  1
#include <stdio.h>
typedef enum {
	DHVANI_OK = 0,
	DHVANI_INTERNAL_ERROR = -1
} dhvani_ERROR;

typedef enum {
	DHVANI_OGG_FORMAT = 0,
	DHVANI_WAV_FORMAT
} dhvani_output_file_format;

typedef enum {
	HINDI = 1,
	MALAYALAM = 2,
	TAMIL = 3,
	KANNADA = 4,
	ORIYA = 5,
	PANJABI = 6,
	GUJARATI = 7,
	TELUGU = 8,
	BENGALI = 9,
	MARATHI = 10,
	PASHTO = 11
} dhvani_Languages;

/* callback function*/

typedef int (t_dhvani_synth_callback) (int);
typedef int (t_dhvani_audio_callback) (short *, int);

typedef struct {
	struct dhvani_VOICE *voice;	/* not used now.. for future use. */
	float pitch;
	float tempo;
	int rate;
	dhvani_Languages language;
	int output_file_format;
	int isPhonetic;
	int speech_to_file;
	char *output_file_name;
	t_dhvani_synth_callback *synth_callback_fn;
	t_dhvani_audio_callback *audio_callback_fn;
} dhvani_options;

/*
 *Get a list of supported Languages
 */
#ifdef __cplusplus
extern "C"
#endif
const char *dhvani_ListLanguage();
/*
 *Print the version details of the program
 */
#ifdef __cplusplus
extern "C"
#endif
const char* dhvani_version();
/*	
 *Initialize dhvani. Must be called once before using other APIs
 */
#ifdef __cplusplus
extern "C"
#endif
 dhvani_options * dhvani_init();

/* Must be called before any synthesis functions are called.
   This specifies a function in the calling program which is called when a word(after tokenizing a sentense) is finished processing.
   The location of the synthesizer in the input is returned. 

   The callback function is of the form:

   int SynthCallback(int text_position);
   text_position:  the number of characters from the start of the text that has been finished.
   Callback returns: 0=continue synthesis,  1=abort synthesis.
*/
#ifdef __cplusplus
extern "C"
#endif
void dhvani_set_synth_callback(t_dhvani_synth_callback *, dhvani_options *);
/* Must be called before any synthesis functions are called.
   This specifies a function in the calling program which is called when a buffer of
   speech sound data has been produced. 

   The callback function is of the form:

   int SynthCallback(short *wav, int numsamples);

   wav:  is the speech sound data which has been produced.
      NULL indicates that the synthesis has been completed.

   numsamples: is the number of entries in wav.  This number may vary, may be less than
      the value implied by the buflength parameter given in espeak_Initialize, and may
      sometimes be zero (which does NOT indicate end of synthesis).
*/
#ifdef __cplusplus
extern "C"
#endif
void dhvani_set_audio_callback(t_dhvani_audio_callback *, dhvani_options *);

/*
 *Use this API to utter a text. Set the speech parameters in the dhvani_options structure
 */
#ifdef __cplusplus
extern "C"
#endif
 dhvani_ERROR dhvani_say(char *, dhvani_options *);
/*
 *Use this API to speak a file. Set the speech parameters in the dhvani_options structure
 */
#ifdef __cplusplus
extern "C"
#endif
 dhvani_ERROR dhvani_speak_file(FILE *, dhvani_options *);
/*
 *Use this API to speak a dhvani phonetic file. For development purpose only
 */
#ifdef __cplusplus
extern "C"
#endif
 dhvani_ERROR dhvani_speak_phonetic_file(FILE *, dhvani_options *);

/*
 *Use this API to speak a dhvani phonetic string. For development purpose only
 */
#ifdef __cplusplus
extern "C"
#endif
 dhvani_ERROR dhvani_speak_phonetic(char *, dhvani_options *);
/*
 *Call this to clean up everything and to close the synthesizer.Not mandatory but reccommended
 */
#ifdef __cplusplus
extern "C"
#endif
 dhvani_ERROR dhvani_close();

#endif
