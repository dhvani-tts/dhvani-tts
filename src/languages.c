/*
 * languages.c 
 * 
 * Copyright (C) 2007-2008 Santhosh Thottingal
 * <santhosh.thottingal@gmail.com>, Swathanthra Malayalam Computing. 
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your
 * option) any later version. 
 * 
 * This program is distributed in the hope
 * that it will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU General Public License for more details. You
 * should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 */

#include<stdlib.h>
#include<string.h>
#include <stdio.h>
#include "languages.h"
#include "debug.h"

dhvani_Languages detect_language(unsigned short *word, int size)
{
	unsigned short unicode;
	int i = 0;
	/*check each letters from left to right */
	while (i < size) {
		unicode = word[i];
		if (unicode >= 0x0D00 && unicode <= 0x0D6F) {
			return MALAYALAM;
		}
		if (unicode >= 0x0901 && unicode <= 0x097D) {
			/* Marathi mr_IN shares the code points of devangari with Hindi */
			if (getenv("LANG") != NULL
			    && strcmp(getenv("LANG"), "mr_IN") == 0) {
				return MARATHI;	/*giving priority to LANG environment variable */
			} else {
				return HINDI;
			}

		}
		if (unicode >= 0x0C82 && unicode <= 0x0CEF) {

			return KANNADA;
		}
		if (unicode >= 0x0B01 && unicode <= 0x0B71) {

			return ORIYA;
		}
		if (unicode >= 0x0A81 && unicode <= 0x0AF1) {

			return GUJARATI;
		}
		if (unicode >= 0x0C01 && unicode <= 0x0C6F) {

			return TELUGU;
		}

		if (unicode >= 0x0A01 && unicode <= 0x0A74) {

			return PANJABI;
		}
		if (unicode >= 0x0981 && unicode <= 0x09FA) {

			return BENGALI;
		}
		if (unicode >= 0x0B82 && unicode <= 0x0BFA) {

			return TAMIL;
		}
		if (unicode >= 0x060B && unicode <= 0xFEFC) {

			return PASHTO;
		}

		i++;
	}
	/* Another attempt to see whether the LANG env variable is a supported language */
	if (getenv("LANG") != NULL) {
		return get_language_code(getenv("LANG"));
	} else {
		return -1;	/*sigh... unknown language... */
	}
}

/*
 *Get the language code from the user. Allow user to enter in an of the standard forms
 *Either langcode or langcode_countrycode
 */
dhvani_Languages get_language_code(char *lang_option)
{
	if (strcmp(lang_option, "ml") == 0 || strcmp(lang_option, "ml_IN") == 0)
		return MALAYALAM;
	if (strcmp(lang_option, "hi") == 0 || strcmp(lang_option, "hi_IN") == 0)
		return HINDI;
	if (strcmp(lang_option, "ta") == 0 || strcmp(lang_option, "ta_IN") == 0)
		return TAMIL;
	if (strcmp(lang_option, "or") == 0 || strcmp(lang_option, "or_IN") == 0)
		return ORIYA;
	if (strcmp(lang_option, "kn") == 0 || strcmp(lang_option, "kn_IN") == 0)
		return KANNADA;
	if (strcmp(lang_option, "gu") == 0 || strcmp(lang_option, "gu_IN") == 0)
		return GUJARATI;
	if (strcmp(lang_option, "te") == 0 || strcmp(lang_option, "te_IN") == 0)
		return TELUGU;
	if (strcmp(lang_option, "pa") == 0 || strcmp(lang_option, "pa_IN") == 0)
		return PANJABI;
	if (strcmp(lang_option, "bn") == 0 || strcmp(lang_option, "bn_IN") == 0)
		return BENGALI;
	if (strcmp(lang_option, "mr") == 0 || strcmp(lang_option, "mr_IN") == 0)
		return MARATHI;
	if (strcmp(lang_option, "ps") == 0 || strcmp(lang_option, "ps_AF") == 0)
		return PASHTO;
	return -1;

}
