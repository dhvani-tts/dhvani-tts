/* soundtouch_utils.h
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
#ifdef  WITH_SOUNDTOUCH
#ifndef _SOUNDTOUCH_UTILS_H
#define _SOUNDTOUCH_UTILS_H
#include <soundtouch4c.h>
#include "dhvani_lib.h"

struct soundtouch *soundtouch_create(dhvani_options *);
int process_pitch_tempo(dhvani_options *, char *, char *);
void SoundTouch_free();
#endif
#endif
