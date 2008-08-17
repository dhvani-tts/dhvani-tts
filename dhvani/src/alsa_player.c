


/* alsa_player.c
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
#include <unistd.h>

#include "alsa_player.h"
#include "synthesizer.h"
#include "debug.h"
static char *device = "default";

/*-----------------------------------------------------------------------
Initialise ALSA and sets the sampling rate and audio format.
--------------------------------------------------------------------------*/
snd_pcm_t*
alsa_init() {
    int rc;
    snd_pcm_t *handle; /* the variable for the sound device,  */
    snd_pcm_hw_params_t *params;
    unsigned int val;
    int dir;
    snd_pcm_uframes_t frames;

    /* Open PCM device for playback. */
    rc = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        dhvani_error( "unable to open pcm device: %s\n", snd_strerror(rc));
        exit(1);
    }

    /* Allocate a hardware parameters object. */
    snd_pcm_hw_params_alloca(&params);

    /* Fill it in with default values. */
    snd_pcm_hw_params_any(handle, params);

    /* Set the desired hardware parameters. */

    /* Interleaved mode */
    snd_pcm_hw_params_set_access(handle, params,
            SND_PCM_ACCESS_RW_INTERLEAVED);

    /* Signed 16-bit little-endian format */
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

    /* Two channels (stereo) */
    snd_pcm_hw_params_set_channels(handle, params, 2);

    /* sampling rate. effective sampling rate= channels * rate= 16000 KHz */
    val = 8000;
    /*output format 4000*speed*rate 16 Bit little endian 16000 KHz*/
    snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

    /* Set period size to 32 frames. */
    frames = 32;
    snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

    /* Write the parameters to the driver */
    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) {
        dhvani_error("unable to set hw parameters: %s\n", snd_strerror(rc));
        exit(1);
    }

    return handle;
}

void
alsa_play(FILE *wave_file, snd_pcm_t *handle) {

    int rfd;
    int rc;
    char *buffer;
    rfd = open(wave_file, O_RDONLY);
    if (rfd < 0) {
        dhvani_error("File Read error\n");
    }
 
    buffer = (char *) malloc(32 * 4);
    while (1) {

        rc = read(rfd, buffer, 32 * 4);
        if (rc == -1) {
            dhvani_error("ALSA Read error\n");
            break;
        }
        if (rc == 0) {
            break;
        }

        rc = snd_pcm_writei(handle, buffer, 32);

        if (rc == -EPIPE) {
            /* EPIPE means underrun */
            dhvani_debug( "underrun occurred\n");
            snd_pcm_prepare(handle);
        } else if (rc < 0) {
            dhvani_error("error from writei: %s\n", snd_strerror(rc));
        } else if (rc != 32) {
            dhvani_error("short write, write %d frames\n", rc);
        }

    }
 
    
    close(rfd);
    remove(wave_file);/*Played, so delete the file*/
    free(buffer);

}
/*
Close the alsa handler
*/
snd_pcm_t* alsa_close(snd_pcm_t *handle){
        if(handle!=NULL){
            return snd_pcm_close(handle);
        }
        return 0;
}
