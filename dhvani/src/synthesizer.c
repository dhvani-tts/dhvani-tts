/* synthesizer.c
 *
 * Copyright (C) 2007-2008
 *  Santhosh Thottingal <santhosh.thottingal@gmail.com>, 
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

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <gsm/private.h>
#include <gsm/gsm.h>
#include <math.h>
#include <string.h>
#include "dhvani_lib.h"
#include "debug.h"
#include "synthesizer.h"
#include "languages.h"
#include  "alsa_player.h"
#include  "UTF8Decoder.h"
#include <assert.h>
#ifdef WITH_OGGVORBIS
#include "oggencoder.h"
#endif
#ifdef  WITH_SOUNDTOUCH
#include "soundtouch_utils.h"
#endif
#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x
#ifndef DATABASEPATH
#define DATABASEPATH /usr/share/dhvani/database/
#endif

/*upper bounds on number of sounds------------------------------------------*/
#define numsounds 800		/*total number of sounds */
#define numc 34			/*total number of consonants */
#define numv  15		/*total number of vowels */
#define numhalfs 61		/*total number of half sounds */

#define DHVANI_SPACE_DELAY " G3000 "
#define DHVANI_LINE_DELAY " G5000 "

dhvani_options *options;
float delta_pitch = 0.0;
float delta_tempo = 10.0;
int rate = 2;
int playing = 0;
/*default vowel duration values for 8KHz, for vowels 1..15 in order;
see ../doc folder for mapping of vowels to numbers; the 0th entry 
below is a dummy------------------------------------------------------------*/
int vdefault[16] =
    { 0, 800, 1700, 800, 1600, 800, 1500, 1000, 1600, 1600, 1500, 900,
	1700,
	1500, 1600, 700
};

/*some consonants like k,t,tt,p etc are such that is a sound begins with
this consonant and the previous sound is a cvc or hcvc having 
r,l,ll etc as their last consonant then a gap of
delaygap*rate is inserted before this consonant is pronounced; 
the delayconsonants are listed in the following array (see ../doc/algo 
for the mapping of consonants to numbers); currently 7 consonants have been
identified as delay consonants; the gap is introduced when theu are preceded
by prevdelay consonants ----------------------------------------------*/
int delayconsonants[7] = { 1, 5, 9, 10, 14, 15, 19
};

int numdelayconsonants = 7;
int prevdelayconsonants[9] = { 24, 25, 26, 27, 28, 29, 30, 31, 32
};

int numprevdelayconsonants = 9;
int delaygap = 600;
int silent = 0;
int speed = 0;

/*filenames----------------------------------------------------------------*/
char *pathname = STRINGIFY(DATABASEPATH) ;	/*pathname to get to the database directory */
char *output_file;
int text_position = 0;

/*callback function*/
t_dhvani_synth_callback *synth_callback_fn = NULL;
t_dhvani_audio_callback *audio_callback_fn = NULL;

char cvpathname[50];		/*will store pathname/cv/ */
char vcpathname[50];		/*will store pathname/vc/ */
char vpathname[50];		/*will store pathname/v/ */
char cpathname[50];		/*will store pathname/c/ */
char halfspathname[50];		/*will store pathname/halfs/ */
char noisepathname[50];		/*will store pathname/noise, used for gap filling */

char cvoffsets[50];		/*will store pathname/cvoffsets */
char voffsets[50];		/*will store pathname/vcoffsets */
char vcoffsets[50];		/*will store pathname/voffsets */
char halfsoffsets[50];		/*will store pathname/hoffsets */

/*array structures containing sound names and features
these arrays will store the attributes of the various sounds; these attributes
are stored in the offsets files in the pathname directory and will be read
into the arrays listed below when the synthesiser is invoked. The cv
and vc sounds are indexed by two numbers, one corresponding to the vowel
and one corresponding to the consonant.-------------------------------------*/

struct cvsounds {
	int start;
	int diphst;
	int ctov;
	int longvowlen;
	int shortvowlen;
	int diphend;
	float diphshortfactor;
	int halfst;
};
struct cvsounds cv[numc + 1][numv + 1];

struct vcsounds {
	int end;
	int diphend;
	int vtoc;
	int longvowlen;
	int shortvowlen;
	int diphst;
};
struct vcsounds vc[numv + 1][numc + 1];

struct vsounds {
	int length;
};
struct vsounds vowel[numv + 1];

struct halfssounds {
	int cons1;
	int cons2;
	int start;
	int end;
};
/*cons1 and cons2 are the two component consonants of the half */
struct halfssounds halfs[numhalfs];
int halfsindex = 0;

/*-----------------------------------------------------------------------*/
/*global variables*/
snd_pcm_t *handle; /* the variable for the sound device,  */
struct soundtouch *sound_touch;

/*structure describing sound types------------------------------------------
When a sound (i.e., vc,cv,v,c,hcv,hcvc,gap) is read from the input, 
it is analysed and broken into components. The following structure
is designed to store these components. The fields have the following
meaning. If the sound is a gap sound then gap is the value of the
gap length. The consonants in the sound are stored in cons1 and cons2.
The vowel is stored in vow. The half-consonant, if any, is stored 
in half. Leftcut is an indicator which is set to 1 if the current
sound must be played as a diphone from the left (i.e., from middle of the 
consonant, not from the start of the consonant). Rightcut is the analogous
indicator for the right. 
Prevcons is the last cons in the previous sound and is useful
in deciding whether to introduce a delay or not between this
sound and the previous one.
Halfsig, halfsigst and halfsig end will be 
used only for hcv and hcvc sounds; these fields are used for the
following purely technical reason. To process an hcv sound, we first
process the half and then make a call to the procedure which processes
cv sound; this call will pass the attributes of the halfs in the above
three fields.------------------------------------------------------------*/

struct sndtype {
	int ok;
	int gap;
	int half;
	int cons1;
	int cons2;
	int vow;
	int prevcons;
	int leftcut;
	int rightcut;
	short *halfsig;
	int halfsigst;
	int halfsigend;
};

/*-----------------------------------------------------------------------*/
/*global variables*/
int sndfd;			/* the variable for the sound device, currently it is /dev/dsp */
char *splitarr[1000];		/*array in which split will return, max size is 1000 */
int maxsignalsize = 20000;	/* the largest size of an uncompresses sound unit */
int maxmarkssize = 200;		/* max number of pitch marks in a sound */
short noise[600];		/*array in which noise for gap filling is stored */

/*-----------------------------------------------------------------------
This procedure just initializes the pathnames for database and offset
files.
-------------------------------------------------------------------------*/
void initialize_pathnames()
{
	FILE *fd;
	int i;

	strcpy(cvpathname, pathname);
	strcat(cvpathname, "cv/");

	strcpy(cvoffsets, pathname);
	strcat(cvoffsets, "cvoffsets");

	strcpy(vcpathname, pathname);
	strcat(vcpathname, "vc/");

	strcpy(vcoffsets, pathname);
	strcat(vcoffsets, "vcoffsets");

	strcpy(vpathname, pathname);
	strcat(vpathname, "v/");

	strcpy(voffsets, pathname);
	strcat(voffsets, "voffsets");

	strcpy(halfspathname, pathname);
	strcat(halfspathname, "halfs/");

	strcpy(halfsoffsets, pathname);
	strcat(halfsoffsets, "hoffsets");

	strcpy(cpathname, pathname);
	strcat(cpathname, "c/");

	strcpy(noisepathname, pathname);
	strcat(noisepathname, "noise");

	fd = fopen(noisepathname, "r");
	i = 0;
	while (fread(&noise[i], sizeof(noise[0]), 1, fd)) {
		i = i + 1;
	};
	fclose(fd);

}

/*-----------------------------------------------------------------------
Checks if  cons is indeed a delaycons (see above for explanation).
-------------------------------------------------------------------------*/
int delaycons(int cons)
{
	int k = 0;
	for (k = 0; k < numdelayconsonants; ++k) {
		if (delayconsonants[k] == cons) {
			return (1);
		}
	};
	return (0);
}

/*-----------------------------------------------------------------------
Checks if  cons is indeed a prevdelaycons (see above for explanation).
-------------------------------------------------------------------------*/
int prevdelaycons(int cons)
{
	int k = 0;
	for (k = 0; k < numprevdelayconsonants; ++k) {
		if (prevdelayconsonants[k] == cons) {
			return (1);
		}
	};
	return (0);
}

/*-----------------------------------------------------------------------
Converts a given consonant written as a alphabetic string to its
corresponding number.
-------------------------------------------------------------------------*/
int getconsnum(char *s)
{
	if (strcmp(s, "k") == 0) {
		return (1);
	} else if (strcmp(s, "kh") == 0) {
		return (2);
	} else if (strcmp(s, "g") == 0) {
		return (3);
	} else if (strcmp(s, "gh") == 0) {
		return (4);
	} else if (strcmp(s, "ch") == 0) {
		return (5);
	} else if (strcmp(s, "chh") == 0) {
		return (6);
	} else if (strcmp(s, "j") == 0) {
		return (7);
	} else if (strcmp(s, "jh") == 0) {
		return (8);
	} else if (strcmp(s, "t") == 0) {
		return (9);
	} else if (strcmp(s, "th") == 0) {
		return (10);
	} else if (strcmp(s, "d") == 0) {
		return (11);
	} else if (strcmp(s, "dh") == 0) {
		return (12);
	} else if (strcmp(s, "n") == 0) {
		return (13);
	} else if (strcmp(s, "tt") == 0) {
		return (14);
	} else if (strcmp(s, "tth") == 0) {
		return (15);
	} else if (strcmp(s, "dd") == 0) {
		return (16);
	} else if (strcmp(s, "ddh") == 0) {
		return (17);
	} else if (strcmp(s, "nna") == 0) {
		return (18);
	} else if (strcmp(s, "p") == 0) {
		return (19);
	} else if (strcmp(s, "f") == 0) {
		return (20);
	} else if (strcmp(s, "b") == 0) {
		return (21);
	} else if (strcmp(s, "bh") == 0) {
		return (22);
	} else if (strcmp(s, "m") == 0) {
		return (23);
	} else if (strcmp(s, "y") == 0) {
		return (24);
	} else if (strcmp(s, "r") == 0) {
		return (25);
	} else if (strcmp(s, "l") == 0) {
		return (26);
	} else if (strcmp(s, "ll") == 0) {
		return (27);
	} else if (strcmp(s, "v") == 0) {
		return (28);
	} else if (strcmp(s, "sh") == 0) {
		return (29);
	} else if (strcmp(s, "s") == 0) {
		return (30);
	} else if (strcmp(s, "h") == 0) {
		return (31);
	} else if (strcmp(s, "zh") == 0) {
		return (32);
	} else if (strcmp(s, "z") == 0) {
		return (33);
	} else if (strcmp(s, "an") == 0) {
		return (34);
	} else {
		dhvani_debug("INVALID CONSONANT: %s\n", s);
		return (1);
	};

}

/*-----------------------------------------------------------------------
given an string, it behaves like the PERL split routine splitting
the string into tokens comprising characters A_Z,a-z,0-9,.,-
The various tokens are pointed to by successive locations in the
array splitarr.
-----------------------------------------------------------------------*/

int split(char *s)
{
	int i, j, k;
	j = 0;
	i = 0;

	while (s[i] != '\0') {
		k = i;
		while ((s[i] >= 'A' && s[i] <= 'Z')
		       || (s[i] >= 'a' && s[i] <= 'z')
		       || (s[i] >= '0' && s[i] <= '9') || (s[i] == '.')
		       || (s[i] == '-')) {
			++i;
		}
		if (i > k) {
			splitarr[j] =
			    (char *)(malloc(sizeof(char) * (i - k + 1)));
			strncpy(splitarr[j], &s[k], i - k);
			splitarr[j][i - k] = '\0';
			j++;
		} else {
			i++;
		};
	}
	return (j);
}

/*-----------------------------------------------------------------------
Read the cvoffsets files and fill in the appropriate information
in array cv; take note of the default values being set below if the
cvoffsets file has an xxx in it.
-------------------------------------------------------------------------*/
void readcvoffsets()
{
	FILE *fd;
	int c;
	char tmp[100];
	int i = 0;
	int j = 0;
	int c1, v, k;

	fd = fopen(cvoffsets, "r");

	/* while ((c=getc(fd))!='\n') {;};
	   while ((c=getc(fd))!='\n') {;}; */

	while (((c = getc(fd)) != EOF) && (c != '\n')) {
		i = 0;
		while ((c != EOF) && (c != '\n')) {
			tmp[i++] = c;
			c = getc(fd);
		};
		tmp[i] = '\0';
		j = split(tmp);

		k = 0;
		while (splitarr[0][k] != '.') {
			k++;
		};
		splitarr[0][k] = '\0';
		c1 = atoi(splitarr[0]);
		k++;
		v = atoi(&splitarr[0][k]);

		cv[c1][v].start = atoi(splitarr[1]);
		cv[c1][v].ctov = atoi(splitarr[3]);
		if (splitarr[2][0] != 'x') {
			cv[c1][v].diphst =
			    ((-1 * cv[c1][v].start + cv[c1][v].ctov) / 2.5) +
			    cv[c1][v].start;
		} else {
			cv[c1][v].diphst =
			    ((-1 * cv[c1][v].start + cv[c1][v].ctov) / 2.5) +
			    atoi(splitarr[2]) + cv[c1][v].start;
		};

		if (splitarr[4][0] != 'x') {
			cv[c1][v].longvowlen = vdefault[v] * rate;
		} else {
			cv[c1][v].longvowlen =
			    vdefault[v] * rate + atoi(splitarr[4]);
		};

		if (splitarr[5][0] != 'x') {
			cv[c1][v].shortvowlen = vdefault[v - 1] * rate;
		} else {
			cv[c1][v].shortvowlen =
			    vdefault[v - 1] * rate + atoi(splitarr[5]);
		};

		if (splitarr[6][0] != 'x') {
			cv[c1][v].diphend =
			    cv[c1][v].ctov + ((cv[c1][v].longvowlen / 2.5));
		} else {
			cv[c1][v].diphend =
			    (cv[c1][v].ctov + (cv[c1][v].longvowlen) / 2.5) +
			    atoi(splitarr[6]);
		};

		if (splitarr[7][0] != 'x') {
			cv[c1][v].diphshortfactor = 0.5;
		} else {
			cv[c1][v].diphshortfactor = atoi(splitarr[7]) / 10.0;
		};

		if (splitarr[8][0] != 'x') {
			cv[c1][v].halfst = cv[c1][v].diphst;
		} else {
			cv[c1][v].halfst = atoi(splitarr[8]) + cv[c1][v].diphst;
		};

	}
	fclose(fd);
}

/*-----------------------------------------------------------------------
Read the vcoffsets files and fill in the appropriate information
in array vc; take note of the default values being set below if the
vcoffsets file has an xxx in it.
-------------------------------------------------------------------------*/
void readvcoffsets()
{
	FILE *fd;
	int c;
	char tmp[400];
	int i = 0;
	int j = 0;
	int c1, v, k;

	fd = fopen(vcoffsets, "r");

	while (((c = getc(fd)) != EOF) && (c != '\n')) {
		i = 0;
		while ((c != EOF) && (c != '\n')) {
			tmp[i++] = c;
			c = getc(fd);
		};
		tmp[i] = '\0';
		j = split(tmp);

		if (j > 0) {
			k = 0;
			while (splitarr[0][k] != '.') {
				k++;
			};
			splitarr[0][k] = '\0';
			v = atoi(splitarr[0]);
			k++;
			c1 = atoi(&splitarr[0][k]);

			vc[v][c1].end = atoi(splitarr[1]);
			vc[v][c1].vtoc = atoi(splitarr[3]);

			if (splitarr[2][0] != 'x') {
				vc[v][c1].diphend =
				    ((vc[v][c1].end - vc[v][c1].vtoc) / 2.5) +
				    vc[v][c1].vtoc;
			} else {
				vc[v][c1].diphend =
				    vc[v][c1].vtoc +
				    ((vc[v][c1].end - vc[v][c1].vtoc) / 2.5) +
				    atoi(splitarr[2]);
			};

			if (splitarr[4][0] != 'x') {
				vc[v][c1].longvowlen = vdefault[v] * rate;
			} else {
				vc[v][c1].longvowlen =
				    vdefault[v] * rate + atoi(splitarr[4]);
			};

			if (splitarr[5][0] != 'x') {
				vc[v][c1].shortvowlen = vdefault[v - 1] * rate;
			} else {
				vc[v][c1].shortvowlen =
				    vdefault[v - 1] * rate + atoi(splitarr[5]);
			};

			if (splitarr[6][0] != 'x') {
				vc[v][c1].diphst =
				    vc[v][c1].vtoc -
				    ((vc[v][c1].longvowlen) / 2.5);
			} else {
				vc[v][c1].diphst =
				    vc[v][c1].vtoc -
				    ((vc[v][c1].longvowlen) / 2.5) +
				    atoi(splitarr[6]);
			};

		}

	}

	fclose(fd);
}

/*-----------------------------------------------------------------------
Read the voffsets files and fill in the appropriate information
in array vowel; take note of the default values being set below if the
voffsets file has an xxx in it.
-------------------------------------------------------------------------*/
void readvoffsets()
{
	FILE *fd;
	int c;
	char tmp[100];
	int i = 0;
	int j = 0;
	int v;

	fd = fopen(voffsets, "r");

	while (((c = getc(fd)) != EOF) && (c != '\n')) {
		i = 0;
		while ((c != EOF) && (c != '\n')) {
			tmp[i++] = c;
			c = getc(fd);
		};
		tmp[i] = '\0';
		j = split(tmp);

		v = atoi(splitarr[0]);

		if (splitarr[1][0] != 'x') {
			vowel[v].length = vdefault[v] * rate;
		} else {
			vowel[v].length =
			    vdefault[v] * rate + atoi(splitarr[1]);
		};

	}
	fclose(fd);
}

/*-----------------------------------------------------------------------
Read the hoffsets files and fill in the appropriate information
in array halfs;
-------------------------------------------------------------------------*/
void readhoffsets()
{
	FILE *fd;
	int c;
	char tmp[100];
	int i = 0;
	int j = 0;
	int c1, c2, k;

	fd = fopen(halfsoffsets, "r");

	while (((c = getc(fd)) != EOF) && (c != '\n')) {
		i = 0;
		while ((c != EOF) && (c != '\n')) {
			tmp[i++] = c;
			c = getc(fd);
		};
		tmp[i] = '\0';
		j = split(tmp);

		k = 0;
		while (splitarr[0][k] != '.') {
			k++;
		};
		splitarr[0][k] = '\0';
		c1 = atoi(splitarr[0]);
		k++;
		c2 = atoi(&splitarr[0][k]);

		halfs[halfsindex].cons1 = c1;
		halfs[halfsindex].cons2 = c2;
		halfs[halfsindex].start = atoi(splitarr[1]);
		halfs[halfsindex].end = atoi(splitarr[2]);
		halfsindex++;
	}
	fclose(fd);
}

/*-----------------------------------------------------------------------
Gets pitchmarks from s.gsm.marks, stored as one mark per byte
and puts them in outmarks. Returns size of output.
-------------------------------------------------------------------------*/
int getmarks(char *s, unsigned char *outmarks)
{
	FILE *fd;
	int i = 0;

	strcat(s, ".gsm.marks");
	fd = fopen(s, "r");
	while (fread(&outmarks[i], sizeof(outmarks[0]), 1, fd)) {
		i = i + 1;
	};
	fclose(fd);

	return (i);
}

/*-----------------------------------------------------------------------
Pitch Marking is explained in the ../doc/algo file. This procedure,
given the name s of a marks file, and an integer n will get the
that pitch mark which is just after n.
-------------------------------------------------------------------------*/
int getnearestmark(char *s, int n)
{
	unsigned char outmarks[maxmarkssize];
	int i, j, sum;

	i = getmarks(s, outmarks);
	j = 0;
	sum = 0;
	while (j < i && sum < n) {
		sum += outmarks[j++];
	};
	return (sum);
}

/*-----------------------------------------------------------------------
Given a string s representing a sound (one of cv, cvc, v, c, hcv, hcvc, gap) 
this procedure pulls out all the parameters explained in the definition
of the soundtype structure above.
-------------------------------------------------------------------------*/
struct sndtype gettype(char *s)
{
	struct sndtype type;
	int i, j;
	unsigned char foundvowel = 0;
	char temp[10];

	type.ok = 1;
	type.halfsig =0;
	type.gap = 0;
	type.half = 0;
	type.cons1 = 0;
	type.cons2 = 0;
	type.vow = 0;
	type.leftcut = 0;
	type.prevcons = 0;
	type.rightcut = 0;
	type.halfsigend = -1;
	type.halfsigst = -1;

	i = 0;

	while (s[i] != '\0' && s[i] != 'H') {
		i++;
	};
	if (s[i] != 'H') {
		i = 0;
	};

	while (s[i] != '\0') {
		if (s[i] == 'H') {
			j = 0;
			strncpy(temp, &s[j], i - j);
			temp[i - j] = '\0';
			if ((type.half = getconsnum(temp)) == -1) {
				type.ok = 0;
				return type;
			};
			i++;
		} else if (s[i] >= '0' && s[i] <= '9') {
			j = i;
			while (s[i] >= '0' && s[i] <= '9') {
				i++;
			};
			strncpy(temp, &s[j], i - j);
			temp[i - j] = '\0';
			type.vow = atoi(temp);
			if (type.vow > 15 || type.vow < 0) {
				type.ok = 0;
				return type;
			};
			foundvowel = 1;
		} else if (s[i] >= 'a' && s[i] <= 'z') {
			j = i;
			while (s[i] >= 'a' && s[i] <= 'z') {
				i++;
			};
			if (foundvowel) {
				strncpy(temp, &s[j], i - j);
				temp[i - j] = '\0';
				if ((type.cons2 = getconsnum(temp)) == -1) {
					type.ok = 0;
					return type;
				};
			} else {
				strncpy(temp, &s[j], i - j);
				temp[i - j] = '\0';
				if ((type.cons1 = getconsnum(temp)) == -1) {
					type.ok = 0;
					return type;
				};
			}
		} else if (s[i] == 'G') {
			i++;
			j = i;
			while (s[i] >= '0' && s[i] <= '9') {
				i++;
			};
			type.gap = atoi(&s[j]);
		}
	};

	return type;
}

/*-----------------------------------------------------------------------
Determines whether a given vowel i is long or short. Essentially
used because we are obtaining short vowels by cutting them down 
from long ones.
- -----------------------------------------------------------------------*/
unsigned char isshort(int i)
{
	if (i == 1 || i == 3 || i == 5 || i == 7 || i == 11) {
		return (1);
	} else {
		return (0);
	};
}

/*-----------------------------------------------------------------------
Given two consonants eg the numbers corresponding to p and r,
it linearly searches through the halfs array to locate where 
the attributes for the half pr read from the hoffsets file are
stored.
-------------------------------------------------------------------------*/
int findhalfsindex(int cons1, int cons2)
{
	int i;
	for (i = 0; i < halfsindex; ++i) {
		if ((halfs[i].cons1 == cons1) && (halfs[i].cons2 == cons2)) {
			return (i);
		};
	}
	dhvani_debug("invalid half sound in input");
	return 0;

}

/*-----------------------------------------------------------------------
Used to get a short vowel from a long one in a cv sound. 
Given a signal[0..end], it applies a simple linear decaying envelope
from end- rate*300 to end. The parameters mid and size are redundant
but have not yet been removed.
--------------------------------------------------------------------------*/

void rightwindow(short *signal, int mid, int end, int size)
{
	int j;
	float tmp;
	int decaywindow;

	decaywindow = rate * 300;
	if (mid < 0) {
		mid = 0;
	};
	if (end < 0) {
		end = size;
	};
	for (j = end - decaywindow; j < end; ++j) {
		tmp = (end - j);
		tmp = (tmp / decaywindow);
		signal[j] = (signal[j] * tmp + .5);
	};
}

/*-----------------------------------------------------------------------
Used to get a short vowel from a long one in a vc sound. 
Given a signal[start..end], it applies a simple linear decaying envelope
from start to start+rate*300. The parameters mid and end are redundant
but have not yet been removed.
--------------------------------------------------------------------------*/

void leftwindow(short *signal, int start, int mid, int end)
{
	int j;
	float tmp;
	int decaywindow;

	decaywindow = rate * 300;
	for (j = start; j < start + decaywindow; ++j) {
		tmp = j - start;
		tmp = (tmp / decaywindow);
		signal[j] = (signal[j] * tmp + .5);
	};
}

/*
 *Get a unique file name for this process by using process id
 */
char *get_tempfile_name(int type)
{
	char *tempfile_name;
	tempfile_name = (char *)malloc(25);
	assert(tempfile_name);
	/*construct the temperory file name. since this is going to be unique among apps, maked 
	   any number  of dhvani instances run parallelly */
	if (type == 1) {
		sprintf(tempfile_name, "/tmp/dhvani-st%d", getpid());
	} else {
		sprintf(tempfile_name, "/tmp/dhvani%d", getpid());
	}
	return tempfile_name;
}

/*-----------------------------------------------------------------------
Writes out signal from start to end to the sound device
If end is negative end will be taken to be the same as size. 
Some noise is added to the signal before writing out.
-------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
Writes out signal from start to end to the sound device
If end is negative end will be taken to be the same as size. 
Some noise is added to the signal before writing out.
-------------------------------------------------------------------------*/
void
output(short *signal, int start, int end, int size)
{
    int i = 0;
    FILE *out_file;
    short data_unit;
    int sample_count = 0;
    if (end < 0) {
        end = size;
    };
    out_file = fopen(get_tempfile_name(1), "a"); /*open it in append mode */
    /*this temp file stores the sound for one unit
       playback. direct input to alsa omitted because of underrun
       buffer starvation and thereby causing noises  */

    for (i = start; i < end; ++i) {
        data_unit = floor(noise[sample_count % 500] / 3) + signal[i];
        sample_count++;
        fwrite(&data_unit, sizeof(signal[0]), 1, out_file);
    }

    fclose(out_file);



}

/*-----------------------------------------------------------------------
Processing Gap sounds: just play 
-------------------------------------------------------------------------*/
void gap(int gaplen)
{
	short signal[maxsignalsize];
	int i;

	if (gaplen > maxsignalsize) {
		dhvani_debug("ERROR: Gap value longer than 15000");
	};
	for (i = 0; i < gaplen; ++i) {
		signal[i] = 0;
	};
	output(signal, 0, -1, gaplen);
}

/*self-explanatory-------------------------------------------------------*/
void closedev()
{
	  /*alsa_close(handle);*/

}

/*------------------------------------------------------------------------
Gets the file given by s.gsm, uncompresses it and stores in outarr.
returns the output size.
--------------------------------------------------------------------------*/
int getfile(char *s, short *outarr)
{
	
	unsigned char gsmarr[4000];
	int i = 0;
	int gsmsize = 0;
	int outarrindex = 0;
	gsm g;
	FILE *fd;
	strcat(s, ".gsm");
	fd = fopen(s, "r");
	while (fread(&gsmarr[i], sizeof(gsmarr[0]), 1, fd)) {
		i = i + 1;
	};
	fclose(fd);
	gsmsize = i;

	g = gsm_create();
	for (i = 0; i < gsmsize; i += 33) {
		gsm_decode(g, &gsmarr[i], &outarr[outarrindex]);
		outarrindex += 160;
	}

	gsm_destroy(g);

	return (outarrindex);
}

/*-----------------------------------------------------------------------
Processes a cv sound. Steps are explained below.
-------------------------------------------------------------------------*/
void cvo(struct sndtype type)
{
	short signal[maxsignalsize];
	char fname[100];
	int sigsize = 0;
	char tmp[3];

	int end;
	int start;

	int vow;

	/*construct the soundfilename in fname and then get the signal using getfile.
	   short vowels are obtained from the longer ones */

	strcpy(fname, cvpathname);
	sprintf(tmp, "%d", type.cons1);
	strcat(fname, tmp);
	strcat(fname, ".");
	if (isshort(type.vow)) {
		sprintf(tmp, "%d", type.vow + 1);
	} else {
		sprintf(tmp, "%d", type.vow);
	}
	strcat(fname, tmp);
	sigsize = getfile(fname, signal);

	/* Once the signal is at hand, the endpoint up to which it must be 
	   played is determined using the vowellength information. For long
	   vowels we just play the whole thing, instead of using longvowellength.  */

	if (isshort(type.vow)) {
		vow = type.vow + 1;
		end =
		    cv[type.cons1][type.vow + 1].ctov +
		    cv[type.cons1][type.vow + 1].shortvowlen;
	} else {
		vow = type.vow;
		end = -1;
	}

	/* Next, the start point needs to be determined.
	   If this is to be played as a diphone (i.e., from the middle
	   of the consonant) leftcut will be 1;  then construct the filename 
	   to get the pitchmarks nearest to diphst. Otherwise start from 
	   start.
	 */
	strcpy(fname, cvpathname);
	sprintf(tmp, "%d", type.cons1);
	strcat(fname, tmp);
	strcat(fname, ".");
	if (isshort(type.vow)) {
		sprintf(tmp, "%d", type.vow + 1);
	} else {
		sprintf(tmp, "%d", type.vow);
	}
	strcat(fname, tmp);
	if (type.leftcut == 1 || (type.leftcut == 3 && !delaycons(type.cons1))) {
		start = getnearestmark(fname, cv[type.cons1][vow].diphst);
	} else
		/*if (type.leftcut==2 && !delaycons(type.cons1)) {start= getnearestmark(fname, cv[type.cons1][vow].diphst-((cv[type.cons1][vow].diphst-cv[type.cons1][vow].start)*3/4));}
		   else 
		 */  {
		start = cv[type.cons1][vow].start;
		};

	/* Now start,end have been determined; for shortvowels we need 
	   to decay the signal down to 0 at end */
	if (isshort(type.vow)) {
		rightwindow(signal, (cv[type.cons1][vow]).ctov, end, sigsize);
	};

	/* for delayconsonants put a gap */

	if (delaycons(type.cons1) && (type.leftcut == 0 || type.leftcut == 2)
	    && (prevdelaycons(type.prevcons))) {
		gap(rate * delaygap);
	};

	/* =-1 below indicates that this procedure was called from hcvo(),
	   which means a half sound must first be output; this halfsound
	   would already have been constructed by hcvo(); it just needs to
	   be output now */
	if (type.halfsigend != -1) {
		output(type.halfsig, type.halfsigst, type.halfsigend, -1);
	};

	/*output the actual signal */
	output(signal, start, end, sigsize);
}

/*-----------------------------------------------------------------------
Processes a c sound. Just gets and plays out the whole signal.
--------------------------------------------------------------------------*/
void co(struct sndtype type)
{
	short signal[maxsignalsize];
	char fname[100];
	int sigsize;
	char tmp[3];

	strcpy(fname, cpathname);
	sprintf(tmp, "%d", type.cons2);
	strcat(fname, tmp);

	sigsize = getfile(fname, signal);
	output(signal, 0, -1, sigsize);
}

/*-----------------------------------------------------------------------
Processes a vc sound. Steps are explained below.
-------------------------------------------------------------------------*/
void vco(struct sndtype type)
{
	short signal[maxsignalsize];
	char fname[100];
	int sigsize;
	char tmp[3];
	int i, j;
	int end;
	int vow, start;
	int repeatmark = 0;

	/*Construct file name and get signal. Short to long as usual */
	strcpy(fname, vcpathname);
	if (isshort(type.vow)) {
		sprintf(tmp, "%d", type.vow + 1);
	} else {
		sprintf(tmp, "%d", type.vow);
	}
	strcat(fname, tmp);
	strcat(fname, ".");
	sprintf(tmp, "%d", type.cons2);
	strcat(fname, tmp);
	sigsize = getfile(fname, signal);

	/*Determine the start value depending upon the vowel length.
	   For long vowels we just play the whole thing.  */

	if (isshort(type.vow)) {
		vow = type.vow + 1;
		start =
		    vc[type.vow + 1][type.cons2].vtoc - vc[type.vow +
							   1][type.cons2].
		    shortvowlen;
	} else {
		vow = type.vow;
		/*  start=vc[type.vow][type.cons2].vtoc-vc[type.vow][type.cons2].longvowlen; */
		start = 0;

	};

	/*Determine the end value depending upon whether this is to 
	   be played as a diphone or not. Analogous to the cvo() case above */

	strcpy(fname, vcpathname);
	if (isshort(type.vow)) {
		sprintf(tmp, "%d", type.vow + 1);
	} else {
		sprintf(tmp, "%d", type.vow);
	}
	strcat(fname, tmp);
	strcat(fname, ".");
	sprintf(tmp, "%d", type.cons2);
	strcat(fname, tmp);

	/* Rightcut 1 or 3 means treat as diphone, so cut in the middle of the 
	   consonant, Rightcut 2 means the next consonant will be the same as 
	   this one, so special action is required.  */

	if (type.rightcut == 1 || type.rightcut == 3) {
		end = getnearestmark(fname, vc[vow][type.cons2].diphend);
	} else if (type.rightcut == 2 && type.cons2 != 19) {
		end =
		    getnearestmark(fname,
				   vc[vow][type.cons2].diphend +
				   ((vc[vow][type.cons2].end -
				     vc[vow][type.cons2].diphend) * 3 / 4));
	} else {
		end = vc[vow][type.cons2].end;
	};

	if (type.rightcut == 3) {
		strcpy(fname, vcpathname);
		if (isshort(type.vow)) {
			sprintf(tmp, "%d", type.vow + 1);
		} else {
			sprintf(tmp, "%d", type.vow);
		}
		strcat(fname, tmp);
		strcat(fname, ".");
		sprintf(tmp, "%d", type.cons2);
		strcat(fname, tmp);
		repeatmark =
		    getnearestmark(fname, vc[vow][type.cons2].diphend - 200);
	};

	/*start and end are determined now, just window and play out */

	leftwindow(signal, start, vc[vow][type.cons2].vtoc, end);
	output(signal, start, end, end - start);
	if (type.rightcut == 3) {
		if (type.cons2 == 18) {
			j = 7;
		} else {
			j = 8;
		};
		for (i = 0; i <= j; ++i) {
			output(signal, repeatmark, end, end - repeatmark);
		}

	};

}

/*-----------------------------------------------------------------------
Processes a cvc sound. Steps explained below.
-------------------------------------------------------------------------*/
void cvco(struct sndtype type)
{
	short signal1[maxsignalsize];
	short signal2[maxsignalsize];
	char fname[100];
	int sigsize1, sigsize2;
	char tmp[3];

	int i, j;
	int leftmark, rightmark;
	int start1, end1, start2, end2, vow;
	int repeatmark=0;

	/* Construct both the cv and the vc file names and get the signals */
	strcpy(fname, cvpathname);
	sprintf(tmp, "%d", type.cons1);
	strcat(fname, tmp);
	strcat(fname, ".");
	if (isshort(type.vow)) {
		sprintf(tmp, "%d", type.vow + 1);
	} else {
		sprintf(tmp, "%d", type.vow);
	}
	strcat(fname, tmp);
	sigsize1 = getfile(fname, signal1);

	strcpy(fname, vcpathname);
	if (isshort(type.vow)) {
		sprintf(tmp, "%d", type.vow + 1);
	} else {
		sprintf(tmp, "%d", type.vow);
	}
	strcat(fname, tmp);
	strcat(fname, ".");
	sprintf(tmp, "%d", type.cons2);
	strcat(fname, tmp);
	sigsize2 = getfile(fname, signal2);

	/* Get start for vc and end for cv using the diphst and diphend parameters
	   respectively. For short vowels cut down size by a factor of 2 */
	if (isshort(type.vow)) {
		vow = type.vow + 1;
		start2 =
		    vc[type.vow + 1][type.cons2].vtoc -
		    ((vc[type.vow + 1][type.cons2].vtoc -
		      vc[type.vow + 1][type.cons2].diphst) / 2.0);
		end1 =
		    cv[type.cons1][type.vow + 1].ctov +
		    ((cv[type.cons1][type.vow + 1].diphend -
		      cv[type.cons1][type.vow + 1].ctov) / 2.0);
	} else {
		vow = type.vow;
		start2 = vc[type.vow][type.cons2].diphst;
		end1 = cv[type.cons1][type.vow].diphend;
	};

	/*Get the pitch marks nearest to the start for vc and end for cv
	   identified above */
	strcpy(fname, cvpathname);
	sprintf(tmp, "%d", type.cons1);
	strcat(fname, tmp);
	strcat(fname, ".");
	if (isshort(type.vow)) {
		sprintf(tmp, "%d", type.vow + 1);
	} else {
		sprintf(tmp, "%d", type.vow);
	}
	strcat(fname, tmp);
	leftmark = getnearestmark(fname, end1);

	strcpy(fname, vcpathname);
	if (isshort(type.vow)) {
		sprintf(tmp, "%d", type.vow + 1);
	} else {
		sprintf(tmp, "%d", type.vow);
	}
	strcat(fname, tmp);
	strcat(fname, ".");
	sprintf(tmp, "%d", type.cons2);
	strcat(fname, tmp);
	rightmark = getnearestmark(fname, start2);

	/* Determine start for cv and end for vc; this depends on whether 
	   they have to be played as diphones or not.          
	 */
	strcpy(fname, vcpathname);
	if (isshort(type.vow)) {
		sprintf(tmp, "%d", type.vow + 1);
	} else {
		sprintf(tmp, "%d", type.vow);
	}
	strcat(fname, tmp);
	strcat(fname, ".");
	sprintf(tmp, "%d", type.cons2);
	strcat(fname, tmp);

	/* Rightcut 1 or 3 means treat as diphone, so cut in the middle of the 
	   consonant, Rightcut 2 means the next consonant will be the same as 
	   this one, so special action is required.  */
	if (type.rightcut == 1 || type.rightcut == 3) {
		end2 = getnearestmark(fname, vc[vow][type.cons2].diphend);
	} else if (type.rightcut == 2 && type.cons2 != 19) {
		end2 =
		    getnearestmark(fname,
				   vc[vow][type.cons2].diphend +
				   ((vc[vow][type.cons2].end -
				     vc[vow][type.cons2].diphend) * 3 / 4));
	} else {
		end2 = vc[vow][type.cons2].end;
	};

	strcpy(fname, cvpathname);
	sprintf(tmp, "%d", type.cons1);
	strcat(fname, tmp);
	strcat(fname, ".");
	if (isshort(type.vow)) {
		sprintf(tmp, "%d", type.vow + 1);
	} else {
		sprintf(tmp, "%d", type.vow);
	}
	strcat(fname, tmp);
	if (type.leftcut == 1 || (type.leftcut == 3 && !delaycons(type.cons1))) {
		start1 = getnearestmark(fname, cv[type.cons1][vow].diphst);
	} else
		/*if (type.leftcut==2 && !delaycons(type.cons1)) {start1= getnearestmark(fname, cv[type.cons1][vow].diphst-((cv[type.cons1][vow].diphst-cv[type.cons1][vow].start)*3/4));}
		   else */  {
		start1 = cv[type.cons1][vow].start;
		};

	if (type.rightcut == 3) {
		strcpy(fname, vcpathname);
		if (isshort(type.vow)) {
			sprintf(tmp, "%d", type.vow + 1);
		} else {
			sprintf(tmp, "%d", type.vow);
		}
		strcat(fname, tmp);
		strcat(fname, ".");
		sprintf(tmp, "%d", type.cons2);
		strcat(fname, tmp);
		repeatmark =
		    getnearestmark(fname, vc[vow][type.cons2].diphend - 200);
	}

	/* Add a delay as in earlier cases if necessary */
	if (delaycons(type.cons1) && (type.leftcut == 0 || type.leftcut == 2)
	    && (prevdelaycons(type.prevcons))) {
		gap(rate * delaygap);
	};

	/*Play out the half as before and then output the cv followed by vc */
	if (type.halfsigend != -1) {
		output(type.halfsig, type.halfsigst, type.halfsigend, -1);
	};
	output(signal1, start1, leftmark, leftmark - start1);
	output(signal2, rightmark, end2, end2 - rightmark);
	if (type.rightcut == 3) {
		if (type.cons2 == 18) {
			j = 6;
		} else {
			j = 8;
		};
		for (i = 0; i <= j; ++i) {
			output(signal2, repeatmark, end2, end2 - repeatmark);
		}

	};

}

/*-----------------------------------------------------------------------
Processes hcv sounds. Just constructs the half part in signal and then 
calls cv setting leftcut=1.
 ------------------------------------------------------------------------*/
void hcvo(struct sndtype type)
{
	short signal[maxsignalsize];
	char fname[100];
	int sigsize;
	char tmp[3];
	int start, end;

	int sndindex1;
	strcpy(fname, halfspathname);
	sprintf(tmp, "%d", type.half);
	strcat(fname, tmp);
	strcat(fname, ".");
	sprintf(tmp, "%d", type.cons1);
	strcat(fname, tmp);
	sigsize = getfile(fname, signal);

	sndindex1 = findhalfsindex(type.half, type.cons1);
	start = halfs[sndindex1].start;
	end = halfs[sndindex1].end;

	strcpy(fname, halfspathname);
	sprintf(tmp, "%d", type.half);
	strcat(fname, tmp);
	strcat(fname, ".");
	sprintf(tmp, "%d", type.cons1);
	strcat(fname, tmp);
	end = getnearestmark(fname, end);

	if (delaycons(type.half) && (type.leftcut == 0)
	    && (prevdelaycons(type.prevcons))) {
		gap(rate * 200);
	};

	type.halfsig = signal;
	type.halfsigst = start;
	type.halfsigend = end;

	type.leftcut = 1;
	cvo(type);
}

/*----------------------------------------------------------------------
Processes hcvc sounds. Just constructs the half part in signal and then 
calls cvc setting leftcut=1.
------------------------------------------------------------------------*/
void hcvco(struct sndtype type)
{
	short signal[maxsignalsize];
	char fname[100];
	int sigsize;
	char tmp[3];
	int sndindex1;
	int start, end;

	strcpy(fname, halfspathname);
	sprintf(tmp, "%d", type.half);
	strcat(fname, tmp);
	strcat(fname, ".");
	sprintf(tmp, "%d", type.cons1);
	strcat(fname, tmp);
	sigsize = getfile(fname, signal);

	sndindex1 = findhalfsindex(type.half, type.cons1);
	start = halfs[sndindex1].start;
	end = halfs[sndindex1].end;

	strcpy(fname, halfspathname);
	sprintf(tmp, "%d", type.half);
	strcat(fname, tmp);
	strcat(fname, ".");
	sprintf(tmp, "%d", type.cons1);
	strcat(fname, tmp);
	end = getnearestmark(fname, end);

	if (delaycons(type.half) && (type.leftcut == 0)
	    && (prevdelaycons(type.prevcons))) {
		gap(rate * 200);
	};

	type.leftcut = 1;
	type.halfsig = signal;
	type.halfsigst = start;
	type.halfsigend = end;
	cvco(type);
}

/*----------------------------------------------------------------------
Processes v sounds. Just plays out the whole signal.
-------------------------------------------------------------------------*/
void v(struct sndtype type)
{
	short signal[15000];
	char fname[100];
	int sigsize = 0;
	char tmp[3];
	strcpy(fname, vpathname);
	sprintf(tmp, "%d", type.vow);
	strcat(fname, tmp);
	sigsize = getfile(fname, signal);

	output(signal, 0, -1, sigsize);

}

/*-----------------------------------------------------------------------
Synthesises from a phonetic string s 
--------------------------------------------------------------------------*/
void synthesize(char *s)
{
	int i, j;

	struct sndtype type;
	struct sndtype prevtype;
	struct sndtype nexttype;
	prevtype.vow=0;
	prevtype.cons2=0;
	/*split s into tokens, and analyse each token */
	if ((j = split(s)) > 0) {

		for (i = 0; i < j; ++i) {
			/*determine token type and call the appropriate procedure
			   with the right parameters */

			if ((type = gettype(splitarr[i])).ok == 0) {
				closedev();
				return;
			};

			if (type.cons1 != 0 && type.cons2 != 0 && type.vow != 0
			    && type.half != 0) {
				hcvco(type);
				if (i != j - 1) {
					nexttype = gettype(splitarr[i + 1]);
					if (nexttype.cons1 != 0) {
						if (nexttype.cons1 ==
						    type.cons2) {
							type.rightcut = 3;
						} else {
							type.rightcut = 2;
						};

					}
				}
			} else if (type.cons1 != 0 && type.vow != 0
				   && type.half != 0) {
				if (i != j - 1) {
					nexttype = gettype(splitarr[i + 1]);
					if (nexttype.half != 0) {
						type.cons2 = nexttype.half;
						type.rightcut = 1;
						hcvco(type);
						type.cons2 = 0;
					} else if (nexttype.cons1 != 0) {
						type.cons2 = nexttype.cons1;
						type.rightcut = 1;
						hcvco(type);
						type.cons2 = 0;
					} else {
						hcvo(type);
					};
				} else {
					hcvo(type);
				};
			} else if (type.cons1 != 0 && type.vow != 0
				   && type.cons2 != 0) {
				if (i > 0 && prevtype.vow != 0
				    && (prevtype.cons2 == 0
					|| prevtype.cons2 == type.cons1)) {
					type.leftcut = 1;
				};
				if (i > 0 && prevtype.cons2 != 0) {
					if (prevtype.cons2 == type.cons1) {
						type.leftcut = 3;
					} else {
						type.leftcut = 2;
					};
				};
				if (i > 0) {
					type.prevcons = prevtype.cons2;
				};
				if (i != j - 1) {
					nexttype = gettype(splitarr[i + 1]);
					if (nexttype.cons1 != 0) {
						if (nexttype.cons1 ==
						    type.cons2) {
							type.rightcut = 3;
						} else {
							type.rightcut = 2;
						};
					}
				}
				cvco(type);
			} else if (type.cons1 != 0 && type.vow != 0) {
				if (i > 0 && prevtype.vow != 0
				    && (prevtype.cons2 == 0)) {
					type.leftcut = 1;
				};
				if (i > 0 && prevtype.cons2 != 0) {
					if (prevtype.cons2 == type.cons1) {
						type.leftcut = 3;
					} else {
						type.leftcut = 2;
					};
				};
				if (i > 0) {
					type.prevcons = prevtype.cons2;
				};
				if (i != j - 1) {
					nexttype = gettype(splitarr[i + 1]);
					if (nexttype.half != 0) {
						type.cons2 = nexttype.half;
						type.rightcut = 1;
						cvco(type);
						type.cons2 = 0;
					} else if (nexttype.cons1 != 0) {
						type.cons2 = nexttype.cons1;
						type.rightcut = 1;
						cvco(type);
						type.cons2 = 0;
					} else {
						cvo(type);
					};
				} else {
					cvo(type);
				};
			} else if (type.cons2 != 0 && type.vow != 0) {
				if (i != j - 1) {
					nexttype = gettype(splitarr[i + 1]);
					if (nexttype.cons1 != 0) {
						if (nexttype.cons1 ==
						    type.cons2) {
							type.rightcut = 3;
						} else {
							type.rightcut = 2;
						};
					}
				}
				vco(type);
			} else if (type.cons2 != 0) {
				co(type);
			} else if (type.vow != 0) {
				if (i != j - 1) {
					nexttype = gettype(splitarr[i + 1]);
					if (nexttype.half != 0) {
						type.cons2 = nexttype.half;
						type.rightcut = 1;
						vco(type);
						type.cons2 = 0;
					} else if (nexttype.cons1 != 0) {
						type.cons2 = nexttype.cons1;
						type.rightcut = 1;
						vco(type);
						type.cons2 = 0;
					} else {
						v(type);
					};
				} else {
					v(type);
				};
			} else if (type.gap > 0) {
				gap(type.gap);
				if (i != j - 1) {
					nexttype = gettype(splitarr[i + 1]);
					if (delaycons(nexttype.cons1)) {
						gap(rate * 400);
					};
				}
			};
			prevtype = type;
		}

	}
	

}

void
process_sound()
{
    int callback_ret = 0;
#ifdef  WITH_SOUNDTOUCH 
    process_pitch_tempo(options, get_tempfile_name(1), output_file);
#endif
   
    if (!silent && options->audio_callback_fn == NULL) {
#ifdef  WITH_SOUNDTOUCH
        alsa_play(output_file);
#else
        alsa_play(get_tempfile_name(1));
#endif
        closedev();
    }
    if (options->synth_callback_fn != NULL) {
        callback_ret = (options->synth_callback_fn) (text_position);
        if (callback_ret == 1) {
            dhvani_debug("Forced stop by synth callback");
            return;
        }
    }
    
	remove(get_tempfile_name(1));
	
    
}
/*
 * Based on the language code disptch the strings to corresponding
 * language modules
 */

void
dispatch_to_phonetic_synthesizer(unsigned short *word, int length,
				 dhvani_Languages language_code)
{

	/*use language_code to determine the phonetic processer */
	switch (language_code) {
	case MALAYALAM:
		synthesize(generate_phonetic_script_ml(word, length));
		break;
	case HINDI:
		synthesize(generate_phonetic_script_hi(word, length));
		break;
	case KANNADA:
		synthesize(generate_phonetic_script_kn(word, length));
		break;
	case GUJARATI:
		synthesize(generate_phonetic_script_gu(word, length));
		break;
	case ORIYA:
		synthesize(generate_phonetic_script_or(word, length));
		break;
	case TELUGU:
		synthesize(generate_phonetic_script_te(word, length));
		break;
	case PANJABI:
		synthesize(generate_phonetic_script_pa(word, length));
		break;
	case BENGALI:
		synthesize(generate_phonetic_script_bn(word, length));
		break;
	case TAMIL:
		synthesize(generate_phonetic_script_ta(word, length));
		break;
	case MARATHI:
		synthesize(generate_phonetic_script_mr(word, length));
		break;
	case PASHTO:
		synthesize(generate_phonetic_script_ps(word, length));
		break;
	default:
		dhvani_debug("This language is not supported.\n");
	}
}

/*read the given file. If there is no user language preference , language will be detected*/
void speak_file(FILE * fd, int usr_language)
{
	unsigned short word[150];	/*150 is enough? 150= 50 unicode letters */
	int i;			/* 'verbatim' flag  */
	struct code letter;
	int detected_lang = 0;
#ifdef WITH_OGGVORBIS
	dhvani_ogg_parameters ogg_parameters;
#endif
	int prev_language = 0;
	i = 0;
	text_position = 0;
	while ((letter = utf8_to_utf16_file(fd)).type != -1) {	/* while vaild input */
		text_position++;
		if (letter.type == 0) {	/* if ASCII  */

			/*    Work to be done !   */
			//0x0D to 0x07 seperators
			if ((letter.beta <= 0x0D && letter.beta >= 0x07) || (letter.beta == 0x20)	/*space */
			    ||(letter.beta == 0x2D)	/*hiphen */
			    ) {
				if (i > 0) {
					word[i++] = '\0';

					if (usr_language < 1
					    || usr_language >
					    DHVANI_NO_OF_LANGUAGES_SUPPORTED) {
						dhvani_debug
						    ("No language switch. Detecting...");
						detected_lang =
						    detect_language(word, i);
						if (detected_lang > -1) {
							dhvani_debug
							    ("%d [OK]\n",
							     detected_lang);
						}
						//      else continue with the current language
						else {
							if (prev_language > 0) {
								detected_lang =
								    prev_language;
							} else {
								continue;	/*skip */
							}
						}
					} else {
						detected_lang = usr_language;
					}
					/*set the preve language */
					prev_language = detected_lang;
					dispatch_to_phonetic_synthesizer(word,
									 i - 1,
									 detected_lang);

					synthesize(DHVANI_SPACE_DELAY);	//space delay
					if (letter.beta == '\n'
					    || letter.beta == 0x0D){
						synthesize(DHVANI_LINE_DELAY);	//Line delay
						process_sound();
					}
					i = 0;
				}

			} else if ((letter.beta >= 0x30 && letter.beta <= 0x39)	//asci numbers
				   || (letter.beta == 0x2E)	//'.' - point- dashamsham
				   || (letter.beta >= 0x41 && letter.beta <= 0x7A)	//[A-Za-z]
				   || (letter.beta == 0x24)	//$
				   || (letter.beta == 0x25)	//%
				   || (letter.beta == 0x3D)	//=
				   || (letter.beta == 0x40)	//@
			    ) {
				word[i++] = letter.beta;
			}

		}		//end of ASCII
		else if (letter.type == 1) {
			word[i++] = letter.alpha;	/* collect next 'letter' */
		}

	}
	/*if anything remains*/
	process_sound();
	/*check whether we need to encode the output file to ogg format */
	if (options->speech_to_file == 1 && options->audio_callback_fn == NULL) {
		if (options->output_file_format == DHVANI_OGG_FORMAT) {
#ifdef WITH_OGGVORBIS
			ogg_parameters.channels = 2;
			ogg_parameters.sampling_rate = 16000;
			ogg_parameters.quality = 0.4;
			oggenc(output_file, options->output_file_name,
			       &ogg_parameters);

#endif
		}
	}
}

/*read the given text. If there is no user language preference , language will be detected*/

void speak_text(char *text, int usr_language)
{
	unsigned short word[100];
#ifdef WITH_OGGVORBIS
	dhvani_ogg_parameters ogg_parameters;
#endif
	int i;
	int detected_lang = 0;
	int prev_language = 0;
	struct code letter;
	i = 0;
	int length = strlen(text);
	int end_of_word = 0;
	if (length == 0) {
		return;		/* don't waste time */
	}
	text_position = 0;

	while (text_position <= length) {
		/* while vaild input */
		letter = utf8_to_utf16_text(text, &text_position);
		if (letter.type == 0) {	/* if ASCII */
		dhvani_debug
					    ("asci %d", *(text+text_position));
			//0x0D to 0x07 seperators
			if ((letter.beta <= 0x0D && letter.beta >= 0x07) || (letter.beta == 0x20)	/*space */
			    ||(letter.beta == 0x2D)	/*hiphen */
			    ) {
				end_of_word = 1;
dhvani_debug
					    ("endof word");
			} else if ((letter.beta >= 0x30 && letter.beta <= 0x39)	//asci numbers
				   || (letter.beta == 0x2E)	//'.' - point- dashamsham
				   || (letter.beta >= 0x41 && letter.beta <= 0x7A)	//[A-Za-z]
				   || (letter.beta == 0x24)	//$
				   || (letter.beta == 0x25)	//%
				   || (letter.beta == 0x3D)	//=
				   || (letter.beta == 0x40)	//@
			    ) {
				word[i++] = letter.beta;
				dhvani_debug
					    ("got something");
			}

		}		//end of ASCII
		else if (letter.type == 1) {
			word[i++] = letter.alpha;	/* collect next 'letter' */

		}
		if (end_of_word || text_position >= length) {
			dhvani_debug
					    ("1860");
			if (i > 0) {
				word[i++] = '\0';
				if (usr_language < 1
				    || usr_language >
				    DHVANI_NO_OF_LANGUAGES_SUPPORTED) {
					dhvani_debug
					    ("No language switch. Detecting...");
					detected_lang =
					    detect_language(word, i);
					if (detected_lang > -1) {
						dhvani_debug("%d [OK]",
							     detected_lang);
					}	//      else continue with the current language
					else {
						if (prev_language > 0) {
							detected_lang =
							    prev_language;
						} else {
							continue;
						}
					}
				} else {
					dhvani_debug
					    ("Using the user language option %d",
					     usr_language);
				}
				prev_language = detected_lang;
				dispatch_to_phonetic_synthesizer(word, i - 1,
								 detected_lang);
				end_of_word = 0;
				i = 0;
			}
		}
	}
	process_sound();
	/*check whether we need to encode the output file to ogg format */
	if (options->speech_to_file == 1 && options->audio_callback_fn == NULL) {
		if (options->output_file_format == DHVANI_OGG_FORMAT) {
#ifdef WITH_OGGVORBIS
			ogg_parameters.channels = 2;
			ogg_parameters.sampling_rate = 16000;
			ogg_parameters.quality = 0.4;
			oggenc(output_file, options->output_file_name,
			       &ogg_parameters);

#endif
		}
	}
}

/*Initialize the database path and alsa player*/
int start_synthesizer()
{
	dhvani_debug("Starting the synthesizer...");
	initialize_pathnames();
	readhoffsets();
	readvcoffsets();
	readcvoffsets();
	readvoffsets();
	 /*
       Initialize the alsa player only of we are working on direct play mode
     */
    if (!silent) {
        handle = alsa_init();
        if (handle == NULL) {
            dhvani_debug("The alsa handle retured is null.");
            return DHVANI_INTERNAL_ERROR;
        }
    }
	return DHVANI_OK;

}

/*Initialize the database path and alsa player*/
int stop_synthesizer()
{
	dhvani_debug("Stopping the synthesizer...");
    if(remove(get_tempfile_name(2))==-1)
		perror("Error in deleting temporary file");
	return DHVANI_OK;
}

/*
 * read out a file given by file pointer. All other options are in the dhvani_options
 * Structure
 */
int file_to_speech(FILE * fd, dhvani_options * dhvani_opts)
{
	options = dhvani_opts;

	if (options->speech_to_file == 1) {
		silent = 1;
	}
	if (options->output_file_format == DHVANI_OGG_FORMAT) {
		output_file = get_tempfile_name(2);
	} else {
		output_file = options->output_file_name;
	}
	speak_file(fd, options->language);
	return DHVANI_OK;
}

int text_to_speech(char *string, dhvani_options * dhvani_opts)
{
	options = dhvani_opts;

	if (options->speech_to_file == 1) {
		silent = 1;
	}
	if (options->output_file_format == DHVANI_OGG_FORMAT) {
		output_file = get_tempfile_name(2);
	} else {
		output_file = options->output_file_name;
	}
	dhvani_debug("saying: %s", string);
	speak_text(string, options->language);
	return 0;
}

/*
Read a phonetic file in dhvani phonetic file format
 **********EXPERIMENTAL***************
 */
int phoneticfile_to_speech(FILE * fd, dhvani_options * dhvani_opts)
{
	char ch;
	char instr[10000];
	int instrindex = 0;
	options = dhvani_opts;

	if (options->speech_to_file == 1) {
		silent = 1;
	}
	if (options->output_file_format == DHVANI_OGG_FORMAT) {
		output_file = get_tempfile_name(2);
	} else {
		output_file = options->output_file_name;
	}
	while ((fread(&ch, sizeof(unsigned char), 1, fd))) {

		instr[instrindex++] = ch;
		if (ch == '\n' || instrindex == 1000) {

			/*play this line     */
			instr[instrindex - 1] = '\0';
			synthesize(instr);
			instrindex = 0;
		}

	}

	return 0;
}

/*
Read a phonetic string in dhvani phonetic file format
 **********EXPERIMENTAL***************
 */
int phonetic_to_speech(char *instr, dhvani_options * dhvani_opts)
{
	options = dhvani_opts;

	if (options->speech_to_file == 1) {
		silent = 1;
	}
	if (options->output_file_format == DHVANI_OGG_FORMAT) {
		output_file = get_tempfile_name(2);
	} else {
		output_file = options->output_file_name;
	}
	dhvani_debug("saying: %s", instr);
	synthesize(instr);
	return 0;
}
