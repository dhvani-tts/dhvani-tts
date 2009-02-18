/*
 
UTF8Decoder.c
This program converts a utf-8 encoded string to utf-16 hexadecimal code sequence

UTF-8 is a variable-width encoding of Unicode: it uses one byte for ASCII characters
(and with exactly the same codes), two bytes for most Western characters (both bytes
have their top bit set), three bytes for most Chinese characters, and four bytes for
new rarer characters. UTF-16 is a fixed width encoding of two bytes

a UTF-8 decoder must not accept UTF-8 sequences that are longer than necessary to
encode a character. For example, the character U+000A (line feed) must be accepted from
a UTF-8 stream only in the form 0x0A, but not in any of the following five possible overlong forms:

  0xC0 0x8A
  0xE0 0x80 0x8A
  0xF0 0x80 0x80 0x8A
  0xF8 0x80 0x80 0x80 0x8A
  0xFC 0x80 0x80 0x80 0x80 0x8A

Ref: UTF-8 and Unicode FAQ for Unix/Linux http://www.cl.cam.ac.uk/~mgk25/unicode.html

Author: Santhosh Thottingal <santhosh.thottingal at gmail.com>
License: This program is licensed under GPLv2 or later version(at your choice)
 */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "UTF8Decoder.h"

struct code utf8_to_utf16_text( unsigned char *text, int *ptr)
{
	unsigned short c;	/*utf-16 character */
	int trailing = 0;
	struct code retval;
	retval.alpha=0;
	if (text[*ptr] < 0x80) {	/*ascii character till 128 */
		trailing = 0;
		c = text[(*ptr)++];
		retval.type = 0;
		retval.beta = c;
	} else if (text[*ptr] >> 7) {
		if (text[*ptr] < 0xE0) {
			c = text[*ptr] & 0x1F;
			trailing = 1;
		} else if (text[*ptr] < 0xF8) {
			c = text[*ptr] & 0x07;
			trailing = 3;
		}

		for (; trailing; trailing--) {
			if ((((text[++*ptr]) & 0xC0) != 0x80))
				break;
			c <<= 6;
			c |= text[*ptr] & 0x3F;
		}
		retval.type = 1;
		retval.alpha = c;
	}
	return retval;

}

struct code utf8_to_utf16_file(FILE * fileptr)
{

	unsigned short c = 0;	/*utf-16 character */
	int trailing = 0;
	unsigned char ch = 0;
	struct code retval;
	retval.alpha=retval.beta=0;
	if (fread(&ch, sizeof(unsigned char), 1, fileptr) <= 0) {
		retval.type = -1;
		return retval;
	}

	if (ch < 0x80) {	/*ascii character till 128 */
		trailing = 0;
		c = ch;
		retval.type = 0;
		retval.beta = c;
	} else if (ch >> 7) {
		if (ch < 0xE0) {
			c = ch & 0x1F;
			trailing = 1;
		} else if (ch < 0xF8) {
			c = ch & 0x07;
			trailing = 3;
		}

		for (; trailing; trailing--) {

			fread(&ch, sizeof(unsigned char), 1, fileptr);

			if ((ch & 0xC0) != 0x80) {
				fseek(fileptr, -1, SEEK_CUR);
				break;
			}
			c <<= 6;
			c |= ch & 0x3F;

		}
		retval.type = 1;
		retval.alpha = c;
	}

	return retval;

}
