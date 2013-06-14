/* Code written by Ravi Masalthi, masalthi@green.csa.iisc.ernet.in */

#include<stdio.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/unistd.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include "phonetic_synthesizer_kn.h"
#include "debug.h"
/*------------------------------------------------------------------------*/
#define base 0x0C82
#define maxcode 0x0CEF

/*------------------------------------------------------------------------*/

char *kn_replacenum(unsigned short *, int);
char *kn_parsenum(char *);

/*------------------------------------------------------------------------*/

struct code {
	int type;
	unsigned short alpha;
	unsigned char beta;
};

char *alph[] =
    { "0", "00", "none", "1", "2", "3", "4", "5", "6", "r5", "ll5", "none",
	"7",
	"8", "10", "none", "11", "12", "14", "k", "kh", "g", "gh", "gn", "ch",
	"chh", "j",
	"jh", "ny", "tt", "tth", "dd", "ddh", "nna", "t", "th", "d", "dh", "n",
	"none", "p",
	"f", "b", "bh", "m", "y", "r", "rr", "l", "ll", "none", "v", "sh",
	"sh", "s", "h",
	"none", "none", "none", "none", "c2", "c3", "c4", "c5", "c6", "Hr",
	"Hrr", "none",
	"c7", "c8", "c10", "none", "c11", "c12", "c14", "HH", "none", "none",
	"none", "none",
	"none", "none", "none", "dheerga", "ru", "none", "none", "none",
	"none", "none",
	"none", "none", "ll", "none", "ru", "ruu", "none", "none", "none",
	"none"
};

char *kn_halfs[] =
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

char spacedelay[] = " G3000 ";
char linedelay[] = " G10000 ";
char *word[100];

/* A word is assumed to be not more than 100 letters.
				   This array is written into by readfile */
struct wordtemplate {
	int type;
	char *letter;
};
struct wordtemplate word1[100];

/*-----------------------------------------------------------*/
int result;

/* ----------------------------------------------------*/
char *kn_breakit(int);
int kn_replace(unsigned short *s, int size);
char *kn_getvowel(char *t);

/*------------------------------------------------------------------------
 Readfile uses getnext repeatedly to identify tokens in a Kannada text.
 A token is delimited by either space/commas/tab/carriage-return etc 
 and must be in Kannada. i.e., code.type==1.  Once a token is 
 identified, it is processed and synthesised . At each space/comma/tab/newline 
 etc, a G3000 is also synthesised.
---------------------------------------------------------------------------*/

/*void
readfile (FILE * fd)*/
char *generate_phonetic_script_kn(short *word, int size)
{
	int j = 0;
	char *final;
	final = (char *)malloc(100 * sizeof(char));
	final[0] = '\0';
	//check for number. 0x0CE6-0x0CEF : Kannada numbers
	//0x30 to 0x39 : ASCII numbers
	if ((word[0] >= 0xCE6 && word[0] <= 0x0CEF)
	    || (word[0] >= 0x30 && word[0] <= 0x39)) {
		final = strcat(final, kn_parsenum(kn_replacenum(word, size)));
	} else {
		if (word[size-1]  == 0x2E){
			/*This is a fullstop . at the end of the word*/
			size-=1;
		}	
		j = kn_replace(word, size);
		final = strcat(final, kn_breakit(j));
		final = strcat(final, spacedelay);
	}

	dhvani_debug("%s", final);
	return final;
}

char *kn_replacenum(unsigned short *s, int size)
{
	int i, j;
	char *numchar;
	i = 0;
	j = 0;
	numchar = (char *)malloc(size * sizeof(char));
	while (i < size) {
		if (s[i] >= 0x0CE6 && s[i] <= 0x0CEF)	/* if in range of kannada numbers */
			numchar[i] = ((s[i] - 0x0CE6) == '\0') ? '0' : (s[i] - 0x0CE6);	/* subtract offset */
		if (s[i] >= 0x30 && s[i] <= 0x39)	/* if in range of ascii numbers */
			numchar[i] = ((s[i] - 0x30) == '\0') ? '0' : (s[i] - 0x30);	/* subtract offset */

		i++;
	}
	numchar[i] = '\0';
	return (numchar);

}

/* function ishul. this is used by getsymboltype
 */
int ishul(char *symbol)
{
	if (strcmp(symbol, "HH") == 0) {
		return (1);
	} else
		return (0);
}

/* function isvowel. this is used by getsymboltype
 */
int isvowel(char *symbol)
{
	if ((symbol[0] > '0' && symbol[0] < '9')
	    || ((symbol[0] == 'c') && (symbol[1] > '0' && symbol[1] < '9'))) {
		return (1);
	} else
		return (0);
}

/* function iscon. this is used by getsymboltype
 */
int iscon(char *symbol)
{
	int i = 0;
	char *cons[] =
	    { "k", "kh", "g", "gh", "ch", "chh", "j", "jh", "tt", "tth", "dd",
		"ddh",
		"nna", "t", "th", "d", "dh", "n", "p", "f", "b", "bh", "m", "y",
		"r", "l", "ll", "v",
		"sh", "s", "h", "\0"
	};

	while (cons[i] != "\0") {
		if (strcmp(cons[i], symbol) == 0) {
			return (1);
			break;
		}
		i++;
	}
	return (0);
}

int kn_replace(unsigned short *s, int size)
{
	int k, j = 0;
	for (k = 0; k < size; k++) {
		word[k] = malloc(sizeof *((word[k])));
		word[k] = alph[s[k] - base];
	}
	for (k = 0; k < size; k++) {
		if (isvowel(word[k])) {
			word1[j].letter = malloc(sizeof *(word1[j].letter));
			word1[j].letter = word[k];
			word1[j].type = 0;
			j++;
		} else if (iscon(word[k])) {
			word1[j].letter = malloc(sizeof *(word1[j].letter));
			word1[j].letter = word[k];
			word1[j].type = 1;
			j++;
		} else if (ishul(word[k])) {
			word1[j].letter = malloc(sizeof *(word1[j].letter));
			word1[j].letter = word[k];
			word1[j].type = 2;
			j++;
		} else {
			j = processmixed(k, j);
		}
		//      printf("%s  -> %d \n",word1[j-1].letter,word1[j-1].type);
	};
	//      printf("\n");
	return (j);
}

/* function processmixed. Called by main 
 */

int processmixed(int i, int j)
{
	if (strcmp(word[i], "gn")) {
		word1[j].type = 1;
		word1[j++].letter = "g";
		word1[j].type = 2;
		word1[j++].letter = "HH";
		word1[j].type = 1;
		word1[j++].letter = "n";
	} else if (strcmp(word[i], "ny")) {
		word1[j].type = 1;
		word1[j++].letter = "n";
		word1[j].type = 2;
		word1[j++].letter = "HH";
		word1[j].type = 1;
		word1[j++].letter = "y";
	} else if (strcmp(word[i], "Hr")) {
		word1[j].type = 1;
		word1[j++].letter = "r";
		word1[j].type = 0;
		word1[j++].letter = "c5";
	} else if (strcmp(word[i], "Hrr")) {
		word1[j].type = 1;
		word1[j++].letter = "r";
		word1[j].type = 0;
		word1[j++].letter = "c6";
	} else if (strcmp(word[i], "00")) {	/* Visarga handled here */
		word1[j].type = 1;
		word1[j++].letter = "h";
	} else if (strcmp(word[i], "0")) {	/* Anuswara handled here */
		if (strlen(word) < (i + 1))
			if (strcmp(word[i + 1], "k") ||
			    strcmp(word[i + 1], "kh") ||
			    strcmp(word[i + 1], "g") ||
			    strcmp(word[i + 1], "gh") ||
			    strcmp(word[i + 1], "ch") ||
			    strcmp(word[i + 1], "chh") ||
			    strcmp(word[i + 1], "j") ||
			    strcmp(word[i + 1], "jh") ||
			    strcmp(word[i + 1], "t") ||
			    strcmp(word[i + 1], "th") ||
			    strcmp(word[i + 1], "d")
			    || strcmp(word[i + 1], "dh")) {
				word1[j].type = 1;
				word1[j++].letter = "n";
			}
		if (strcmp(word[i + 1], "tt") ||
		    strcmp(word[i + 1], "tth") ||
		    strcmp(word[i + 1], "dd") ||
		    strcmp(word[i + 1], "ddh") || strcmp(word[i + 1], "nna")) {
			word1[j].type = 1;
			word1[j++].letter = "nna";
		}
		if (strcmp(word[i + 1], "p") ||
		    strcmp(word[i + 1], "f") ||
		    strcmp(word[i + 1], "b") ||
		    strcmp(word[i + 1], "bh") ||
		    strcmp(word[i + 1], "m") ||
		    strcmp(word[i + 1], "y") ||
		    strcmp(word[i + 1], "r") ||
		    strcmp(word[i + 1], "l") ||
		    strcmp(word[i + 1], "ll") ||
		    strcmp(word[i + 1], "v") ||
		    strcmp(word[i + 1], "sh") ||
		    strcmp(word[i + 1], "s") || strcmp(word[i + 1], "h")) {
			word1[j].type = 1;
			word1[j++].letter = "m";
		}
	}
	return (j);
}

char *kn_breakit(int last)
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

	/* store positions of cvs in cvpos array cvflag is 1 at c & 2 at v in a cv */
	for (i = 0; i < last + 5; i++) {
		if (i + 1 < last && word1[i].type == 1
		    && word1[i + 1].type == 0) {
			cvflag[i] = 1;
			cvflag[i + 1] = 2;
			i++;
		} else {
			cvflag[i] = 0;
		}
	}
	i = 0;
	for (i = last - 1; i >= 0; i--) {
		dcr = 0;
		hf = 0;
		if (cvflag[i] == 2) {
			strcpy(lsyl, "\0");
			strcat(lsyl, kn_getvowel(word1[i].letter));
			strcat(lsyl, syllable);
			strcpy(syllable, lsyl);
		} else if (cvflag[i] == 1) {
			strcpy(lsyl, word1[i].letter);
			strcat(lsyl, syllable);
			strcpy(syllable, lsyl);

			strcpy(lsyl, " ");
			strcat(lsyl, syllable);
			strcpy(syllable, lsyl);

		} else {
			if ((i >= 1) && (word1[i].type == 2)
			    && (word1[i - 1].type == 1)) {
				/* there is a cH */

				if (cvflag[i + 1]
				    || ( word1[i + 1].letter != NULL && *(syllable +
					 strlen(word1[i + 1].letter) + 1) ==
				    '1')) {
					/* if cv or c1 follows current conH */
					itr = 0;
					strcpy(t_half, word1[i - 1].letter);
					strcat(t_half, word1[i + 1].letter);
					while (!hf && itr++ < (no_of_halfs - 1))
						if (!strcmp
						    (kn_halfs[itr], t_half))
							hf = 1;
					if (hf) {
						strcpy(lsyl,
						       word1[i - 1].letter);
						strcat(lsyl, "H");
						strcat(lsyl, ++syllable);
						strcpy(syllable, lsyl);

						strcpy(lsyl, " ");
						strcat(lsyl, syllable);
						strcpy(syllable, lsyl);

						dcr = 1;
					}
				}
				if (i > 2 && word1[i - 2].type == 2
				    && word1[i - 3].type == 1
				    && !(cvflag[i - 2] == 2
					 && cvflag[i - 3] == 1)
				    && !hf) {
					/* there is a cH behind cH giving  0c */
					strcpy(lsyl, "0");
					strcat(lsyl, word1[i - 1].letter);
					strcat(lsyl, syllable);
					strcpy(syllable, lsyl);

					strcpy(lsyl, " ");
					strcat(lsyl, syllable);
					strcpy(syllable, lsyl);

					dcr = 1;
				} else if (i > 1 && word1[i - 2].type == 1
					   && !hf) {
					/* there is a c behind cH giving c1c */
					strcpy(lsyl, word1[i - 2].letter);
					strcat(lsyl, "1");
					strcat(lsyl, word1[i - 1].letter);
					strcat(lsyl, syllable);
					strcpy(syllable, lsyl);

					strcpy(lsyl, " ");
					strcat(lsyl, syllable);
					strcpy(syllable, lsyl);

					dcr = 2;
				} else if (i > 1 && word1[i - 2].type == 0
					   && !hf) {
					/* there is a v behind cH giving vc */
					if (*word1[i - 2].letter == 'c') {
						strcpy(lsyl,
						       word1[i - 3].letter);
						dcr = 1;
					} else {
						strcpy(lsyl, "");
					}
					strcat(lsyl,
					       kn_getvowel(word1[i - 2].
							   letter));
					strcat(lsyl, word1[i - 1].letter);
					strcat(lsyl, syllable);
					strcpy(syllable, lsyl);

					strcpy(lsyl, " ");
					strcat(lsyl, syllable);
					strcpy(syllable, lsyl);

					dcr = 2 + dcr;
				} else if (i == 1 && word1[i].type == 2
					   && word1[i - 1].type == 1 && !hf) {
					/* There is nothing behind cH hence 0c */
					strcpy(lsyl, "0");
					strcat(lsyl, word1[i - 1].letter);
					strcat(lsyl, syllable);
					strcpy(syllable, lsyl);

					strcpy(lsyl, " ");
					strcat(lsyl, syllable);
					strcpy(syllable, lsyl);
					dcr = 2;
				}
			} else if (word1[i].type == 1 && !hf) {
				/* There is a c. so just op c1 */
				strcpy(lsyl, word1[i].letter);
				strcat(lsyl, "1");
				strcat(lsyl, syllable);
				strcpy(syllable, lsyl);

				strcpy(lsyl, " ");
				strcat(lsyl, syllable);
				strcpy(syllable, lsyl);

			} else if (word1[i].type == 0 && !hf) {
				/* There is a v. so just op v */
				strcpy(lsyl, word1[i].letter);
				strcat(lsyl, syllable);
				strcpy(syllable, lsyl);

				strcpy(lsyl, " ");
				strcat(lsyl, syllable);
				strcpy(syllable, lsyl);
			}
		}
		i -= dcr;
	}
	return (syllable);
}

char *kn_getvowel(char *t)
{
	if (t[0] == 'c') {
		return (++t);
	} else {
		return (t);
	}
}

char *kn_parsenum(char *numstr)
{

	char *singles[] =
	    { "s11n n7", "11n d5", "7 r1 dd5", "m6 r5", "n2l k5", "10 d5",
		"2 r5",
		"8 ll5", "7nna tt5", "11m bh1t t5"
	};
	char *teens[] =
	    { "h1t t5", "h1n n11n d5", "h1n n7 r1 dd5", "h1 d3 m6 r5",
		"h1 d3 n2l k5",
		"h1 d3 n10 d5", "h1 d3 n2 r5", "h1 d3 n8 ll5",
		    "h1 d3 n7nna tt5",
		"h1t t11m bh1t t5"
	};
	char *tens[] =
	    { " ", " ", "3p p1t t1", "m6 v1t t1", "n1 l1 v1t t1", "10 v1t t1",
		"1 r1 v1t t1", "7p p1t t1", "7m bh1t t1", "t11m bh1t t1"
	};
	char *hundreds[] =
	    { "", "n6 r1", "3n n6 r1", "m5n n6 r1", "n2 n6 r1", "10 n6 r1",
		"2 r5 n6 r1", "8 ll5 n6 r1", "7nna tt5 n6 r1", "11m bh10 n6 r1"
	};
	int place, i, inr;
	char *number, *tmp;
	place = strlen(numstr);
	number = (char *)malloc(100 * sizeof(char));
	number[0] = '\0';
	tmp = (char *)malloc(20 * sizeof(char));

	for (i = 0; i < place; i++) {
		inr = 0;
		//      while (numstr[i] == '0')
		//      i++;                    /* remove leading spaces */
		if (place - i == 9) {
			strncpy(tmp, numstr + i, 2);
			strcat(number, kn_parsenum(tmp));
			strcat(number, " k12 tt3 ");
			inr = 1;
		} else if (place - i == 8) {
			strcat(number, singles[numstr[i] - 0]);
			strcat(number, " k12 tt3 ");
		} else if (place - i == 7) {
			strncpy(tmp, numstr + i, 2);
			strcat(number, kn_parsenum(tmp));
			if (numstr[i + 2] == '0' && numstr[i + 3] == '0'
			    && numstr[i + 4] == '0' && numstr[i + 5] == '0'
			    && numstr[i + 6] == '0') {
				strcat(number, " l1 kHsh1 ");
				inr = 6;
			} else {
				strcat(number, " l1 kHsh1 d1 ");
				inr = 1;
			}
		} else if (place - i == 6) {
			if (numstr[i + 1] == '0' && numstr[i + 2] == '0'
			    && numstr[i + 3] == '0' && numstr[i + 4] == '0'
			    && numstr[i + 5] == '0') {
				strcat(number, singles[numstr[i] - 0]);
				strcat(number, " l1 kHsh1 ");
			} else {
				strcat(number, singles[numstr[i] - 0]);
				strcat(number, " l1 kHsh1 d1 ");
			}
		} else if (place - i == 5) {

			strncpy(tmp, numstr + i, 2);
			strcat(number, kn_parsenum(tmp));
			if (numstr[i + 2] == '0' && numstr[i + 3] == '0'
			    && numstr[i + 4] == '0') {
				strcat(number, " s2 v3 r1 ");
				inr = 3;
			} else {
				strcat(number, " s2 v3 r1 d1 ");
				inr = 1;
			}
		} else if (place - i == 4) {
			if (numstr[i + 1] == '0' && numstr[i + 2] == '0'
			    && numstr[i + 3] == '0') {
				strcat(number, singles[numstr[i] - 0]);
				strcat(number, " s2 v3 r1 ");
			} else {
				strcat(number, singles[numstr[i] - 0]);
				strcat(number, " s2 v3 r1 d1 ");
			}
		} else if (place - i == 3) {
			if (numstr[i + 1] == '0' && numstr[i + 2] == '0') {
				strcat(number, hundreds[numstr[i] - 0]);
				number[strlen(number) - 1] = '5';
				inr = 2;
			} else if (numstr[i + 1] == 0) {
				strcat(number, hundreds[numstr[i] - 0]);
				inr = 1;
			} else {
				strcat(number, hundreds[numstr[i] - 0]);
			}
			strcat(number, " ");
		} else if (place - i == 2) {
			if (numstr[i] == '1') {
				strcat(number, teens[numstr[i + 1] - 0]);
				inr = 1;
			} else if (numstr[i + 1] == '0') {
				strcat(number, tens[numstr[i] - 0]);
				number[strlen(number) - 1] = '5';
				inr = 1;
			} else {
				strcat(number, tens[numstr[i] - 0]);
			}
			strcat(number, " ");
		} else if (place - i == 1) {
			strcat(number, singles[numstr[i] - 0]);
			strcat(number, " ");
		}
		i += inr;
		strcat(number, " G1500 ");
	}
	return (number);
}
