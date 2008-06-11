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
/*************************************************************/
/* This is the header file for the library version of dhvani */
/*                                                           */
/*************************************************************/

#define DHVANI_API_REVISION  1
#define VERSION "Version 0.92 (Beta)"
#define NORMAL 1
#define FAST 2

typedef enum {
    DHVANI_OK = 0,
    DHVANI_INTERNAL_ERROR = -1,
    DHVANI_BUFFER_FULL = 1,
    DHVANI_NOT_FOUND = 2
} dhvani_ERROR;
// voice table

typedef struct {
    const char *name; // a given name for this voice. UTF8 string.
    const char *languages; // list of pairs of (byte) priority + (string) language (and dialect qualifier)
    const char *identifier; // the filename for this voice within dhvani-data/voices
    unsigned char gender; // 0=none 1=male, 2=female,
    unsigned char age; // 0=not specified, or age in years
    unsigned char variant; // only used when passed as a parameter to dhvani_SetVoiceByProperties
} dhvani_VOICE;

typedef enum {
    /* PLAYBACK mode: plays the audio data, supplies events to the calling program */
    AUDIO_OUTPUT_PLAYBACK,

    /* RETRIEVAL mode: supplies audio data and events to the calling program */
    AUDIO_OUTPUT_RETRIEVAL,

    /* SYNCHRONOUS mode: as RETRIEVAL but doesn't return until synthesis is completed */
    AUDIO_OUTPUT_SYNCHRONOUS,

    /* Synchronous playback */
    AUDIO_OUTPUT_SYNCH_PLAYBACK,
    /* Save the data to file */
    AUDIO_OUTPUT_FILE
} dhvani_AUDIO_OUTPUT;


/***********************/
/*  Speech Parameters - Some of them are reserved for future use */

/***********************/

typedef enum {
    dhvaniSILENCE = 0, /* internal use */
    dhvaniRATE = 1,
    dhvaniVOLUME = 2,
    dhvaniPITCH = 3,
    dhvaniRANGE = 4,
    dhvaniPUNCTUATION = 5,
    dhvaniCAPITALS = 6,
    dhvaniWORDGAP = 7,
    dhvaniOPTIONS = 8, // reserved for misc. options.  not yet used
    dhvaniINTONATION = 9,

    dhvaniRESERVED1 = 10,
    dhvaniRESERVED2 = 11,
    dhvaniEMPHASIS, /* internal use */
    dhvaniLINELENGTH, /* internal use */
    dhvaniVOICETYPE, // internal, 1=mbrola
    N_SPEECH_PARAM /* last enum */
} dhvani_PARAMETER;

dhvani_ERROR dhvani_SetVoiceByName(const char *name);

const dhvani_VOICE **dhvani_ListVoices();
const char *dhvani_ListLanguage();
const char *dhvani_Info();
dhvani_ERROR dhvani_SetParameter(dhvani_PARAMETER parameter, int value,
        int relative);

dhvani_ERROR dhvani_Initialize(dhvani_AUDIO_OUTPUT output, int buflength,
        const char *path, int options);
dhvani_ERROR dhvani_say(char *, int, int, char *);
dhvani_ERROR dhvani_Cancel(void);

int dhvani_IsPlaying(void);

dhvani_ERROR dhvani_Terminate(void);
