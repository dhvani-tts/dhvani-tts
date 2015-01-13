/* phonetic_synthesizer_bn.c
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

#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<stdlib.h>
#include<stdio.h>
#include "phonetic_synthesizer_bn.h"
#include "debug.h"

/*-----------------------    FUNCTIONS     ----------------------------*/

void bn_readfile();
char *bn_process(unsigned short *word, int size);
int bn_replace(unsigned short *s, int size);
char *bn_replacenum(unsigned short *s, int size);
char *bn_parsenum(char *numstr);
char *bn_spellNumbers(unsigned short *word, int start, int end);
char *bn_parseMiscellaneous(unsigned short *s, int size);
char *bn_parseword(int);
char *bn_getvowel(char *t);

/*-----------------------------------------------------------------------*/

static int result;

struct wordtemplate {
	int type;
	char *letter;
};
static struct wordtemplate word[100];
char *bn_available_halfs[] =
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

bn_replace accepts an array of unicode characters as input, and 
bn_replaces them by the corresponding phonetic strings.

 
------------------------------------------------------------------------*/

int bn_replace(unsigned short *s, int size)
{
	int i, j;

	i = 0;
	j = 0;
	while (i < size) {
		switch (s[i]) {

		case 0x0982:	/*am */
			if (word[j - 1].type != 0) {

				word[j].letter = "1";
				word[j].type = 0;
				j++;
			}

			word[j].letter = "m";
			word[j].type = 0;
			j++;

			break;

		case 0x0983:	/*ah */

			word[j].letter = "h";
			word[j].type = 1;
			j++;

			break;
		/*--------------VOWELS-----------------*/

		case 0x0985:	/* a 1 */
			word[j].letter = "1";
			word[j].type = 0;
			j++;
			break;

		case 0x0986:	/* A 2 */

			word[j].letter = "2";
			word[j].type = 0;
			j++;
			break;

		case 0x0987:	/*i 3 */

			word[j].letter = "3";
			word[j].type = 0;
			j++;
			break;

		case 0x0988:	/*I 4 */

			word[j].letter = "4";
			word[j].type = 0;
			j++;
			break;
		case 0x0989:	/* u 5 */

			word[j].letter = "5";
			word[j].type = 0;
			j++;
			break;
		case 0x098A:	/* U 6 */

			word[j].letter = "6";
			word[j].type = 0;
			j++;
			break;

		case 0x098B:	/*ri by r3 */

			word[j].type = 1;
			word[j++].letter = "r";
			word[j].type = 0;
			word[j++].letter = "c5";
			break;

		case 0x098F:	/*E 7 */

			word[j].letter = "7";
			word[j].type = 0;
			j++;
			break;

		case 0x0990:	/*ai by 9 */

			word[j].letter = "9";
			word[j].type = 0;
			j++;
			break;

		case 0x0993:	/*otilda by 11 */

			word[j].letter = "11";
			word[j].type = 0;
			j++;
			break;

		case 0x0994:	/*au by 13 */

			word[j].letter = "13";
			word[j].type = 0;
			j++;
			break;

		/*--------------MATRAS-----------------*/

		case 0x09BD:	/*1 */

			word[j].letter = "c1";
			word[j].type = 0;
			j++;
			break;

		case 0x09BE:	/*2 */

			word[j].letter = "c2";
			word[j].type = 0;
			j++;
			break;

		case 0x09BF:	/*3 */

			word[j].letter = "c3";
			word[j].type = 0;
			j++;
			break;

		case 0x09C0:	/*4 */

			word[j].letter = "c4";
			word[j].type = 0;
			j++;
			break;
		case 0x09C1:	/*5 */
			if (i <= s[i - 1] && s[i + 1] == 0x09CD) {
				i++;
			}
			word[j].letter = "c5";
			word[j].type = 0;
			j++;
			break;

		case 0x09C2:	/*6 */

			word[j].letter = "c6";
			word[j].type = 0;
			j++;
			break;

		case 0x09C3:	/*hr by r3 */

			word[j].letter = "r";
			word[j].type = 1;
			j++;
			word[j].letter = "c3";
			word[j].type = 0;
			j++;

			break;

		case 0x09C6:	/*etilda by 7 */

			word[j].letter = "c7";
			word[j].type = 0;
			j++;
			break;

		case 0x09C7:	/*e by 8 */

			word[j].letter = "c8";
			word[j].type = 0;
			j++;
			break;

		case 0x09C8:	/*ai by 9 */

			word[j].letter = "c9";
			word[j].type = 0;
			j++;
			break;

		case 0x09CA:	/*otilda by 11 */

			word[j].letter = "c11";
			word[j].type = 0;
			j++;
			break;

		case 0x09CB:	/*o by 12 */

			word[j].letter = "c12";
			word[j].type = 0;
			j++;
			break;

		case 0x09CC:	/*au by 13 */

			word[j].letter = "c13";
			word[j].type = 0;
			j++;
			break;

		case 0x09CD:	/*virama */
			if (i == size - 1 || (s[i + 1] == 0x2E || s[i + 1] == 0x20))	//endng character
			{
				word[j].letter = "c15";	//emphasis at the end
				word[j].type = 0;
				j++;
			} else {
				//any conjuct or ZWJ just pronounce half part.
				word[j].letter = "HH";
				word[j].type = 2;
				j++;
			}
			break;

		/*--------------CONSONANTS-----------------*/
		case 0x0995:	/*k */

			word[j].letter = "k";
			word[j].type = 1;
			j++;
			break;

		case 0x0996:	/*kh */

			word[j].letter = "kh";
			word[j].type = 1;
			j++;
			break;

		case 0x0997:	/*g */

			word[j].letter = "g";
			word[j].type = 1;
			j++;
			break;

		case 0x0998:	/*gh */

			word[j].letter = "gh";
			word[j].type = 1;
			j++;
			break;

		case 0x0999:	/*nga -Not exactly correct pronounciation */

			word[j].letter = "n";
			word[j].type = 1;
			j++;

			break;

		case 0x099A:	/*ch */

			word[j].letter = "ch";
			word[j].type = 1;
			j++;
			break;

		case 0x099B:	/*chh */

			word[j].letter = "chh";
			word[j].type = 1;
			j++;
			break;

		case 0x099C:	/*j */

			word[j].letter = "j";
			word[j].type = 1;
			j++;
			break;

		case 0x099D:	/*jh */

			word[j].letter = "jh";
			word[j].type = 1;
			j++;
			break;

		case 0x099E:	/*nja */
			if (i <= size - 1 && s[i + 1 == 0xD4D]
			    && s[i + 1 == 0xD1E]) {
				i += 2;
			}	// ഞ്ഞ എന്നതിന് വേണ്ടി ഞ് ഞ് എന്ന പറയാതിരിക്കാന്‍
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

		case 0x099F:	/*tt */

			word[j].letter = "tt";
			word[j].type = 1;
			j++;
			break;

		case 0x09A0:	/*tth */

			word[j].letter = "tth";
			word[j].type = 1;
			j++;
			break;

		case 0x09A1:	/*dd */

			word[j].letter = "dd";
			word[j].type = 1;
			j++;
			break;

		case 0x09A2:	/*ddh */

			word[j].letter = "ddh";
			word[j].type = 1;
			j++;
			break;

		case 0x09A3:	/*nna */

			word[j].letter = "nna";
			word[j].type = 1;
			j++;

			break;

		case 0x09A4:	/*t */

			word[j].letter = "t";
			word[j].type = 1;
			j++;

			break;

		case 0x09A5:	/*th */

			word[j].letter = "th";
			word[j].type = 1;
			j++;
			break;

		case 0x09A6:	/*d */

			word[j].letter = "d";
			word[j].type = 1;
			j++;
			break;

		case 0x09A7:	/*dh */

			word[j].letter = "dh";
			word[j].type = 1;
			j++;
			break;

		case 0x09A8:	/*n */

			word[j].letter = "n";
			word[j].type = 1;
			j++;
			break;

		case 0x09AA:	/*p */

			word[j].letter = "p";
			word[j].type = 1;
			j++;
			break;

		case 0x09AB:	/*f */

			word[j].letter = "f";
			word[j].type = 1;
			j++;
			break;

		case 0x09AC:	/*b */

			word[j].letter = "b";
			word[j].type = 1;
			j++;
			break;

		case 0x09AD:	/*bh */

			word[j].letter = "bh";
			word[j].type = 1;
			j++;
			break;
		case 0x09AE:	/*m */

			word[j].letter = "m";
			word[j].type = 1;
			j++;
			break;

		case 0x09AF:	/*y */

			word[j].letter = "y";
			word[j].type = 1;
			j++;
			break;

		case 0x09B0:	/*r */

			word[j].letter = "r";
			word[j].type = 1;
			j++;
			break;

		case 0x09B1:	/*R */
			if (i + 2 <= size && s[i + 2] == 0x09B1
			    && s[i + 1] == 0x09CD) {
				// handling rra-> tra    
				word[j].letter = "t";
				word[j].type = 1;
				j++;
				word[j].letter = "HH";
				word[j].type = 2;
				j++;
				word[j].letter = "r";
				word[j].type = 1;
				i = i + 2;
			} else {
				word[j].letter = "r";
				word[j].type = 1;
			}
			j++;
			break;

		case 0x09B2:	/*l */

			word[j].letter = "l";
			word[j].type = 1;
			j++;
			break;

		case 0x09B5:	/*v */

			word[j].letter = "v";
			word[j].type = 1;
			j++;
			break;

		case 0x09B6:	/*sh */

			word[j].letter = "sh";
			word[j].type = 1;
			j++;
			break;

		case 0x09B7:	/*sh */

			word[j].letter = "sh";
			word[j].type = 1;
			j++;
			break;

		case 0x09B8:	/*s */

			word[j].letter = "s";
			word[j].type = 1;
			j++;
			break;

		case 0x09B9:	/*h */

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

bn_process takes as input a short array and first determines whether it is 
a word  or a number. 

If it is a word it first calls bn_replace to get the phonetic symbols and 
followed by bn_parseword to get the phonetic description of the word. 

If it is a number, it calls bn_replacenum to get the phonetic symbols and then 
bn_parsenum to get the phonetic description of the number.

It then outputs the phonetic description.

--------------------------------------------------------------------------*/

char *generate_phonetic_script_bn(unsigned short *word, int size)
{
    char *final, *misc = NULL, *number = NULL, *spell_num = NULL;
	int arrsz;
	int i = 0;
	;
	final = (char *)malloc(100 * sizeof(char));
	final[0] = '\0';
	//check for number. 0x0B66-0x0B6F : Bengali numbers
	//0x30 to 0x39 : ASCII numbers
	if ((word[0] >= 0x09E6 && word[0] <= 0x09EF)
	    || (word[0] >= 0x30 && word[0] <= 0x39)) {	/*  Number ?  */
		//dashamsam (0x2E)   to be handled here
		for (i = 0; i < size; i++) {
			if (word[i] == 0x2E)
				break;
		}
		if (i == size || i == size - 1)	//No decimal points
		{
			if (size > 9) {
				//size >9 ie greater than 1 crore.
				//Assuming it is a phone number or credit card number etc
				//Read each numbers seperately  
				//      printf("spell out numbers\n");
				spell_num = bn_spellNumbers(word, 0, size);
				final = strcat(final, spell_num);
				free(spell_num); spell_num = NULL;
			} else {	//construct the number string using bn_parsenum logic
				//printf("reading numbers\n");
				number = bn_parsenum(bn_replacenum(word, size));
				final = strcat(final, number);
				free(number); number = NULL;
			}
		} else {
			//Read the number part as usual
			if (i > 9) {
				//size >9 i.e. greater than 1 crore.
				//Assuming it is a phone number or credit card number etc
				//Read each numbers seperately  
				//      printf("spell out numbers\n");
				spell_num = bn_spellNumbers(word, 0, i);
				final = strcat(final, spell_num);
				free(spell_num); spell_num = NULL;
				//      printf("spell out numbers-over\n");
			} else {	//construct the number string using bn_parsenum logic
				//      printf("speak numbers%d\n",i);
				number = bn_parsenum(bn_replacenum(word, i));
				final = strcat(final, number);
				free(number); number = NULL;
				//      printf("speak numbers%d done\n",i);
			}
			//say dashamsham
			//        printf("say dashamsham");
			final = strcat(final, " d1 sh2m sh1m ");
			//Now spell out each decimal places
			spell_num = bn_spellNumbers(word, i + 1, size);
			final = strcat(final, spell_num);
			free(spell_num); spell_num = NULL;
		}
	} else {
		/* Bengali Word ? */
		arrsz = bn_replace(word, size);	/* Get phonetic description */
		//arrsz is the size of the phonetic array

		final = strcat(final, bn_parseword(arrsz));

	}

	//Miscellaneous support!!!
	//English letters, special symbols etc...
	misc = bn_parseMiscellaneous(word, size);
	final = strcat(final, misc);
	free(misc); misc = NULL;

	//print the phonetic string produced by this engine to stdout..
	DHVANI_DEBUG("%s", final);
	return (final);		/* Done!!!  */
}

char *bn_spellNumbers(unsigned short *word, int start, int end)
{
	int i;
	char *final = (char *)malloc(100 * sizeof(char)), *num = NULL;
	char *numchar = NULL;
	final[0] = '\0';
	unsigned short  *decimal = (unsigned short *)malloc(2 *
			sizeof(unsigned short));
	
	for (i = start; i < end; i++) {
		decimal[0] = word[i];
		decimal[1] = '\0';
		numchar = bn_replacenum(decimal, 1);
		num = bn_parsenum(numchar);
		final = strcat(final, num);
		final = strcat(final, " G1500 ");

		free(numchar); numchar = NULL;
		free(num); num = NULL;
	}

	free(decimal); decimal = NULL;

	return final;
}

char *bn_parseMiscellaneous(unsigned short *s, int size)
{
	char *phoneticScript = (char *)malloc(100 * sizeof(char));
	int i = 0;
	char gap[] = " G3000 ";
	char *englishAlph[26] =
	    { "7", "9B", "s3", "d3", "3", "7f", "j3", "7ch", "1y", "j7", "k3",
"7l",
		"7m", "7n", "11", "p3", "kHy5", "2r", "7s", "tt3", "y5", "v3",
		"d1b l3 y5", "7 k 0s",
		"v1y", "3z 1d"
	};
	phoneticScript[0] = '\0';
	strcat(phoneticScript, " ");
	for (i = 0; i < size; i++) {
		//English caps letters 
		if (s[i] >= 65 && s[i] <= 90) {
			strcat(phoneticScript, englishAlph[s[i] - 65]);
			strcat(phoneticScript, gap);
		}
		/*else
		   //English small letters        
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

char *bn_parseword(int last)
{
	int hf, itr, dcr, prevcv, i, cvflag[50], cvcnt = 0;
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
			strcat(lsyl, bn_getvowel(word[i].letter));
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
						    (bn_available_halfs[itr],
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
					       bn_getvowel(word[i - 2].letter));
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
				strcpy(lsyl, bn_getvowel(word[i].letter));
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

char *bn_getvowel(char *t)
{
	if (t[0] == 'c') {
		return (++t);
	} else {
		return (t);
	}
}

/*------------------------------------------------------------------------

 Function bn_parsenum takes an array of strings and generates the
 phonetic representation, which can then be played out.

 In Bengali,we can't really specify any rules for pronunciation of 
 the natural numbers upto 100, so one has to explicitly specify 
 the phonetics of each natural number till 100.

 From there on,one can break up the number into units,and use the 
 phonetic symbols specified for no.s < 100,along with the phonetics 
 for the various units.

 We've included units upto 'Koti' ,which is one crore, 

 Conceptually,of course,we can extend this indefinitely.

------------------------------------------------------------------------*/

char *bn_parsenum(char *numstr)
{
	//TODO change this according to the language
	char *number;
	number = (char *)malloc(10 * sizeof(char));
	number[0] = '\0';
	return number;
}

/*-----------------------------------------------------------------------

   Function bn_replacenum - takes as input a unicode array, assigns the
   corresponding phonetic string to an array of integers and returns
   the size of that array, ie no. of digits in the correponding no.

------------------------------------------------------------------------*/

char *bn_replacenum(unsigned short *s, int size)
{
	int i, j;
	char *numchar;
	i = 0;
	numchar = (char *)malloc(size * sizeof(char));
	while (i < size) {
		if (s[i] >= 0x09E6 && s[i] <= 0x09EF)	/* if in range of Bengali numbers */
			numchar[i] = ((s[i] - 0x09E6) == '\0') ? '0' : (s[i] - 0x0B66);	/* subtract offset */
		if (s[i] >= 0x30 && s[i] <= 0x39)	/* if in range of ascii numbers */
			numchar[i] = ((s[i] - 0x30) == '\0') ? '0' : (s[i] - 0x30);	/* subtract offset */

		i++;
	}
	numchar[i] = '\0';
	return (numchar);

}
