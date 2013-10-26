/* phonetic_synthesizer_ml.c
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

#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<stdlib.h>
#include<stdio.h>
#include "phonetic_synthesizer_ml.h"
#include "debug.h"

/*-----------------------    FUNCTIONS     ----------------------------*/

void ml_readfile();
int ml_replace(unsigned short *s, int size);
char *ml_replacenum(unsigned short *s, int size);
char *ml_parsenum(char *numstr);
char *ml_spellNumbers(unsigned short *word, int start, int end);
char *ml_parseMiscellaneous(unsigned short *s, int size);
char *ml_parseword(int);
char *ml_getvowel(char *t);

/*-----------------------------------------------------------------------*/

int result;

struct wordtemplate {
	int type;
	char *letter;
};
struct wordtemplate word[100];
char *available_halfs[] =
    { "ky", "kr", "kl", "kll", "kv", "ksh", "khy", "khr", "khl", "khv",
	"gy",
	"gr", "gl", "gv", "gn", "ghy", "ghr", "ghv", "ghn", "chy", "chr",
	"chv", "jy", "jv",
	"ty", "tr", "tv", "thy", "thr", "dy", "dr", "dv", "dhy", "dhr", "dhv",
	"ny", "nr", "nv",
	"tty", "ttr", "ttv", "ddy", "ddr", "ddv", "py", "pr", "pl", "pll",
	"fr", "fl", "by",
	"br", "bl", "bhy", "bhr", "bhl", "my", "mr", "vy", "vr", "vl"
};

/*-----------------------------------------------------------------------*/

/*------------------------------------------------------------------------

ml_replace accepts an array of unicode characters as input, and 
ml_replaces them by the corresponding phonetic strings.

 
------------------------------------------------------------------------*/

int ml_replace(unsigned short *s, int size)
{
	int i, j;

	i = 0;
	j = 0;
	while (i < size) {
		switch (s[i]) {
		case 0x0D02:	/*am */
			if (j >= 1 && word[j - 1].type != 0) {

				word[j].letter = "1";
				word[j].type = 0;
				j++;
			}

			word[j].letter = "m";
			word[j].type = 0;
			j++;

			break;

		case 0x0D03:	/*ah */

			word[j].letter = "h";
			word[j].type = 1;
			j++;

			break;
			/*--------------VOWELS-----------------*/

		case 0x0D05:	/* a 1 */

			word[j].letter = "1";
			word[j].type = 0;
			j++;
			break;

		case 0x0D06:	/* A 2 */

			word[j].letter = "2";
			word[j].type = 0;
			j++;
			break;

		case 0x0D07:	/*i 3 */

			word[j].letter = "3";
			word[j].type = 0;
			j++;
			break;

		case 0x0D08:	/*I 4 */

			word[j].letter = "4";
			word[j].type = 0;
			j++;
			break;
		case 0x0D09:	/* u 5 */

			word[j].letter = "5";
			word[j].type = 0;
			j++;
			break;
		case 0x0D0A:	/* U 6 */

			word[j].letter = "6";
			word[j].type = 0;
			j++;
			break;

		case 0x0D0B:	/*ri by r3 */

			word[j].type = 1;
			word[j++].letter = "r";
			word[j].type = 0;
			word[j++].letter = "c5";
			break;

		case 0x0D0E:	/*E 7 */

			word[j].letter = "7";
			word[j].type = 0;
			j++;
			break;

		case 0x0D0F:	/* EE by 8 */

			word[j].letter = "8";
			word[j].type = 0;
			j++;
			break;

		case 0x0D10:	/*ai by 9 */

			word[j].letter = "10";
			word[j].type = 0;
			j++;
			break;

		case 0x0D12:	/*otilda by 11 */

			word[j].letter = "11";
			word[j].type = 0;
			j++;
			break;

		case 0x0D13:	/*o by 12 */

			word[j].letter = "12";
			word[j].type = 0;
			j++;
			break;

		case 0x0D14:	/*au by 13 */

			word[j].letter = "13";
			word[j].type = 0;
			j++;
			break;

			/*--------------MATRAS-----------------*/
		case 0x0D3E:	/*2 */
			word[j].letter = "c2";
			word[j].type = 0;
			j++;
			break;

		case 0x0D3F:	/*3 */
			word[j].letter = "c3";
			word[j].type = 0;
			j++;
			break;

		case 0x0D40:	/*4 */

			word[j].letter = "c4";
			word[j].type = 0;
			j++;
			break;
		case 0x0D41:	/*5 */
			if (i <= s[i - 1] && s[i + 1] == 0x0D4D) {
				word[j].letter = "c15";	/*emphasis at the end */
				word[j].type = 0;
				j++;
				i++;
			} else {
				word[j].letter = "c5";
				word[j].type = 0;
				j++;
			}
			break;

		case 0x0D42:	/*6 */

			word[j].letter = "c6";
			word[j].type = 0;
			j++;
			break;

		case 0x0D43:	/*hr by r3 */

			word[j].letter = "r";
			word[j].type = 1;
			j++;
			word[j].letter = "c3";
			word[j].type = 0;
			j++;

			break;

		case 0x0D46:	/*etilda by 7 */

			word[j].letter = "c7";
			word[j].type = 0;
			j++;
			break;

		case 0x0D47:	/*e by 8 */

			word[j].letter = "c8";
			word[j].type = 0;
			j++;
			break;

		case 0x0D48:	/*ai by 9 */

			word[j].letter = "c10";
			word[j].type = 0;
			j++;
			break;

		case 0x0D4A:	/*otilda by 11 */

			word[j].letter = "c11";
			word[j].type = 0;
			j++;
			break;

		case 0x0D4B:	/*o by 12 */

			word[j].letter = "c12";
			word[j].type = 0;
			j++;
			break;

		case 0x0D4C:	/*au by 14 */

			word[j].letter = "c14";
			word[j].type = 0;
			j++;
			break;

		case 0x0D57:	/*au by 14 */

			word[j].letter = "c14";
			word[j].type = 0;
			j++;
			break;

		case 0x0D4D:	/*virama */
			if (i == size - 1 || (s[i + 1] == 0x2E || s[i + 1] == 0x20)) {	/*endng character */
				word[j].letter = "c15";	/*emphasis at the end */
				word[j].type = 0;
				j++;
			} else {
				/*any conjuct or ZWJ just pronounce half part. */
				word[j].letter = "HH";
				word[j].type = 2;
				j++;
			}
			break;

			/*--------------CONSONANTS-----------------*/

		case 0x0D15:	/*k */

			word[j].letter = "k";
			word[j].type = 1;
			j++;
			break;

		case 0x0D16:	/*kh */

			word[j].letter = "kh";
			word[j].type = 1;
			j++;
			break;

		case 0x0D17:	/*g */

			word[j].letter = "g";
			word[j].type = 1;
			j++;
			break;

		case 0x0D18:	/*gh */

			word[j].letter = "gh";
			word[j].type = 1;
			j++;
			break;

		case 0x0D19:	/*nga -Not exactly correct pronounciation */

			word[j].letter = "n";
			word[j].type = 1;
			j++;

			break;

		case 0x0D1A:	/*ch */

			word[j].letter = "ch";
			word[j].type = 1;
			j++;
			break;

		case 0x0D1B:	/*chh */

			word[j].letter = "chh";
			word[j].type = 1;
			j++;
			break;

		case 0x0D1C:	/*j */

			word[j].letter = "j";
			word[j].type = 1;
			j++;
			break;

		case 0x0D1D:	/*jh */

			word[j].letter = "jh";
			word[j].type = 1;
			j++;
			break;

		case 0x0D1E:	/*nja */
			if (i <= size - 1 && s[i + 1 == 0xD4D]
			    && s[i + 1 == 0xD1E]) {
				i += 2;
			}	/* ഞ്ഞ എന്നതിന് വേണ്ടി ഞ് ഞ് എന്ന പറയാതിരിക്കാന്‍ */
			word[j].letter = "n";
			word[j].type = 1;
			j++;

			word[j].letter = "HH";
			word[j].type = 2;
			j++;

			word[j].letter = "y";
			word[j].type = 1;
			j++;
			break;

		case 0x0D1F:	/*tt */

			word[j].letter = "tt";
			word[j].type = 1;
			j++;
			break;

		case 0x0D20:	/*tth */

			word[j].letter = "tth";
			word[j].type = 1;
			j++;
			break;

		case 0x0D21:	/*dd */

			word[j].letter = "dd";
			word[j].type = 1;
			j++;
			break;

		case 0x0D22:	/*ddh */

			word[j].letter = "ddh";
			word[j].type = 1;
			j++;
			break;

		case 0x0D23:	/*nna */

			word[j].letter = "nna";
			word[j].type = 1;
			j++;

			break;

		case 0x0D24:	/*t */

			word[j].letter = "t";
			word[j].type = 1;
			j++;

			break;

		case 0x0D25:	/*th */

			word[j].letter = "th";
			word[j].type = 1;
			j++;
			break;

		case 0x0D26:	/*d */

			word[j].letter = "d";
			word[j].type = 1;
			j++;
			break;

		case 0x0D27:	/*dh */

			word[j].letter = "dh";
			word[j].type = 1;
			j++;
			break;

		case 0x0D28:	/*n */

			word[j].letter = "n";
			word[j].type = 1;
			j++;
			break;

		case 0x0D2A:	/*p */

			word[j].letter = "p";
			word[j].type = 1;
			j++;
			break;

		case 0x0D2B:	/*f */

			word[j].letter = "f";
			word[j].type = 1;
			j++;
			break;

		case 0x0D2C:	/*b */

			word[j].letter = "b";
			word[j].type = 1;
			j++;
			break;

		case 0x0D2D:	/*bh */

			word[j].letter = "bh";
			word[j].type = 1;
			j++;
			break;
		case 0x0D2E:	/*m */

			word[j].letter = "m";
			word[j].type = 1;
			j++;
			break;

		case 0x0D2F:	/*y */

			word[j].letter = "y";
			word[j].type = 1;
			j++;
			break;

		case 0x0D30:	/*r */

			word[j].letter = "r";
			word[j].type = 1;
			j++;
			break;

		case 0x0D31:	/*R */
			if (i > 1 && s[i - 1] == 0x0D4D) {
				/* handling rra-> tra    */
				word[j].letter = "tt";
				word[j].type = 1;

			} else {
				word[j].letter = "r";
				word[j].type = 1;
			}
			j++;
			break;

		case 0x0D32:	/*l */

			word[j].letter = "l";
			word[j].type = 1;
			j++;
			break;

		case 0x0D33:	/*ll */

			word[j].letter = "ll";
			word[j].type = 1;
			j++;
			break;

		case 0x0D34:	/*ll */

			word[j].letter = "zh";
			word[j].type = 1;
			j++;
			break;

		case 0x0D35:	/*v */

			word[j].letter = "v";
			word[j].type = 1;
			j++;
			break;

		case 0x0D36:	/*sh */

			word[j].letter = "sh";
			word[j].type = 1;
			j++;
			break;

		case 0x0D37:	/*sh */

			word[j].letter = "sh";
			word[j].type = 1;
			j++;
			break;

		case 0x0D38:	/*s */

			word[j].letter = "s";
			word[j].type = 1;
			j++;
			break;

		case 0x0D39:	/*h */

			word[j].letter = "h";
			word[j].type = 1;
			j++;
			break;

		}

		++i;
	}

	return (j);
}

/*-------------------------------------------------------------------------

ml_process takes as input a short array and first determines whether it is 
a word  or a number. 

If it is a word it first calls ml_replace to get the phonetic symbols and 
followed by ml_parseword to get the phonetic description of the word. 

If it is a number, it calls ml_replacenum to get the phonetic symbols and then 
ml_parsenum to get the phonetic description of the number.

It then outputs the phonetic description.

--------------------------------------------------------------------------*/

char *generate_phonetic_script_ml(unsigned short *word, int size)
{

	char *final, *script = NULL, *number = NULL, *spell_num = NULL;
	int arrsz;
	int i = 0;

	final = (char *)malloc(100 * sizeof(char));
	final[0] = '\0';
	/*check for number. 0x0D66-0x0D6F : Malayalam numbers
	   0x30 to 0x39 : ASCII numbers */
	if ((word[0] >= 0x0D66 && word[0] <= 0x0D6F)
	    || (word[0] >= 0x30 && word[0] <= 0x39)) {	/*  Number ?  */
		/*dashamsam (0x2E)   to be handled here */
		for (i = 0; i < size; i++) {
			if (word[i] == 0x2E)
				break;
		}
		if (i == size || i == size - 1)	//No decimal points
		{
			if (size > 9) {
				/*size >9 ie greater than 1 crore.
				   Assuming it is a phone number or credit card number etc
				   Read each numbers seperately  
				   printf("spell out numbers\n"); */
				spell_num = ml_spellNumbers(word, 0, size);
				final = strcat(final, spell_num);
				free(spell_num); spell_num = NULL;
			} else {	/*construct the number string using ml_parsenum logic
					   //printf("reading numbers\n"); */
				final =
				    strcat(final,
					   ml_parsenum(ml_replacenum
						       (word, size)));
			}
		} else {
			/*Read the number part as usual */
			if (i > 9) {
				/*size >9 i.e. greater than 1 crore.
				  Assuming it is a phone number or credit card number etc
				  Read each numbers seperately  
				  printf("spell out numbers\n"); */
				spell_num = ml_spellNumbers(word, 0, i);
				final = strcat(final, spell_num);
				free(spell_num); spell_num = NULL;
				/*      printf("spell out numbers-over\n"); */
			} else {	/*construct the number string using ml_parsenum logic */
				final =
					strcat(final,
							ml_parsenum(ml_replacenum(word, i)));
				/*     printf("speak numbers%d done\n",i); */
			}
			/*say dashamsham
			  printf("say dashamsham"); */
			final = strcat(final, " d1 sh2m sh1m ");
			/*Now spell out each decimal places */

			spell_num = ml_spellNumbers(word, i + 1, size);
			final = strcat(final, spell_num);
			free(spell_num); spell_num = NULL;
		}
	} else {
		/* Malayalam Word ? */
		arrsz = ml_replace(word, size);	/* Get phonetic description */
		/*arrsz is the size of the phonetic array */

		final = strcat(final, ml_parseword(arrsz));

	}

	/*Miscellaneous support!!!
	   English letters, special symbols etc... */
	script = ml_parseMiscellaneous(word, size);
	final = strcat(final, script);
	free(script); script = NULL;

	/*print the phonetic string produced by this engine to log file.. */
	DHVANI_DEBUG("%s", final);
	return (final);		/* Done!!!  */

}

char *ml_spellNumbers(unsigned short *word, int start, int end)
{
	int i;
	unsigned short *decimal;
	char *final = (char *)malloc(100 * sizeof(char));
	final[0] = '\0';
	decimal = (unsigned short *)malloc(2 * sizeof(unsigned short));

	for (i = start; i < end; i++) {
		decimal[0] = word[i];
		decimal[1] = '\0';
		final = strcat(final, ml_parsenum(ml_replacenum(decimal, 1)));
		final = strcat(final, " G1500 ");
	}

	free(decimal); decimal = NULL;

	return final;
}

char *ml_parseMiscellaneous(unsigned short *s, int size)
{
	char *phoneticScript = (char *)malloc(100 * sizeof(char));
	int i = 0;
	char gap[] = " G3000 ";
	char *englishAlph[26] =
	    { "7", "b3", "s3", "d3", "3", "7f", "j3", "7ch", "1y", "j7", "k3",
"7l",
		"7m", "7n", "11", "p3", "kHy5", "2r", "7s", "tt3", "y5", "v3",
		"d1b l3 y5", "7 k 0s",
		"v1y", "3z 1d"
	};
	phoneticScript[0] = '\0';
	strcat(phoneticScript, " ");
	for (i = 0; i < size; i++) {
		/*English caps letters */
		if (s[i] >= 65 && s[i] <= 90) {
			strcat(phoneticScript, englishAlph[s[i] - 65]);
			strcat(phoneticScript, gap);
		}
		/*             else 
		   English small letters        
		   if (s[i] >= 97 && s[i] <= 122) {
		   strcat(phoneticScript, englishAlph[s[i] - 97]);
		   strcat(phoneticScript, gap);
		   } */
		else {
			if (s[i] == 0x25) {	//percentage - sathamaanam
				strcat(phoneticScript, "s1 t1 m2 n1m");
				strcat(phoneticScript, gap);
			}
			if (s[i] == 0x24) {	//Dollar
				strcat(phoneticScript, "d12 ll1r");
				strcat(phoneticScript, gap);
			}

			if (s[i] == 0x3D) {	//equal to - samam
				strcat(phoneticScript, "s1 m1m");
				strcat(phoneticScript, gap);
			}
			if (s[i] == 0x40) {	//@- at
				strcat(phoneticScript, "1tt");
				strcat(phoneticScript, gap);
			}

		}
	}

	return phoneticScript;
}

/*-------------------------------------------------------------------------

Break the words, produce phnonetic script based
on the type and cv flags.
 

--------------------------------------------------------------------------*/

char *ml_parseword(int last)
{
	int hf, itr, dcr, prevcv, i, cvflag[50], cvcnt;
	char *syllable, *lsyl, *t_half;

	syllable = (char *)malloc(100 * sizeof(char));
	lsyl = (char *)malloc(100 * sizeof(char));
	t_half = (char *)malloc(10 * sizeof(char));

	strcpy(syllable, " ");

	/* store positions of cvs in cvpos array 
	 * cvflag is 1 at a cv 
	 * else 2 
	 * type is 0 for vowel and 1 for a consonant*/
	for (i = 0; i < last + 5; i++) {
		if (i + 1 < last && word[i].type == 1 && word[i + 1].type == 0) {	//A CV
			cvflag[i] = 1;
			cvflag[i + 1] = 2;
			i++;
		} else {
			cvflag[i] = 0;
		}
	}

	for (i = last - 1; i >= 0; i--) {	//right to left
		dcr = 0;
		hf = 0;
		if (cvflag[i] == 2) {
			//nothing to do. just concatenate with syllable string
			strcpy(lsyl, "\0");
			strcat(lsyl, ml_getvowel(word[i].letter));
			strcat(lsyl, syllable);
			strcpy(syllable, lsyl);
		} else if (cvflag[i] == 1) {	// a consonant

			//output 'c '
			strcpy(lsyl, word[i].letter);
			strcat(lsyl, syllable);
			strcpy(syllable, lsyl);

			strcpy(lsyl, " ");
			strcat(lsyl, syllable);
			strcpy(syllable, lsyl);

		} else {
			if (i >= 1 && word[i].type == 2
			    && word[i - 1].type == 1) {
				/* there is a cH */

				if (cvflag[i + 1])	//if (cvflag[i + 1] || strcmp (word[i + 1].letter, "1") >= 0)
				{
					strcpy(lsyl, " ");
					/* if cv or c1 follows current conH */
					itr = 0;
					strcpy(t_half, word[i - 1].letter);
					strcat(t_half, word[i + 1].letter);
					while (!hf && itr++ < (no_of_halfs - 1))
						if (!strcmp
						    (available_halfs[itr],
						     t_half))
							hf = 1;
					if (hf) {

						strcpy(lsyl,
						       word[i - 1].letter);
						strcat(lsyl, "H");
						strcat(lsyl, ++syllable);
						strcpy(syllable, lsyl);

						strcpy(lsyl, " ");
						strcat(lsyl, syllable);
						strcpy(syllable, lsyl);

						dcr = 1;
					}
				}
				if (i > 2 && word[i - 2].type == 2
				    && word[i - 3].type == 1
				    && !(cvflag[i - 2] == 2
					 && cvflag[i - 3] == 1) && !hf) {
					/* there is a cH behind cH giving  0c */
					strcpy(lsyl, "0");
					strcat(lsyl, word[i - 1].letter);
					strcat(lsyl, syllable);
					strcpy(syllable, lsyl);

					strcpy(lsyl, " ");
					strcat(lsyl, syllable);
					strcpy(syllable, lsyl);

					dcr = 1;
				} else if (i > 1 && word[i - 2].type == 1
					   && !hf) {
					/* there is a c behind cH giving c1c */
					strcpy(lsyl, word[i - 2].letter);
					strcat(lsyl, "1");
					strcat(lsyl, word[i - 1].letter);
					strcat(lsyl, syllable);
					strcpy(syllable, lsyl);

					strcpy(lsyl, " ");
					strcat(lsyl, syllable);
					strcpy(syllable, lsyl);

					dcr = 2;
				} else if (i > 1 && word[i - 2].type == 0
					   && !hf) {
					/* there is a v behind cH giving vc */
					if (*word[i - 2].letter == 'c') {
						strcpy(lsyl,
						       word[i - 3].letter);
						dcr = 1;
					} else {
						strcpy(lsyl, "");
					}
					strcat(lsyl,
					       ml_getvowel(word[i - 2].letter));
					strcat(lsyl, word[i - 1].letter);
					strcat(lsyl, syllable);
					strcpy(syllable, lsyl);

					strcpy(lsyl, " ");
					strcat(lsyl, syllable);
					strcpy(syllable, lsyl);

					dcr = 2 + dcr;
				} else if (i == 1 && word[i].type == 2
					   && word[i - 1].type == 1 && !hf) {
					/* There is nothing behind cH hence 0c */
					strcpy(lsyl, "0");
					strcat(lsyl, word[i - 1].letter);

					strcat(lsyl, syllable);
					strcpy(syllable, lsyl);

					strcpy(lsyl, " ");
					strcat(lsyl, syllable);
					strcpy(syllable, lsyl);
					dcr = 2;
				}
			} else if (word[i].type == 1 && !hf) {
				/* There is a c. so just op c1 */
				strcpy(lsyl, word[i].letter);
				strcat(lsyl, "1");
				strcat(lsyl, syllable);
				strcpy(syllable, lsyl);

				strcpy(lsyl, " ");
				strcat(lsyl, syllable);
				strcpy(syllable, lsyl);

			} else if (word[i].type == 0 && !hf) {
				/* There is a v. so just op v */
				strcpy(lsyl, ml_getvowel(word[i].letter));
				strcat(lsyl, syllable);
				strcpy(syllable, lsyl);

				strcpy(lsyl, " ");
				strcat(lsyl, syllable);
				strcpy(syllable, lsyl);
			}
		}
		i -= dcr;
	}

	free(lsyl); lsyl = NULL;
	free(t_half); t_half = NULL;

	return (syllable);
}

char *ml_getvowel(char *t)
{
	if (t[0] == 'c') {
		return (++t);
	} else {
		return (t);
	}
}

/*------------------------------------------------------------------------

 Function ml_parsenum takes an array of strings and generates the
 phonetic representation, which can then be played out.

 In malayalam,we can't really specify any rules for pronunciation of 
 the natural numbers upto 100, so one has to explicitly specify 
 the phonetics of each natural number till 100.

 From there on,one can break up the number into units,and use the 
 phonetic symbols specified for no.s < 100,along with the phonetics 
 for the various units.

 We've included units upto 'Koti' ,which is one crore, 

 Conceptually,of course,we can extend this indefinitely.

------------------------------------------------------------------------*/

char *ml_parsenum(char *numstr)
{

	char *singles[] =
	    { "p6j y1m", "11n n15", "r1n dd15", "m6n n15", "n2l l15", "1n ch15",
		"2r r15",
		"8 zh15", "7t tt15", "11m p1 t15"
	};
	char *teens[] =
	    { "p1t t15", "p1t t3n 11n n15", "p1n d1 r1n dd15", "p1 t3 m6 n15",
		"p1 t3 n2 l15", "p1 t3 n1n ch15", "p1 t3 n2r r15",
		    "p1 t3 n8 zh15",
		"p1 t3 n7t t15",
		"p1 t11m p1 t15"
	};
	char *tens[] =
	    { " ", " ", "3 r5 p1 t", "m5p p1 t", "n1l p1 t", "1m p1 t",
		"1 r5 p1 t", "7 zh5  p1 t", "7m p1 t", "t11n n6 r"
	};
	char *hundreds[] =
	    { "", "n6 r1", "3 r5n n6 r1", "m5n n6 r1", "n2 n6 r1", "10n n6 r1",
		"1 r5 n6 r1", "8 zh5 n6 r1", "7n n6 r1", "t11l l2y 3r1 m"
	};

	int i, inr;
	int place;
	char *number, *tmp;
	place = strlen(numstr);
	number = (char *)malloc(100 * sizeof(char));
	tmp = (char *)malloc(20 * sizeof(char));
	number[0] = '\0';
	for (i = 0; i < place; i++) {
		inr = 0;
		if (place != 1)
			while (numstr[i] == '0')
				i++;	/* remove leading spaces */
		if (place - i == 9) {
			strncpy(tmp, numstr + i, 2);
			tmp[2] = '\0';
			strcat(number, ml_parsenum(tmp));
			strcat(number, " k12 tt3 ");
			inr = 1;
		} else if (place - i == 8) {

			strcat(number, singles[numstr[i] - 0]);
			strcat(number, " k12 tt3 ");
		} else if (place - i == 7) {
			strncpy(tmp, numstr + i, 2);
			tmp[2] = '\0';
			strcat(number, ml_parsenum(tmp));
			if (numstr[i + 2] == '0' && numstr[i + 3] == '0'
			    && numstr[i + 4] == '0' && numstr[i + 5] == '0'
			    && numstr[i + 6] == '0') {
				strcat(number, " l1 kHsh1 m");
				inr = 6;
			} else {
				strcat(number, " l1 kHsh1 th3 ");
				inr = 1;
			}
		} else if (place - i == 6) {
			if (numstr[i + 1] == '0' && numstr[i + 2] == '0'
			    && numstr[i + 3] == '0' && numstr[i + 4] == '0'
			    && numstr[i + 5] == '0') {
				strcat(number, singles[numstr[i] - 0]);
				strcat(number, " l1 kHsh1 m");
			} else {
				strcat(number, singles[numstr[i] - 0]);
				strcat(number, " l1 kHsh1 th3 ");
			}
		} else if (place - i == 5) {

			strncpy(tmp, numstr + i, 2);
			tmp[2] = '\0';
			strcat(number, ml_parsenum(tmp));
			if (numstr[i + 2] == '0' && numstr[i + 3] == '0'
			    && numstr[i + 4] == '0') {

				if ((numstr[i] - 0 == 9 || numstr[i + 1] != '0') && numstr[i + 1] - 0 != 9)	//90,000 is thonnorayiram , not thonnoorinayiram
				{
					number[strlen(number) - 3] = '2';
					number[strlen(number) - 2] = ' ';
					strcat(number, "y3 r1 m ");
				} else {
					number[strlen(number) - 3] = '3';
					number[strlen(number) - 2] = ' ';
					strcat(number, "n2 y3 r1 m ");	//--inayiram - pathinaayiram , iruapathinaayiram etc..
				}

				inr = 3;
			} else {
				if (number[strlen(number) - 2] == '5'
				    && number[strlen(number) - 3] == '1') {
					number[strlen(number) - 3] = '2';
					number[strlen(number) - 2] = ' ';
					strcat(number, "y3 r1 t3 ");
				} else
					strcat(number, "2 y3 r1 t3 ");
				inr = 1;
			}
		} else if (place - i == 4) {
			if (numstr[i + 1] == '0' && numstr[i + 2] == '0'
			    && numstr[i + 3] == '0') {
				if (numstr[i] - 0 == 1) {
					strcat(number, "2 y3 r1 m ");	//for aayiram instead of onnayiram

				} else if (numstr[i] - 0 == 9) {
					strcat(number, "11m p1 t3 n2 y3 r1 m ");	//for ompathinaayiram instead of ompathaayiram

				} else {
					strcat(number, singles[numstr[i] - 0]);
					strcat(number, " 2 y3 r1 m");
					inr = 1;
				}
			} else {
				if (numstr[i] - 0 == 1) {
					strcat(number, " 2 y3 r1 t3 ");	//for aayirathi instead of onnayirathi
				} else if (numstr[i] - 0 == 9) {
					strcat(number, "11m p1 t3 n2 y3 r1 t3 ");	//for ompathinaayirathi instead of ompathaayirayhi
				} else {
					strcat(number, singles[numstr[i] - 0]);
					strcat(number, " 2 y3 r1 t3 ");
				}
			}
		} else if (place - i == 3) {
			if (numstr[i + 1] == '0' && numstr[i + 2] == '0') {
				strcat(number, hundreds[numstr[i] - 0]);
				inr = 2;
			} else if (numstr[i + 1] == '0') {
				strcat(number, hundreds[numstr[i] - 0]);
				number[strlen(number) - 1] = '3';
				number[strlen(number) - 2] = 't';
				inr = 1;
			} else {
				strcat(number, hundreds[numstr[i] - 0]);
				number[strlen(number) - 1] = '3';
				number[strlen(number) - 2] = 't';
			}
			strcat(number, " ");
		} else if (place - i == 2) {
			if (numstr[i] - 0 == 1) {
				if (numstr[i + 1] == '0')
					strcat(number, teens[0]);
				else
					strcat(number,
					       teens[numstr[i + 1] - 0]);
				inr = 1;
			} else if (numstr[i + 1] == '0') {
				strcat(number, tens[numstr[i] - 0]);

				strcat(number, "15");	//irupath ,mupath, nalpath..
				inr = 1;
			} else {
				strcat(number, tens[numstr[i] - 0]);
				if (numstr[i] - 0 == 9) {
					number[strlen(number) - 1] = 't';	// for thonnoorh
				}
				strcat(number, "3");	//irupathi ,mupathi, nalpathi..
			}
			strcat(number, " ");
		} else if (place - i == 1) {
			if (numstr[i] == '0')
				strcat(number, singles[0]);
			else
				strcat(number, singles[numstr[i] - 0]);
			strcat(number, " ");
		}
		i += inr;
		//strcat (number, " G1500 ");
	}

	free(tmp); tmp = NULL;

	return (number);
}

/*-----------------------------------------------------------------------

   Function ml_replacenum - takes as input a unicode array, assigns the
   corresponding phonetic string to an array of integers and returns
   the size of that array, ie no. of digits in the correponding no.

------------------------------------------------------------------------*/

char *ml_replacenum(unsigned short *s, int size)
{
	int i, j;
	char *numchar;
	i = 0;

	numchar = (char *)malloc(size * sizeof(char));
	while (i < size) {
		if (s[i] >= 0x0D66 && s[i] <= 0x0D6F)	/* if in range of malayalam numbers */
			numchar[i] = ((s[i] - 0x0D66) == '\0') ? '0' : (s[i] - 0x0D66);	/* subtract offset */
		if (s[i] >= 0x30 && s[i] <= 0x39)	/* if in range of ascii numbers */
			numchar[i] = ((s[i] - 0x30) == '\0') ? '0' : (s[i] - 0x30);	/* subtract offset */

		i++;
	}
	numchar[i] = '\0';
	return (numchar);

}
