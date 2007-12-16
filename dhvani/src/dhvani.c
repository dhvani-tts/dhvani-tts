/* dhvani.c
 *
 * Copyright (C) 2007-2008
 *  Santhosh Thottingal <santhosh00@gmail.com>, Swathanthra Malayalam Computing.
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

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include "dhvani.h"
#include "synthesizer.h"
#include "languages.h"
void
print_usage ()
{
  printf ("Usage: dhvani [options] File\n");
  printf ("[options] is any of the following \n");
  printf
    ("  -l, --lang <languagecode>\t Set the language of the file. If this option is not set, dhvani will detect the language automatically.\n");
  printf ("  -p, --phonetic\t Read Phonetic code.\n");
  printf ("  -t, --text\t Read a text.\n");
  printf ("  -d, --list\t Print the list of supported languages.\n");
  printf
    ("  -s, --speed <normal|fast>\t Speed of the speech.Default is normal\n");
  printf ("  -o, --output <file>\t Save the speech to a file.\n");
  printf ("  -v, --version\t Print the version.\n");
  printf ("  -h, --help\t Print this help.\n");
  printf ("File should be in UTF-8 format.\n");
  printf
    ("Report bugs/suggestions to Santhosh Thottingal<santhosh00@gmail.com>.\n");
}

int
main (int argc, char *argv[])
{

  int language = 0;
  FILE *fd = 0;
  int arg_itr = 0;
  int phonetic = 0;
  int text = 0;
  int speed = NORMAL;
  int character;
  char *outputfile = NULL;
  char instr[1000];
  int instrindex = 0;
  // printf ("%s %s\n", PROGRAM_NAME, VERSION);
  if (argc == 1)
    {				//No args
      print_usage ();
      exit (0);
    }
  else
    {
      if (argc == 2)
	{
	  if (strcmp (argv[1], "-d") == 0 || strcmp (argv[1], "--list") == 0)
	    {
	      printf
		("Malayalam\nHindi\nKannada\nGujarati\nOriya\nTelugu\nPanjabi\nBengali\n");
	      exit (0);
	    }

	  if (strcmp (argv[1], "-h") == 0 || strcmp (argv[1], "--help") == 0)
	    {
	      print_usage ();
	      exit (0);

	    }
	  if (strcmp (argv[1], "-v") == 0
	      || strcmp (argv[1], "--version") == 0)
	    {
	      printf ("%s", VERSION);
	      exit (0);

	    }
	  //No options, direct play
	  fd = fopen (argv[1], "r");
	}
      else
	{
	  for (arg_itr = 1; arg_itr < argc; arg_itr++)
	    {
	      //language option
	      if (strcmp (argv[arg_itr], "-l") == 0
		  || strcmp (argv[arg_itr], "--language") == 0)
		{
		  if (++arg_itr == argc)
		    {
		      print_usage ();
		      exit (0);
		    }
		  language = get_language_code (argv[arg_itr]);
		}
	      //phonetic code play
	      if (strcmp (argv[arg_itr], "-p") == 0
		  || strcmp (argv[arg_itr], "--phonetic") == 0)
		{
		  phonetic = 1;
		}
	      //phonetic code play
	      if (strcmp (argv[arg_itr], "-t") == 0
		  || strcmp (argv[arg_itr], "--text") == 0)
		{
		  if (++arg_itr == argc)
		    {
		      print_usage ();
		      exit (0);
		    }
		  text = arg_itr;
		}
	      if (strcmp (argv[arg_itr], "-o") == 0
		  || strcmp (argv[arg_itr], "--output") == 0)
		{
		  //printf("\nOutputfile: %s", argv[arg_itr+1]);
		  if (++arg_itr == argc)
		    {
		      print_usage ();
		      exit (0);
		    }
		  outputfile = argv[arg_itr];
		}
	      if (strcmp (argv[arg_itr], "-s") == 0
		  || strcmp (argv[arg_itr], "--speed") == 0)
		{
		  //printf("\nOutputfile: %s", argv[arg_itr+1]);
		  if (++arg_itr == argc)
		    {
		      print_usage ();
		      exit (0);
		    }
		  if (strcmp (argv[arg_itr], "fast") == 0)
		    speed = FAST;
		}
	    }
	  //printf("\nOpening: %s", argv[arg_itr]);
	  fd = fopen (argv[arg_itr - 1], "r");
	}
    }
  if (fd < 0)
    {
      printf ("couldnot open file\n");
      exit (0);
    }
  if (phonetic == 1)
    phonetic_to_speech (fd);
  else if (text == 0)
    file_to_speech (fd, language, speed, outputfile);
  else if (text > 0)
    text_to_speech (argv[text], language, speed, outputfile);

  if (fd > 0)
    {
      fclose (fd);
    }
  printf ("\nDone.\n");
  return 0;
}
