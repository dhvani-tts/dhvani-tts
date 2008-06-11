/*
 * languages.c Copyright (C) 2007-2008 Santhosh Thottingal
 * <santhosh.thottingal@gmail.com>, Swathanthra Malayalam Computing. This program
 * is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your
 * option) any later version. This program is distributed in the hope
 * that it will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU General Public License for more details. You
 * should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "languages.h"

int
detect_language(unsigned short unicode) {
    if (unicode >= 0x0D00 && unicode <= 0x0D6F) {
        // printf ("MALAYALAM\n");
        return MALAYALAM;
    }
    if (unicode >= 0x0901 && unicode <= 0x097D) {
        // printf ("HINDI\n");
        return HINDI;
    }
    if (unicode >= 0x0C82 && unicode <= 0x0CEF) {
        // printf ("KANNADA\n");
        return KANNADA;
    }
    if (unicode >= 0x0B01 && unicode <= 0x0B71) {
        // printf ("ORIYA\n");
        return ORIYA;
    }
    if (unicode >= 0x0A81 && unicode <= 0x0AF1) {
        // printf ("GUJARATI\n");
        return GUJARATI;
    }
    if (unicode >= 0x0C01 && unicode <= 0x0C6F) {
        // printf ("TELUGU\n");
        return TELUGU;
    }

    if (unicode >= 0x0A01 && unicode <= 0x0A74) {
        // printf ("PANJABI\n");
        return PANJABI;
    }
    if (unicode >= 0x0981 && unicode <= 0x09FA) {
        // printf ("BENGALI\n");
        return BENGALI;
    }
    if (unicode >= 0x0B82 && unicode <= 0x0BFA) {
        // printf ("TAMIL\n");
        return TAMIL;
    }
    // printf ("Unknown Language \n");
    return -1;
}

int
get_language_code(char *lang_option) {
    if (strcmp(lang_option, "ml"))
        return MALAYALAM;
    if (strcmp(lang_option, "hi"))
        return HINDI;
    if (strcmp(lang_option, "ta"))
        return TAMIL;
    if (strcmp(lang_option, "or"))
        return ORIYA;
    if (strcmp(lang_option, "kn"))
        return KANNADA;
    if (strcmp(lang_option, "gu"))
        return GUJARATI;
    if (strcmp(lang_option, "te"))
        return TELUGU;
    if (strcmp(lang_option, "pa"))
        return PANJABI;
    if (strcmp(lang_option, "bn"))
        return BENGALI;


    // printf ("Unknown Language %s\n", lang_option);

    exit(0);
}
