/* phonetic_synthesizer_ta.c
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
/*Thanks to Dr N Ganesan for Tamil Pronociation Rules*/

#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<stdlib.h>
#include<stdio.h>
#include "phonetic_synthesizer_ta.h"
#include "debug.h"

/*-----------------------    FUNCTIONS     ----------------------------*/

void ta_readfile();
char *ta_process(short *word, int size);
int ta_replace(unsigned short *s, int size);
char *ta_replacenum(unsigned short *s, int size);
char *ta_parsenum(char *numstr);
char *ta_spellNumbers(short *word, int start, int end);
char *ta_parseMiscellaneous(short *s, int size);
char *ta_parseword(int);
char *ta_getvowel(char *t);

/*-----------------------------------------------------------------------*/

int result;

struct wordtemplate {
	int type;
	char *letter;
};
struct wordtemplate word[100];

char *available_halfs_ta[] =
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

ta_replace accepts an array of unicode characters as input, and 
ta_replaces them by the corresponding phonetic strings.

 
------------------------------------------------------------------------*/

int ta_replace(unsigned short *s, int size)
{
	int i, j;

	i = 0;
	j = 0;
	while (i < size) {
		switch (s[i]) {

		case 0x0B82:	/*Anuswaram */
			if (j >= 1 && word[j - 1].type != 0) {
				word[j].letter = "1";
				word[j].type = 0;
				j++;
			}
			word[j].letter = "m";
			word[j].type = 0;
			j++;
			break;
		case 0x0B83:	/*Visarga- aytham  -to be corrected */
			word[j].letter = "h";
			word[j].type = 1;
			j++;
			break;
		/*--------------VOWELS-----------------*/
		case 0x0B85:	/* a 1 */
			word[j].letter = "1";
			word[j].type = 0;
			j++;
			break;
		case 0x0B86:	/* A 2 */
			word[j].letter = "2";
			word[j].type = 0;
			j++;
			break;
		case 0x0B87:	/*i 3 */
			word[j].letter = "3";
			word[j].type = 0;
			j++;
			break;
		case 0x0B88:	/*I 4 */
			word[j].letter = "4";
			word[j].type = 0;
			j++;
			break;
		case 0x0B89:	/* u 5 */
			word[j].letter = "5";
			word[j].type = 0;
			j++;
			break;
		case 0x0B8A:	/* U 6 */
			word[j].letter = "6";
			word[j].type = 0;
			j++;
			break;

		case 0x0B8E:	/*E 7 */
			word[j].letter = "7";
			word[j].type = 0;
			j++;
			break;
		case 0x0B8F:	/* EE by 8 */
			word[j].letter = "8";
			word[j].type = 0;
			j++;
			break;
		case 0x0B90:	/*ai by 9 */
			word[j].letter = "10";
			word[j].type = 0;
			j++;
			break;
		case 0x0B92:	/*o by 11 */
			word[j].letter = "11";
			word[j].type = 0;
			j++;
			break;
		case 0x0B93:	/*oo by 12 */
			word[j].letter = "12";
			word[j].type = 0;
			j++;
			break;
		case 0x0B94:	/*au by 13 */
			word[j].letter = "13";
			word[j].type = 0;
			j++;
			break;

		/*--------------MATRAS-----------------*/

		case 0x0BBE:	/*2  aa sign */
			word[j].letter = "c2";
			word[j].type = 0;
			j++;
			break;
		case 0x0BBF:	/*3  i sign ி */
			word[j].letter = "c3";
			word[j].type = 0;
			j++;
			break;

		case 0x0BC0:	/*4  ii sign ீ */
			word[j].letter = "c4";
			word[j].type = 0;
			j++;
			break;
		case 0x0BC1:	/*5 ு */
			word[j].letter = "c5";
			word[j].type = 0;
			j++;
			break;

		case 0x0BC2:	/*6  ூ */
			word[j].letter = "c6";
			word[j].type = 0;
			j++;
			break;

		case 0x0BC6:	/*7  ெ */
			word[j].letter = "c7";
			word[j].type = 0;
			j++;
			break;
		case 0x0BC7:	/*E by 8  ே */
			word[j].letter = "c8";
			word[j].type = 0;
			j++;
			break;
		case 0x0BC8:	/*ai by 9  ை */
			word[j].letter = "c10";
			word[j].type = 0;
			j++;
			break;

		case 0x0BCA:	/*o by 11  ொ */
			word[j].letter = "c11";
			word[j].type = 0;
			j++;
			break;
		case 0x0BCB:	/*oo by 12  ோ */
			word[j].letter = "c12";
			word[j].type = 0;
			j++;
			break;
		case 0x0BCC:	/*au by 13 ௌ */

			word[j].letter = "c13";
			word[j].type = 0;
			j++;
			break;
		case 0x0BD7:	/*au length mark by 13  ௗ */
			word[j].letter = "c13";
			word[j].type = 0;
			j++;
			break;

		case 0x0BCD:	/*virama ் */
			/*
			   if ((i + 1 == size || (s[i + 1] == 0x2E || s[i + 1] == 0x20))) //endng character
			   {
			   word[j].letter = "c15"; //emphasis at the end
			   word[j].type = 0;
			   j++;
			   } else {
			 */
			//just pronounce half part.
			word[j].letter = "HH";
			word[j].type = 2;
			j++;
			/*
			   }
			 */
			break;

		/*--------------CONSONANTS-----------------*/

		case 0x0B95:	/*k */
			if (i - 2 >= 0 && s[i - 2] == 0x0B99
			    && s[i - 1] == 0x0BCD) {
				word[j].letter = "g";	//check whether it is after nasal ng
				word[j].type = 1;
				j++;
				break;
			} else if (isIntervocalical(s, size, i)) {
				word[j].letter = "h";
				word[j].type = 1;
				j++;
				break;
			}

			word[j].letter = "k";
			word[j].type = 1;
			j++;

			break;
		case 0x0B99:	/*nga -Not exactly correct pronounciation */
			word[j].letter = "n";
			word[j].type = 1;
			j++;
			word[j].letter = "HH";
			word[j].type = 2;
			j++;
			word[j].letter = "g";
			word[j].type = 1;
			j++;
			break;

		case 0x0B9A:	/*ch */
			if (i - 2 >= 0 && s[i - 2] == 0x0B9E
			    && s[i - 1] == 0x0BCD) {
				word[j].letter = "j";	//check whether it is after nasal ng
				word[j].type = 1;
				j++;
				break;
			} else if (isIntervocalical(s, size, i)) {
				word[j].letter = "s";
				word[j].type = 1;
				j++;
				break;
			}
			word[j].letter = "ch";
			word[j].type = 1;
			j++;
			break;

		case 0x0B9C:	/*j */

			word[j].letter = "j";
			word[j].type = 1;
			j++;
			break;

		case 0x0B9E:	/*nja */

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

		case 0x0B9F:	/*tt */
			if (isIntervocalical(s, size, i)) {
				word[j].letter = "dd";
				word[j].type = 1;
				j++;
				break;
			}
			word[j].letter = "tt";
			word[j].type = 1;
			j++;
			break;

		case 0x0BA3:	/*nna */

			word[j].letter = "nna";
			word[j].type = 1;
			j++;
			break;

		case 0x0BA4:	/*t */
			if (isIntervocalical(s, size, i)) {
				word[j].letter = "d";
				word[j].type = 1;
				j++;
				break;
			}
			word[j].letter = "t";
			word[j].type = 1;
			j++;
			break;

		case 0x0BA8:	/*n */

			word[j].letter = "n";
			word[j].type = 1;
			j++;
			break;
		case 0x0BA9:	/*n */

			word[j].letter = "n";
			word[j].type = 1;
			j++;
			break;

		case 0x0BAA:	/*p */
			if (isIntervocalical(s, size, i)) {
				word[j].letter = "b";
				word[j].type = 1;
				j++;
				break;
			}
			word[j].letter = "p";
			word[j].type = 1;
			j++;
			break;

		case 0x0BAE:	/*m */

			word[j].letter = "m";
			word[j].type = 1;
			j++;
			break;

		case 0x0BAF:	/*y */

			word[j].letter = "y";
			word[j].type = 1;
			j++;
			break;

		case 0x0BB0:	/*r */

			word[j].letter = "r";
			word[j].type = 1;
			j++;
			break;

		case 0x0BB1:	/*R */
			if (isIntervocalical(s, size, i)) {
				word[j].letter = "ddr";
				word[j].type = 1;
				j++;

				break;
			}
			word[j].letter = "r";
			word[j].type = 1;

			j++;
			break;

		case 0x0BB2:	/*l */

			word[j].letter = "l";
			word[j].type = 1;
			j++;
			break;

		case 0x0BB3:	/*ll */

			word[j].letter = "ll";
			word[j].type = 1;
			j++;
			break;

		case 0x0BB4:	/*ll */

			word[j].letter = "zh";
			word[j].type = 1;
			j++;
			break;

		case 0x0BB5:	/*v */

			word[j].letter = "v";
			word[j].type = 1;
			j++;
			break;

		case 0x0BB6:	/*sh */

			word[j].letter = "sh";
			word[j].type = 1;
			j++;
			break;

		case 0x0BB7:	/*sh */

			word[j].letter = "sh";
			word[j].type = 1;
			j++;
			break;

		case 0x0BB8:	/*s */

			word[j].letter = "s";
			word[j].type = 1;
			j++;
			break;

		case 0x0BB9:	/*h */

			word[j].letter = "h";
			word[j].type = 1;
			j++;
			break;

		}

		++i;
	}

	return (j);
}

int isVowel(unsigned short character)
{
	if ((character == 0x0BBE || character == 0x0BBF || character == 0x0BC0
	     || character == 0x0BC1 || character == 0x0BC2
	     || character == 0x0BC6 || character == 0x0BC7
	     || character == 0x0BC8 || character == 0x0BCA
	     || character == 0x0BCB || character == 0x0BCC)) {
		return 1;
	}
	return 0;
}

int isIntervocalical(unsigned short string[], int size, int position)
{
	if (position - 1 > 0 && position + 1 < size
	    && ((isVowel(string[position - 1]) && isVowel(string[position + 1]))
		|| (string[position - 1] != 0x0BCD
		    && string[position + 1] != 0x0BCD))) {
		return 1;
	}
	return 0;
}

/*-------------------------------------------------------------------------

ta_process takes as input a short array and first determines whether it is 
a word  or a number. 

If it is a word it first calls ta_replace to get the phonetic symbols and 
followed by ta_parseword to get the phonetic description of the word. 

If it is a number, it calls ta_replacenum to get the phonetic symbols and then 
ta_parsenum to get the phonetic description of the number.

It then outputs the phonetic description.

--------------------------------------------------------------------------*/

char *generate_phonetic_script_ta(short *word, int size)
{

	char *final;
	int arrsz;
	int i = 0;
	;
	final = (char *)malloc(100 * sizeof(char *));
	final[0] = '\0';
	//check for number. 0x0D66-0x0D6F : Tamil numbers
	//0x30 to 0x39 : ASCII numbers
	if ((word[0] >= 0x0D66 && word[0] <= 0x0D6F)
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
				final =
				    strcat(final,
					   ta_spellNumbers(word, 0, size));
			} else {	//construct the number string using ta_parsenum logic
				//printf("reading numbers\n");
				final =
				    strcat(final,
					   ta_parsenum(ta_replacenum
						       (word, size)));
			}
		} else {
			//Read the number part as usual
			if (i > 9) {
				//size >9 i.e. greater than 1 crore.
				//Assuming it is a phone number or credit card number etc
				//Read each numbers seperately  
				//      printf("spell out numbers\n");
				final =
				    strcat(final, ta_spellNumbers(word, 0, i));
				//      printf("spell out numbers-over\n");
			} else {	//construct the number string using ta_parsenum logic
				//      printf("speak numbers%d\n",i);
				final =
				    strcat(final,
					   ta_parsenum(ta_replacenum(word, i)));
				//      printf("speak numbers%d done\n",i);
			}
			//say dashamsham
			//        printf("say dashamsham");
			final = strcat(final, " d1 sh2m sh1m ");
			//Now spell out each decimal places
			final =
			    strcat(final, ta_spellNumbers(word, i + 1, size));
		}
	} else {
		/* Tamil Word ? */
		arrsz = ta_replace(word, size);	/* Get phonetic description */
		//arrsz is the size of the phonetic array

		final = strcat(final, ta_parseword(arrsz));

	}

	//Miscellaneous support!!!
	//English letters, special symbols etc...
	final = strcat(final, ta_parseMiscellaneous(word, size));

	//print the phonetic string produced by this engine to stdout..
	DHVANI_DEBUG("%s", final);
	return (final);		/* Done!!!  */

}

char *ta_spellNumbers(short *word, int start, int end)
{
	int i;
	char *final = (char *)malloc(100 * sizeof(char *));
	final[0] = '\0';
	char *decimal = (char *)malloc(100 * sizeof(char *));

	for (i = start; i < end; i++) {
		decimal[0] = word[i];
		decimal[1] = '\0';
		final = strcat(final, ta_parsenum(ta_replacenum(decimal, 1)));
		final = strcat(final, " G1500 ");
	}

	free(decimal); decimal = NULL;

	return final;
}

char *ta_parseMiscellaneous(short *s, int size)
{
	char *phoneticScript = (char *)malloc(100 * sizeof(char *));
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
		//English caps letters 
		if (s[i] >= 65 && s[i] <= 90) {
			strcat(phoneticScript, englishAlph[s[i] - 65]);
			strcat(phoneticScript, gap);
		}		//English small letters        
		/*else if (s[i] >= 97 && s[i] <= 122) {
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

char *ta_parseword(int last)
{
	int hf, itr, dcr, prevcv, i, cvflag[50], cvcnt;
	char *syllable, *lsyl, *t_half;

	prevcv = 0;
	i = 0;
	cvcnt = 0;

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
	i = 0;
	for (i = last - 1; i >= 0; i--) {	//right to left
		dcr = 0;
		hf = 0;
		if (cvflag[i] == 2) {
			//nothing to do. just concatenate with syllable string
			strcpy(lsyl, "\0");
			strcat(lsyl, ta_getvowel(word[i].letter));
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
						    (available_halfs_ta[itr],
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
					       ta_getvowel(word[i - 2].letter));
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
				strcpy(lsyl, ta_getvowel(word[i].letter));
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

char *ta_getvowel(char *t)
{
	if (t[0] == 'c') {
		return (++t);
	} else {
		return (t);
	}
}

/*------------------------------------------------------------------------
TODO: We must replace this function with actual tamil number reading logic. This
is a copy paste from Malayalam. 
 Function ta_parsenum takes an array of strings and generates the
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

char *ta_parsenum(char *numstr)
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
			strcat(number, ta_parsenum(tmp));
			strcat(number, " k12 tt3 ");
			inr = 1;
		} else if (place - i == 8) {

			strcat(number, singles[numstr[i] - 0]);
			strcat(number, " k12 tt3 ");
		} else if (place - i == 7) {
			strncpy(tmp, numstr + i, 2);
			tmp[2] = '\0';
			strcat(number, ta_parsenum(tmp));
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
			strcat(number, ta_parsenum(tmp));
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

   Function ta_replacenum - takes as input a unicode array, assigns the
   corresponding phonetic string to an array of integers and returns
   the size of that array, ie no. of digits in the correponding no.

------------------------------------------------------------------------*/

char *ta_replacenum(unsigned short *s, int size)
{
	int i, j;
	char *numchar;
	i = 0;
	j = 0;
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
