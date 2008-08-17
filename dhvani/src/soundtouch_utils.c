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
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <soundtouch4c.h>
#include "soundtouch_utils.h"
#include "dhvani_lib.h"
#include "debug.h"

struct soundtouch *stouch=NULL;
/*Initialize the soundtouch processor*/
 struct soundtouch *
soundtouch_create(dhvani_options *options)
{
        struct soundtouch *snd;

        dhvani_debug("Creating SoundTouch object...\n");
        snd = SoundTouch_construct();
        if (!snd) {
                dhvani_error("Failed to create SoundTouch object\n");
                return NULL;
        }
  
        SoundTouch_setChannels(snd, 1);
        SoundTouch_setSetting(snd, SETTING_USE_QUICKSEEK, 1);
        SoundTouch_setSetting(snd, SETTING_USE_AA_FILTER, 1);

        return snd;
}
/*Free the soundtouch processor*/
 void
SoundTouch_free( )
{
        dhvani_debug("Freeing SoundTouch object...\n");
        SoundTouch_destruct(stouch);
}

/*
 * Apply pitch, temp variations on the generated wavefile.
 */

void process_sound(dhvani_options *options, char *inputfile_name, char *output_filename)
{
        FILE *out, *in;
        int rc;
        short buffer[32 * 4];
        if (stouch == NULL) {
                if (!(stouch = soundtouch_create(options))) {
                        dhvani_error("Couldnot initialize soundtouch\n");
                        return;
                }
        }
	dhvani_debug("\tPitch = %f\n", options->pitch);
        dhvani_debug("\tTempo = %f\n", options->tempo);
        dhvani_debug("\tRate = %d\n", options->rate);
        SoundTouch_setSampleRate(stouch, options->rate);
        SoundTouch_setPitchSemiTonesFloat(stouch, options->pitch);
        SoundTouch_setTempoChange(stouch, options->tempo);
        in = fopen(inputfile_name, "r");

        if (in < 0) {
                dhvani_error("File Read error %s\n", inputfile_name);
                return;
        }

        out = fopen(output_filename, "a");
        if (out < 0) {
                dhvani_error("File access error %s\n", output_filename);
                return;
        }
        while (1) {
                rc = fread(buffer, sizeof(short), 32 * 4, in);
                if (rc == -1) {
                        dhvani_error("Read error %s\n", inputfile_name);
                        break;
                }
                if (rc == 0) {
                        break;
                }

                SoundTouch_putSamples(stouch, buffer, rc);
                memset(buffer, 0, rc);

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
                        rc = SoundTouch_receiveSamplesEx(stouch, buffer, 32 * 4);
                        fwrite(buffer, sizeof(short), rc, out);
                } while (rc != 0);
        };
        /* Now the input file is processed, yet 'flush' few last samples that are
         * hiding in the SoundTouch's internal processing pipeline.
         */
        SoundTouch_flush(stouch);
        do {
                rc = SoundTouch_receiveSamplesEx(stouch, buffer, 32 * 4);
                fwrite(buffer, sizeof(short), rc, out);
        } while (rc != 0);

        fclose(in);
        fclose(out);
}

