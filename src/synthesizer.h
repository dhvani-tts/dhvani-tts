/* synthesizer.h
 *
 * Copyright (C) 2007-2008
 *  Santhosh Thottingal <santhosh.thottingal@gmail.com>.
 *  Ramesh Hariharan <ramesh@csa.iisc.ernet.in> 
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

#ifndef _SYNTHESIZER_H
#define _SYNTHESIZER_H
#include <stdlib.h>
#include "dhvani_lib.h"
int start_synthesizer();
int file_to_speech(FILE *, dhvani_options *);
int text_to_speech(char *, dhvani_options *);
int phoneticfile_to_speech(FILE *, dhvani_options *);
int phonetic_to_speech(char *, dhvani_options *);
#endif				/*end of _SYNTHESIZER_H */
