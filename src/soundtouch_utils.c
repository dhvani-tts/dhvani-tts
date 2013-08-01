/*soundtouch_utils.c
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

#ifdef  WITH_SOUNDTOUCH
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include "soundtouch4c.h"
#include "soundtouch_utils.h"
#include "dhvani_lib.h"
#include "debug.h"
#define CHUNK_LENGTH 4096
struct soundtouch *stouch = NULL;
/*Initialize the soundtouch processor*/
struct soundtouch *soundtouch_create(dhvani_options * options)
{
	struct soundtouch *snd;

	DHVANI_DEBUG("Creating SoundTouch object...\n");
	snd = SoundTouch_construct();
	if (!snd) {
		DHVANI_ERROR("Failed to create SoundTouch object\n");
		return NULL;
	}

	SoundTouch_setChannels(snd, 1);
	SoundTouch_setSetting(snd, SETTING_USE_QUICKSEEK, 1);
	SoundTouch_setSetting(snd, SETTING_USE_AA_FILTER, 1);

	return snd;
}

/*Free the soundtouch processor*/
void SoundTouch_free()
{
	DHVANI_DEBUG("Freeing SoundTouch object...\n");
	SoundTouch_destruct(stouch);
}

short *shortcat(short *dst, int dstsize, short *src, int srcsize)
{
	short *result = malloc((dstsize + srcsize) * sizeof(short));
	int i = 0, j = 0;
	while (i < dstsize) {
		result[i] = dst[i++];
	}
	while (j < srcsize) {
		result[i++] = src[j++];
	}
	return result;
}

/*
 * Apply pitch, temp variations on the generated wavefile.
 */

int
process_pitch_tempo(dhvani_options * options, char *inputfile_name,
		    char *output_filename)
{
	FILE *out, *in;
	int read_count = 0;
	short buffer[CHUNK_LENGTH];
	int callback_ret = 0;
	if (stouch == NULL) {
		if (!(stouch = soundtouch_create(options))) {
			DHVANI_ERROR("Couldnot initialize soundtouch\n");
			return;
		}
	}
	DHVANI_DEBUG("Pitch = %f\tTempo = %f\tRate = %d\n",
		     options->pitch, options->tempo, options->rate);
	SoundTouch_setSampleRate(stouch, options->rate);
	SoundTouch_setPitchSemiTonesFloat(stouch, options->pitch);
	SoundTouch_setTempoChange(stouch, options->tempo);
	in = fopen(inputfile_name, "r");

	if (in <= 0) {
		/*DHVANI_ERROR("File Read error %s\n", inputfile_name);*/
		return;
	}
	if (options->audio_callback_fn == NULL) {
		out = fopen(output_filename, "a");
		if (out < 0) {
			DHVANI_ERROR("File access error %s\n", output_filename);
			return;
		}
	}
	while (1) {
		read_count = fread(buffer, sizeof(short), CHUNK_LENGTH, in);
		if (read_count == -1) {
			DHVANI_ERROR("Read error %s\n", inputfile_name);
			break;
		}
		if (read_count == 0) {
			break;
		}

		SoundTouch_putSamples(stouch, buffer, read_count);
		memset(buffer, 0, read_count);

		/* Read ready samples from SoundTouch processor & write them output file.
		 * NOTES:
		 * - 'receiveSamples' doesn't necessarily return any samples at all
		 *   during some rounds!
		 * - On the other hand, during some round 'receiveSamples' may have more
		 *   ready samples than would fit into 'sampleBuffer', and for this reason 
		 *   the 'receiveSamples' call is iterated for as many times as it
		 *   outputs samples.
		 */
		do {
			read_count =
			    SoundTouch_receiveSamplesEx(stouch, buffer,
							CHUNK_LENGTH);
			if (options->audio_callback_fn == NULL) {
				fwrite(buffer, sizeof(short), read_count, out);
			} else {
				callback_ret =
				    (options->audio_callback_fn) (buffer,
								  read_count);
				if (callback_ret == 1) {
					return -1;
				}
			}
		}
		while (read_count != 0);
	};
	/* Now the input file is processed, yet 'flush' few last samples that are
	 * hiding in the SoundTouch's internal processing pipeline.
	 */
	SoundTouch_flush(stouch);
	do {
		read_count =
		    SoundTouch_receiveSamplesEx(stouch, buffer, CHUNK_LENGTH);
		if (options->audio_callback_fn == NULL) {
			fwrite(buffer, sizeof(short), read_count, out);
		} else {
			callback_ret =
			    (options->audio_callback_fn) (buffer, read_count);
			if (callback_ret == 1) {
				return -1;
			}
		}
	}
	while (read_count != 0);

	fclose(in);
	if (options->audio_callback_fn == NULL) {
		fclose(out);
	}
	return DHVANI_OK;
}

int
process_pitch_and_tempo(dhvani_options * options, short *inputdata,
			int input_length, short **outputdata)
{
	int read_count = 0;
	short buffer[CHUNK_LENGTH];
	int callback_ret = 0;
	int outdata_length = 0;
	if (stouch == NULL) {
		if (!(stouch = soundtouch_create(options))) {
			DHVANI_ERROR("Couldnot initialize soundtouch\n");
			return;
		}
	}
	DHVANI_DEBUG("Pitch = %f\tTempo = %f\tRate = %d\n",
		     options->pitch, options->tempo, options->rate);
	SoundTouch_setSampleRate(stouch, options->rate);
	SoundTouch_setPitchSemiTonesFloat(stouch, options->pitch);
	SoundTouch_setTempoChange(stouch, options->tempo);

	if (input_length <= 0) {
		DHVANI_ERROR("No data in the input %d\n", input_length);
		return 0;
	}
	SoundTouch_putSamples(stouch, inputdata, input_length);
	/* Read ready samples from SoundTouch processor & write them output file.
	 * NOTES:
	 * - 'receiveSamples' doesn't necessarily return any samples at all
	 *   during some rounds!
	 * - On the other hand, during some round 'receiveSamples' may have more
	 *   ready samples than would fit into 'sampleBuffer', and for this reason 
	 *   the 'receiveSamples' call is iterated for as many times as it
	 *   outputs samples.
	 */
	do {
		read_count =
		    SoundTouch_receiveSamplesEx(stouch, buffer, CHUNK_LENGTH);
		*outputdata =
		    shortcat(*outputdata, outdata_length, buffer, read_count);
		outdata_length += read_count;
	}
	while (read_count != 0);
	/* Now the input file is processed, yet 'flush' few last samples that are
	 * hiding in the SoundTouch's internal processing pipeline.
	 */
	SoundTouch_flush(stouch);
	do {
		read_count =
		    SoundTouch_receiveSamplesEx(stouch, buffer, CHUNK_LENGTH);
		*outputdata =
		    shortcat(*outputdata, outdata_length, buffer, read_count);
		outdata_length += read_count;
	}
	while (read_count != 0);

	return outdata_length;
}

#endif
