/* phonetic_synthesizer_ps.c
 *
 * Copyright (C) 2008
 *  Zabeeh Khan <zabeehkhan@gmail.com>, The Pathanisation Project.
 *  http://pathanisation.pakhtosoft.com
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

/*-----------------------------------------------------------------------

   phonetic_synthesizer_ps.c:
 
     A Unicode-Text-Format (UTF) to phonetics convertor for
     Pashto.
According to "Unicode@unicode.org" the forms of a letter such as initial, medial, final and isolated do not need to be added individually. All the forms are recognized through the original form. Indeed, that is true!
 
-----------------------------------------------------------------------*/

#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<stdlib.h>
#include<stdio.h>
#include "phonetic_synthesizer_ps.h"
#include "debug.h"

/*-----------------------    FUNCTIONS     ----------------------------*/

char *ps_process(short *word, int size);
int ps_replace(unsigned short *s, char **s1, unsigned char *template, int size);
int ps_replacenum(unsigned short *s, int *s1, int size);
void ps_parseword(char **arr, unsigned char *template, int arrsz,
		  char **phon, char *final);
void ps_parsenum(int *digits, int arrsz, char *final);
void ps_synthesize(char *txt);
int ps_ishalf(char *testhalf);
int ps_checkspecial(int size, int start, int *pos, char **arr);

/*-----------------------------------------------------------------------*/

static int result;
/*-----------------------------------------------------------------------*/

/*------------------------------------------------------------------------

replace accepts an array of unicode characters as input, and 
replaces them by the corresponding phonetic strings.

It is assumed that the input is unicode for Pashto.
 
------------------------------------------------------------------------*/

int ps_replace(unsigned short *s, char **s1, unsigned char *template, int size)
{
	int i = 0, j = 0;
	while (i < size) {
		switch (s[i]) {
// Don't know how it works, but it works!
		case 0x0647:	/*Heh (eye-like) in the beginning and at the middle and last */
			if (i < size - 1 && s[i + 1] != '\0') {
				s1[j++] = "h";
				template[j - 1] = 1;
				s1[j++] = "h";
				template[j - 1] = 2;
			} else {
				s1[j++] = "1";
				template[j - 1] = 1;
			};
			break;

			/*--------------VOWELS-----------------*/

		case 0x0639:	/*1 - letter Ain (Arabic) - comp. form */
			s1[j++] = "1";
			template[j - 1] = 0;
			break;

		case 0x0627:	/*2 - letter Alef - comp. form */
			s1[j++] = "2";
			template[j - 1] = 0;
			break;

		case 0x0622:	/*2 - letter Alef with Madda - comp. form - not used in Pashto */
			s1[j++] = "2";
			template[j - 1] = 0;
			break;

		case 0x06D0:	/*3 - letter Yeh with two dots above each other - comp. form */
			s1[j++] = "7";
			template[j - 1] = 0;
			break;

		case 0x064A:	/*4 - letter Yeh with two dots below near each other */
			s1[j++] = "4";
			template[j - 1] = 0;
			break;

		case 0x06CC:	/*8 - letter Yeh with no dots */
			s1[j++] = "8";
			template[j - 1] = 0;
			break;

		case 0x06CD:	/*10 - letter Yeh with tail */
			s1[j++] = "10";
			template[j - 1] = 0;
			break;

		case 0x0626:	/*10 - letter Yeh with hamza above */
			s1[j++] = "10";
			template[j - 1] = 0;
			break;

		case 0x0648:	/*6 - letter Waw - comp. form */
			s1[j++] = "6";
			template[j - 1] = 0;
			break;

		case 0x0624:	/*6 - letter Waw with Hamza above - comp. form - not used in Pashto */
			s1[j++] = "6";
			template[j - 1] = 0;
			break;

			/*--------------PUNCTUATION SIGNS-----------------*/
			// not used, but still have to add them after the beta release

			/*--------------CONSONANTS-----------------*/

		case 0x0643:	/*k - letter Kaf - comp. form */
			s1[j++] = "k";
			template[j - 1] = 1;
			break;

		case 0x06A9:	/*k - letter Kaf (Keheh) - comp. form */
			s1[j++] = "k";
			template[j - 1] = 1;
			break;

// error! it is not the Khah sound. it is the K+Ha sound in Hindi
		case 0x062E:	/*kh - letter Khah - comp. form */
			s1[j++] = "kh";
			template[j - 1] = 1;
			break;

		case 0x069A:	/*kh - letter Kheen - comp. form */
			s1[j++] = "kh";
			template[j - 1] = 1;
			break;

		case 0x06AB:	/*g - letter Gaf - comp. form */
			s1[j++] = "g";
			template[j - 1] = 1;
			break;

		case 0x0696:	/*g - letter Geh - comp. form */
			s1[j++] = "g";
			template[j - 1] = 1;
			break;

// error! it is not the Ghah sound. it is the G+Ha sound in Hindi
		case 0x063A:	/*gh - letter Ghain - comp. form */
			s1[j++] = "g";
			template[j - 1] = 1;
			break;

		case 0x0686:	/*ch - letter Tcheh - comp. form */
			s1[j++] = "ch";
			template[j - 1] = 1;
			break;

		case 0x062C:	/*j - letter Jeem - comp. form */
			s1[j++] = "j";
			template[j - 1] = 1;
			break;

		case 0x0698:	/*j - letter Zhe (Jeh) as in "Rojha" - comp. form */
			s1[j++] = "j";
			template[j - 1] = 1;
			break;

		case 0x067C:	/*tt - letter Tteh as in "Tatobay" - comp. form */
			s1[j++] = "tt";
			template[j - 1] = 1;
			break;

		case 0x0689:	/*dd - letter Ddal as in "Dudai" - comp. form */
			s1[j++] = "dd";
			template[j - 1] = 1;
			break;

		case 0x062A:	/*t - letter Teh as in "Atal" - comp. form */
			s1[j++] = "t";
			template[j - 1] = 1;
			break;

		case 0x0637:	/*t - letter Tah (Arabic)- comp. form */
			s1[j++] = "t";
			template[j - 1] = 1;
			break;

		case 0x062F:	/*d - letter Dal - comp. form */
			s1[j++] = "d";
			template[j - 1] = 1;
			break;

		case 0x0630:	/*z - letter Thal (Arabic) - comp. form */
			s1[j++] = "d";
			template[j - 1] = 1;
			break;

		case 0x0646:	/*n  - letter Noon - comp. form */
			s1[j++] = "n";
			template[j - 1] = 1;
			break;

		case 0x067E:	/*p - letter Peh - comp. form */
			s1[j++] = "p";
			template[j - 1] = 1;
			break;

		case 0x0641:	/*f - letter Feh - comp. form */
			s1[j++] = "f";
			template[j - 1] = 1;
			break;

		case 0x0628:	/*b - letter Beh - comp. form */
			s1[j++] = "b";
			template[j - 1] = 1;
			break;

		case 0x0645:	/*m - letter Meem - comp. form */
			s1[j++] = "m";
			template[j - 1] = 1;
			break;

		case 0x0631:	/*r - letter Reh - comp. form */
			s1[j++] = "r";
			template[j - 1] = 1;
			break;

		case 0x0644:	/*l - letter Lam - comp. form */
			s1[j++] = "l";
			template[j - 1] = 1;
			break;

		case 0x0634:	/*sh - letter Sheen - comp. form */
			s1[j++] = "sh";
			template[j - 1] = 1;
			break;

		case 0x0633:	/*s - letter Seen - comp. form */
			s1[j++] = "s";
			template[j - 1] = 1;
			break;

		case 0x062B:	/*s - letter Theh (Arabic) - comp. form */
			s1[j++] = "s";
			template[j - 1] = 1;
			break;

		case 0x0685:	/*s - letter Theh (Pashto) - comp. form */
			s1[j++] = "s";
			template[j - 1] = 1;
			break;

		case 0x0635:	/*s - letter Sad (Arabic) - comp. form */
			s1[j++] = "s";
			template[j - 1] = 1;
			break;

//                case 0x0647: /*h - letter Heh (eye-like) - comp. form */
//                        s1[j++] = "1";
//                        template[j - 1] = 1;
//                        break;

		case 0x062D:	/*h - letter Hah - comp. form */
			s1[j++] = "h";
			template[j - 1] = 1;
			break;

		case 0x0642:	/*q. - letter Qaf - comp. form */
			s1[j++] = "k";
			template[j - 1] = 1;
			break;

		case 0x0632:	/*z. - letter Zeh - comp. form */
			s1[j++] = "z";
			template[j - 1] = 1;
			break;

		case 0x0681:	/*z. - letter Zeem - comp. form */
			s1[j++] = "z";
			template[j - 1] = 1;
			break;

		case 0x0636:	/*z. - letter Dad (Arabic) - comp. form */
			s1[j++] = "z";
			template[j - 1] = 1;
			break;

		case 0x0638:	/*z. - letter Zah (Arabic) - comp. form */
			s1[j++] = "z";
			template[j - 1] = 1;
			break;

		case 0x0693:	/*ddh. - letter Rreh as in "Narrai (world)" - comp. form */
			s1[j++] = "ddh";
			template[j - 1] = 1;
			break;

		}

		++i;
	}
	return (j);
}

/*-------------------------------------------------------------------------

Process takes as input a short array and first determines whether it is 
a word  or a number. 

If it is a word it first calls replace to get the phonetic symbols and 
followed by parseword to get the phonetic description of the word. 

If it is a number, it calls replacenum to get the phonetic symbols and then 
parsenum to get the phonetic description of the number.

It then outputs the phonetic description.

--------------------------------------------------------------------------*/

char *generate_phonetic_script_ps(unsigned short *word, int size)
{
	char *arr[1000], *subarr1[1000];
	unsigned char template[1000];
	unsigned char subtemplate1[1000];
	char *phon[2000];
	int i, arrsz, special, start = 0;
	int digits[200];
	int *pos = (int *)malloc(100 * sizeof(int));
	char *final = (char *)malloc(1000 * sizeof(char));
	final[0] = '\0';
	if (word[0] >= 0x0966 && word[0] <= 0x096F) {	/* Number ? */
		arrsz = ps_replacenum(word, digits, size);
		ps_parsenum(digits, arrsz, final);
	} else {		/* Word ? */

		arrsz = ps_replace(word, arr, template, size);	/* Get phonetic description */
		special = ps_checkspecial(size, start, pos, arr);	/*check for prefixes */
		if (special && (size > special)) {	/* prefix, ie 'sub-word' found */

			while (special && (start <= size)) {

				for (i = 0; i < special; i++) {
					subarr1[i] = arr[i + start];
					subtemplate1[i] = template[i + start];
				}

				subarr1[i] = " ";
				subtemplate1[i] = 3;	/* 'invalid',so parser simply ignores */
				ps_parseword(subarr1, subtemplate1, special + 1,
					     phon, final);

				start += special;	/* move the 'pointer' to end of prefix */
				special = ps_checkspecial(size, start, pos, arr);	/* check for next prefix  */
			}

			if (start + 1 < size) {	/* word not ended yet  */
				for (i = 0; i < size - start; i++) {	/* make a copy of the prefix */
					subarr1[i] = arr[i + start];
					subtemplate1[i] = template[i + start];
				}
				ps_parseword(subarr1, subtemplate1, size - start, phon, final);	/* parse */
			}
		} else {	/* the 'usual' parsing  */
			ps_parseword(arr, template, arrsz, phon, final);
		}		//end of else

	}

	free(pos); pos = NULL;

	//Now we got the phonetic string
	printf("%s", final);
	return final;		/* Done ? */

}

/*-------------------------------------------------------------------------

Parseword takes as input two arrays, one of phonetic strings and the
other of unsigned char containing a template in which 0 represents vowel,
1 represents consonant and 2 represents halant, and generates the
phonetic representation which can then be played out. 

The parsing is done according to the right-to-left parsing algorithm
described in the file parse_algo.txt 

--------------------------------------------------------------------------*/

void
ps_parseword(char **arr, unsigned char *template, int arrsz, char **phon,
	     char *final)
{

	int i = 0, j = 0, k = 0;
	int cvmarks[100], cvnum, cend = 0;
	char *testhalf;
	char *tempstr;

	for (i = 0; i < arrsz; i++)
		cvmarks[i] = 0;

	i = 0;
	j = 0;

	/* First find all the CV pairs in the input */

	while (i + 1 < arrsz) {
		if ((template[i] == 1) && (template[i + 1] == 0)) {	/* CV */
			cvmarks[i] = 1;
			i += 2;
			j++;
		} else if ((template[i] == 2) && (template[i + 1] == 0)) {	/* ChV */
			cvmarks[i - 1] = 1;
			i += 2;
			j++;
		} else
			i++;
	}

	k = 0;

	if ((template[arrsz - 1] == 1) && (arrsz > 1)) {	/* if ends in C,make it Ch */
		arr[arrsz] = "H";
		template[arrsz] = 2;
		arrsz++;
	}

	for (i = arrsz - 1; i >= 0; i--) {	/* Parse the input */

		if (template[i] == 1) {	/* Consonant */

			if (i == 0) {	/* just a consonant, make it C1 */
				phon[k++] = "1";
				phon[k++] = arr[i];
			} else if (cvmarks[i])	/* If CV,already processed at V */
				;

			else if (template[i - 1] == 2) {	/* Consonant-Halant */
				phon[k++] = "1";
				phon[k++] = arr[i];
			} /* Output C1C */
			else if (template[i - 1] == 1) {	/* Another consonant */
				if (arr[i] == "h") {	/* Special case - output C7C */
					phon[k++] = arr[i];
					phon[k++] = "7";
					phon[k++] = arr[i - 1];
					i -= 1;
				} else {	/* output C1C  */

					phon[k++] = arr[i];
					phon[k++] = "1";
					phon[k++] = arr[i - 1];
					i -= 1;
				}
			} else if (template[i - 1] == 0) {	/* Vowel */
				if (i > 1) {
					if (!cvmarks[i - 2]) {	/* Not CVC ? */
						phon[k++] = arr[i];
						phon[k++] = arr[i - 1];
						i--;	/* output VC */
					} else {	/* CVC */

						if ((arr[i - 1] == "1") && (arr[i] == "h")) {	/* Special case - o/p C7C */
							phon[k++] = arr[i];
							phon[k++] = "7";
							phon[k++] = arr[i - 2];
							i -= 2;
						} else {	/* output CVC */

							phon[k++] = arr[i];
							phon[k++] = arr[i - 1];
							phon[k++] = arr[i - 2];
							i -= 2;
						}
					}
				} else {	/* i <= 1  */

					if (i == 1) {
						if ((template[i - 1] == 0)) {	/*CV!?! */
							phon[k++] = arr[i];
							phon[k++] = arr[i - 1];
							i--;
						} else {	/* i==0 */

							phon[k++] = "1";
							phon[k++] = arr[i];
							i--;
						}
					}
					/*else   
					   { printf(" It does !?! "); 
					   phon[k++]=arr[i];phon[k++]=arr[i-1];
					   } */
				}

			}
		} else if (template[i] == 2) {	/* Consonant-Halant */

			if (((i == 1) && (template[i + 2] != 2)) || (template[i - 2] == 2)) {	/*Another Consonant-Halant */
				if (((i < arrsz - 1) && (cvmarks[i + 1])) || (i == 1)) {	/* Half-sound?? */
					/* test whether the database has corresponding 'half' sound */
					testhalf = malloc(10 * sizeof(char));
					testhalf = strcpy(testhalf, arr[i - 1]);	/* construct test case */
					testhalf = strcat(testhalf, arr[i + 1]);	/* by joining 2 sounds */

					if (ps_ishalf(testhalf)) {	/* yes,we do have the 'half' sound */
						tempstr =
						    malloc(10 * sizeof(char));
						tempstr =
						    strcpy(tempstr, arr[i - 1]);
						tempstr = strcat(tempstr, "H");
						if (phon[k - 2] != "0")	/* e.g "r" becomes "kHr"  */
							phon[k - 2] =
							    strcat(tempstr,
								   phon[k - 2]);
						else {
							phon[k - 2] = tempstr;	/* replace "0" by the 'half' sound */
						}
						i -= 1;
					} else {	/* no,we don't have the 'half' sound,so output 0C */

						phon[k++] = arr[i - 1];
						phon[k++] = "0";
						i--;
					}

					free(testhalf); testhalf = NULL;
				} else {	/* need not check for 'half' sound, just output 0C */

					phon[k++] = arr[i - 1];
					phon[k++] = "0";	/* output 0C */
					i--;
				}
			} else if (i == 1) {	/* Just a Ch, so output 0C */
				phon[k++] = arr[i - 1];
				phon[k++] = "0";
				i--;
			} else if (template[i - 2] == 1) {	/* A consonant, so output C1C */
				phon[k++] = arr[i - 1];
				phon[k++] = "1";
				phon[k++] = arr[i - 2];
				i -= 2;
			} else if (template[i - 2] == 0) {	/* Vowel */
				if (i > 1) {
					if ((i - 3 < 0) || (!cvmarks[i - 3])) {	/* Not CVC ? Output VC */
						phon[k++] = arr[i - 1];
						phon[k++] = arr[i - 2];
						i -= 2;
					} else {	/* CVC */

						phon[k++] = arr[i - 1];
						phon[k++] = arr[i - 2];
						phon[k++] = arr[i - 3];
						i -= 3;
					}
				}
			} else if (cvmarks[i - 1]) ;	/* If CV,already done at V */

		}

		else if (template[i] == 0) {	/* Vowel */
			if (i == 0) {	/* Lone vowel, just output V */
				phon[k++] = arr[i];
			} else if (cvmarks[i - 1]) {	/* Part of a CV */
				if ((arr[i] == "13") && (arr[i + 1] == "v")) {	/* special cases */
					phon[k++] = "14";
					phon[k++] = arr[i - 1];
					i--;
				} else if ((arr[i] == "9")
					   && (arr[i + 1] == "y")
					   && (i < arrsz - 1)) {
					phon[k++] = "10";
					phon[k++] = arr[i - 1];
					i--;
				} else {	/* Just output CV */

					phon[k++] = arr[i];
					phon[k++] = arr[i - 1];
					i--;	/* output CV */
				}
			} else if ((template[i - 1] == 2) && (cvmarks[i - 2])) {	/* Part of a ChV */
				phon[k++] = arr[i];
				phon[k++] = arr[i - 2];
				i -= 2;
			} /* output CV */
			else	/* Lone vowel, output V */
				phon[k++] = arr[i];	/* output V */
		}

		phon[k++] = " ";	/* Insert a blank, to separate 'speakable sounds' */
	}

	for (i = k - 1; i >= 0; i--)	/* combine all the phonetic symbols */
		final = strcat(final, phon[i]);

}

/*----------------------------------------------------------------------

   Function Synthesize - Print out phonetics on the client socket  

----------------------------------------------------------------------*/

/*----------------------------------------------------------------------

Function ishalf checks whether the given phonetic string belongs 
to the set of 'half-sounds' available in our database of sounds - 
if so,it returns 1,else 0.

----------------------------------------------------------------------*/

int ps_ishalf(char *testhalf)
{
	int i = 0;

	char *halves[] =
	    { "bhl", "bhy", "bhr", "bl", "br", "by", "chv", "chr", "chy",
		"ddr",
		"ddv", "ddy", "dhr", "dhv", "dhy", "dr", "dv", "dy", "fl", "fr",
		"ghr", "ghy", "ghn",
		"ghv", "gl", "gn", "gr", "gv", "gy", "jv", "jy", "khr", "khv",
		"khy", "khl", "kll",
		"kl", "kr", "ksh", "kv", "ky", "mr", "my", "nv", "nr", "ny",
		    "pl",
		"pll", "pr", "py",
		"thr", "thy", "ttr", "ttv", "tty", "tr", "tv", "ty", "vr", "vy",
		"vr", "vl", "end"
	};

	while (strcmp(halves[i], "end")) {	/* Traverse the list of 'half' sounds  */
		if (!strcmp(halves[i++], testhalf))	/* Match found ? */
			return (1);
	}

	return (0);		/* No match found, ie desired 'half' sound isn't avilable  */
}

/*------------------------------------------------------------------------

 Function Parsenum takes an array of strings and generates the
 phonetic representation, which can then be played out.

 In Pashto,we can't really specify any rules for pronunciation of 
 the natural numbers upto 100, so one has to explicitly specify 
 the phonetics of each natural number till 100.

 From there on,one can break up the number into units,and use the 
 phonetic symbols specified for no.s < 100,along with the phonetics 
 for the various units.

 We've included units upto 'shankh' ,which is 10^17,so we can go upto
 99*10^17 - only because we don't know what to call the next scale !!

 Conceptually,of course,we can extend this indefinitely.

------------------------------------------------------------------------*/

void ps_parsenum(int *arr, int arrsz, char *final)
{
	char *scales[] =
	    { " ", "s13", "h1 z2r", "l2kh", "k1 r12dd", "1 r1b", "kh1 r1b",
		"n4l",
		"p1 d1m", "sh1n 0kh"
	};

	char *units[] =
	    { "sh6n 0y", "8k", "d12", "t4n", "ch2r", "p2n 0ch", "chh8", "s2t",
		"2tth",
		"n13"
	};

	char *teens[] =
	    { "d1s", "gyHy2 r1h", "b2 r1h", "t8 r2", "ch13 d1h", "p1n drHr1h",
		"s12 l1h", "s1 trHr1", "1tth tth2 r1h", "6n n4s"
	};

	char *twenties[] =
	    { "b4s", "3k k4s", "b2 4s", "t8 y3s", "ch13 b4s", "p1ch ch4s",
		"chh1b b4s", "s1t t2 4s", "1tth tth2 4s", "6n t4s"
	};

	char *thirties[] =
	    { "t4s", "3k t3s", "b1t t3s", "t9 t3s", "ch13 t3s", "p9 t3s",
		"chh1t t3s",
		"s9 t3s", "1dd t3s", "6n t2 l3s"
	};

	char *forties[] =
	    { "ch2 l3s", "3k t2 l3s", "b1 y2 l3s", "t9 t2 l3s", "ch14 v2 l3s",
		"p9 t2 l3s", "chh3 y2 l3s", "s9 t2 l3s", "1dd G1000 t2 l3s",
		"6n ch2s"
	};

	char *fifties[] =
	    { "p1 G500 ch2s", "3k kyHy2 v1n", "b2 v1n", "t3r G500 p1n",
		"ch14 v1n",
		"p1ch G500 p1n", "chh1p p1n", "s1t t2 v1n", "1tt tth2 v1n",
		"6n s1tth"
	};

	char *sixties[] =
	    { "s2tth", "3k s1tth", "b2 s1tth", "t3r s1tth", "ch14 s1tth",
		"p9 s1tth",
		"chh2 s1tth", "s1dd s1tth", "1dd s1tth", "6n h1t t1r"
	};

	char *seventies[] =
	    { "s1t t1r", "3k h1t t1r", "b1 h1t t1r", "t3 h1t t1r",
		"ch14 h1t t1r",
		"p1ch chh1t t1r ", "chh3 h1t t1r", "s1 t1t t1r", "1 tth1t t1r",
		"6n y2 s4"
	};
	char *eighties[] =
	    { "1s s4", "3k kyHy2 s4", "b1 y2 s4", "t3 r2 s4", "ch14 r2 s4",
		"p1ch ch2 s4", "chh3 y2 s4", "s1t t2 s4", "1tt tth2 s4",
		    "n1 v2 s4"
	};

	char *nineties[] =
	    { "n1b b8", "3k kyHy2n b8", "b2n b8", "t3 r2n b8", "ch14 r2n b8",
		"p1n ch2n b8", "chh3 y2n b8", "s1t t2n b8", "1tt G500 tth2n b8",
		"n3 nHy2n b8"
	};

	char **uptohundred[] =
	    { units, teens, twenties, thirties, forties, fifties, sixties,
		seventies,
		eighties, nineties
	};

	int i = 0, j = 0;

	if (arrsz == 1) {	/* single digit case */
		strcat(final, units[arr[i]]);
		strcat(final, " G3000 ");
	} else if (arrsz == 2) {	/* less than hundred */
		strcat(final, uptohundred[arr[i]][arr[i + 1]]);
		strcat(final, " G3000 ");
	} else if (arrsz == 3) {	/* between hundred & thousand - special handling reqd */
		strcat(final, units[arr[i]]);
		strcat(final, " G3000 ");
		i++;
		/* 'this many' hundreds */
		strcat(final, scales[1]);
		strcat(final, " G3000 ");
		/* ten's & units's places */
		strcat(final, uptohundred[arr[i]][arr[i + 1]]);
		strcat(final, " G3000 ");
	} else if (arrsz > 3) {	/* Work required !! */
		j = 0;
		i = (arrsz % 2) ? 0 : 1;	/* special check for the highest scale */
		if (i) {
			/* value in highest scale is less than ten */
			strcat(final, units[arr[0]]);
			strcat(final, " G3000 ");
			j++;
			strcat(final, scales[(arrsz - i) / 2 + 1]);
			strcat(final, " G3000 ");
			j++;
		}
		for (; i < arrsz - 3; i += 2) {	/* repeat till less than one thousand */
			/* next two digits */
			strcat(final, uptohundred[arr[i]][arr[i + 1]]);
			strcat(final, " G3000 ");
			j++;
			/* the scale */
			strcat(final, scales[(arrsz - i) / 2]);
			strcat(final, " G3000 ");
			j++;
		}
		if (arr[arrsz - 3]) {	/* for the last 3 digits  */
			strcat(final, units[arr[arrsz - 3]]);
			strcat(final, " G3000 ");
			j++;
			strcat(final, scales[1]);
			strcat(final, " G3000 ");
			j++;
		}
		strcat(final, uptohundred[arr[arrsz - 2]][arr[arrsz - 1]]);
		strcat(final, " G3000 ");

	}
}

/*-----------------------------------------------------------------------

   Function replacenum - takes as input a unicode array, assigns the
   corresponding phonetic string to an array of integers and returns
   the size of that array, ie no. of digits in the correponding no.

------------------------------------------------------------------------*/

int ps_replacenum(unsigned short *s, int *s1, int size)
{
	int i, j;

	i = 0;
	j = 0;

	while (i < size) {
		if (s[i] >= 0x0966 && s[i] <= 0x096F)	/* if in range */
			s1[j++] = (int)(s[i++] - 0x0966);	/* subtract offset */

	}

	return (j);

}

/*-----------------------------------------------------------------------

   Function checkspecial - looks for occurence of 'special' prefixes
   within the input word,ie subwords,starting from the given position
   & returns either 0 (if no prefix) or length of the prefix.

------------------------------------------------------------------------*/

int ps_checkspecial(int size, int start, int *pos, char **arr)
{
	/* list of 'prefixes' */
	char *special[] =
	    { "r2shHttHr", "sbh2", "mh2", "pHrdh2n", "v3dh2n", "l12k",
		"s5v3dh2",
		"end"
	};
	int i, j;
	char *test = (char *)malloc((size) * sizeof(char));
	test[0] = '\0';
	for (i = start; i < size; i++) {	/* Traverse the input word  */
		j = 0;
		if (arr[i])
			test = strcat(test, arr[i]);
		while (special[j] != "end")	/* Compare with each prefix */
			if (!strcmp(special[j++], test)) {	/* Match found ?  */
				free(test); test = NULL;
				return (i + 1 - start);
			}
	}

	free(test); test = NULL;

	return (0);		/* No match found */
}
