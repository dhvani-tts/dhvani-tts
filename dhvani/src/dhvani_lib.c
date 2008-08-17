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
#include "oggencoder.h"
#include "debug.h"
#include "../config.h"

const char *
dhvani_Info() {
    return (VERSION);
}

const char *
dhvani_ListLanguage() {
    return
    "Malayalam\nHindi\nKannada\nGujarati\nOriya\nTelugu\nPanjabi\nBengali\nTamil\nMarathi\n";
}

dhvani_options *
dhvani_init() {
    dhvani_options *dhvani_handle;
    /*Initialize the synthesizer*/
    start_synthesizer();
    /*And set the default vales in options structure    */
    dhvani_handle->language=-1;
    dhvani_handle->isPhonetic = 0;
    dhvani_handle->speech_to_file = 0;
    dhvani_handle->pitch = 0.0;
    dhvani_handle->tempo = 0;
    dhvani_handle->rate = 16000;
    dhvani_handle->output_file_format = DHVANI_OGG_FORMAT;
    return dhvani_handle;
}

dhvani_ERROR
dhvani_say(char *string, dhvani_options * options) {
    dhvani_ogg_parameters ogg_parameters;
    char *tempfile_name;
    /*check whether we need to encode the output file to ogg format */
    if (options->speech_to_file == 1) {
        if (options->output_file_format == DHVANI_OGG_FORMAT) {
            /*reserve a 25 byte for the /tmp/dhvani<pid> string. Hope it will not go beyond 25*/
            tempfile_name = (char *) malloc(25);
            /*construct the temperory file name. since this is going to be unique among apps, maked 
            any number  of dhvani instances run parallelly*/
            sprintf(tempfile_name, "/tmp/dhvani%d", getpid());
            if (text_to_speech
                    (string, options 
                     ) == DHVANI_OK) {
                dhvani_info("Encoding the output to ogg format\n");
                ogg_parameters.channels = 2;
                ogg_parameters.sampling_rate = 16000;
                ogg_parameters.quality = 0.4;
                oggenc(tempfile_name, options->output_file_name, &ogg_parameters);
                dhvani_info("Speech is written to %s\n",options->output_file_name);
            }
            else{
                    return DHVANI_INTERNAL_ERROR;
            }
        } else {
            if (text_to_speech(string, options)
                    == DHVANI_OK) {
               dhvani_info("Writing the speech to %s...\n", options->output_file_name);
               dhvani_info("To play this file use the following command:\n");
                dhvani_info("aplay -f S16_LE -r 16000 %s\n", options->output_file_name);
                return DHVANI_OK;
            }
            else{
                    return DHVANI_INTERNAL_ERROR;
            }
        }
    } else { /*if there is no output file*/
        return text_to_speech(string, options);

    }
    return DHVANI_INTERNAL_ERROR;
}

dhvani_ERROR
dhvani_speak_file(FILE * fd, dhvani_options * options) {
    dhvani_ogg_parameters ogg_parameters;
    char *tempfile_name;
    /*check whether we need to encode the output file to ogg format */
    if (options->speech_to_file==1) {
        if (options->output_file_format == DHVANI_OGG_FORMAT) {
            dhvani_debug("output file format is ogg");
            /*reserve a 25 byte for the /tmp/dhvani<pid> string. Hope it will not go beyond 25*/
            tempfile_name = (char *) malloc(25);
            /*construct the temperory file name. since this is going to be unique among apps, maked 
            any number  of dhvani instances run parallelly*/
            sprintf(tempfile_name, "/tmp/dhvani%d", getpid());
            if (file_to_speech
                    (fd, options) == DHVANI_OK) {
                dhvani_info( "Encoding the output to ogg format\n");
                ogg_parameters.channels = 2;
                ogg_parameters.sampling_rate = 16000;
                ogg_parameters.quality = 0.4;
                oggenc(tempfile_name, options->output_file_name, &ogg_parameters);
               dhvani_info("Speech is written to %s\n", options->output_file_name);
            }
        } else {
            if (file_to_speech(fd, options)
                    == DHVANI_OK) {
                dhvani_info( "Writing the speech to %s...\n", options->output_file_name);
               dhvani_info( "To play this file use the following command:\n");
                dhvani_info("aplay -f S16_LE -r 16000 %s\n", options->output_file_name);
            }
        }
    } else { /*if there is no output file*/
        file_to_speech(fd, options);

    }
}

dhvani_ERROR
dhvani_speak_phonetic_file(FILE * fd) {
    phonetic_to_speech(fd);
}

dhvani_ERROR
dhvani_close() {
    stop_synthesizer();
}
