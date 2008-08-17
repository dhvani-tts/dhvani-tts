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
 * Read the documentation at http://dhvani.sourceforge.net for the details.
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

typedef enum  {
    HINDI = 1,
    MALAYALAM = 2,
    TAMIL = 3,
    KANNADA = 4,
    ORIYA = 5,
    PANJABI = 6,
    GUJARATI = 7,
    TELUGU = 8,
    BENGALI = 9,
    MARATHI = 10
} dhvani_Languages;

typedef struct {
    struct dhvani_VOICE *voice; /* not used now.. for future use.*/
    float pitch;
    float tempo;
    int rate;
    dhvani_Languages language;
    int output_file_format;
    int isPhonetic;
    int speech_to_file;
    char * output_file_name;
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
const char *dhvani_Info();
/*
 *Initialize dhvani. Must be called once before using other APIs
 */
#ifdef __cplusplus
extern "C"
#endif
dhvani_options* dhvani_init();
/*
 *Use this API to utter a text. Set the speech parameters in the dhvani_options structure
 */
#ifdef __cplusplus
extern "C"
#endif
dhvani_ERROR dhvani_say(char *, dhvani_options*);
/*
 *Use this API to speak a file. Set the speech parameters in the dhvani_options structure
 */
#ifdef __cplusplus
extern "C"
#endif
dhvani_ERROR dhvani_speak_file(FILE *, dhvani_options*);
/*
 *Use this API to speak a dhvani phonetic file. For development purpose only
 */
#ifdef __cplusplus
extern "C"
#endif
dhvani_ERROR dhvani_speak_phonetic_file(FILE *);
/*
 *Call this to clean up everything and to close the synthesizer.Not mandatory but reccommended
 */
#ifdef __cplusplus
extern "C"
#endif
dhvani_ERROR dhvani_close();
#endif

