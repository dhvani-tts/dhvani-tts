/* dhvani.c
 *
 * Copyright (C) 2007-2009
 *  Santhosh Thottingal <santhosh.thottingal@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* This version of the command-line tts program uses the libdhvani shared  library */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include <getopt.h>
#include "dhvani_lib.h"
#include "debug.h"
#include "languages.h"
#include "dhvani_utils.h"

static const char *help_text =
    "Usage: dhvani [options] [file|text]\n"
    "[options] is any of the following \n"
    "  -l, --lang <languagecode>\t Set the language of the file. If this option is not set, dhvani will detect the language automatically.\n"
    "  -t, --text\t Say a text.\n"
    "  -d, --list\t Print the list of supported languages.\n"
    "  -s=n, --speed=n : Change sound tempo by n percents  (n=-95..+5000 %).\n"
    "  -p=n, --pitch=n : Change sound pitch by n semitones (n=-60..+60 semitones).\n"
    "  -o, --output <file>\t Save the speech to a file.\n"
    "  -f, --format <ogg|wav>\t Format of the generated speech file while writing to a file. Default is ogg.\n"
    "  -v, --version\t Print the version.\n"
    "  -i, --stdin\t Read the content from stdin. Interactive mode.\n"
    "  -P, --phonetic\t Read the string in dhvani phonetic format.\n"
    "  -h, --help\t Print this help.\n"
    "File should be in UTF-8 format.\n"
    "Report bugs/suggestions to Santhosh Thottingal <santhosh.thottingal@gmail.com>.\n";

/*
Print the Dhvani command line help message.
 */
void print_usage()
{
	DHVANI_INFO("Dhvani Indian Language Text To Speech System: %s\n%s", help_text);
}

int main(int argc, char *argv[])
{
	static struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"lang", optional_argument, 0, 'l'},
		{"speed", optional_argument, 0, 's'},
		{"output", optional_argument, 0, 'o'},
		{"format", optional_argument, 0, 'f'},
		{"phonetic", optional_argument, 0, 'P'},
		{"pitch", optional_argument, 0, 'p'},
		{"text", optional_argument, 0, 't'},
		{"list", no_argument, 0, 'd'},
		{"debug", no_argument, 0, 'X'},
		{"stdin", no_argument, 0, 'i'},
		{0, 0, 0, 0}
	};
	FILE *fd = 0;
	dhvani_options *options;
	int text_option = 0;
	char *text = NULL;
	int option_index = 0;
	int flag_stdin = 0;
	int argument_character;
	int max = 1000;
	char *stdin_text = (char *)malloc(max);
	options = dhvani_init();
	if (argc == 1) {
		print_usage();
		exit(0);
	}
	while (1) {
		argument_character =
		    getopt_long(argc, argv, "idhvp:l:o:s:f:t:X", long_options,
				&option_index);
		/* Detect the end of the options. */
		if (argument_character == -1)
			break;
		switch (argument_character) {
		case 'd':
			printf("%s\n", dhvani_ListLanguage());
			exit(0);
			break;

		case 'h':
			print_usage();
			exit(0);
			break;

		case 'v':
		        DHVANI_INFO("version: ", dhvani_version());
			exit(0);
			break;

		case 'P':
			fd = dhvani_fopen(optarg, "r");
			options->isPhonetic = 1;
			break;

		case 'p':
#ifdef  WITH_SOUNDTOUCH
			options->pitch = atoi(optarg);
#else
			DHVANI_INFO("Dhvani is compiled without soundtouch support. Default pitch will be used.\n");
#endif

			break;

		case 't':
			text = optarg;
			options->isPhonetic = 0;
			text_option = 1;
			break;

		case 'o':
			options->speech_to_file = 1;
			/*Default output format is ogg */
			options->output_file_name = optarg;
			break;

		case 'l':
			options->language = get_language_code(optarg);
			break;

		case 's':
#ifdef  WITH_SOUNDTOUCH
			options->tempo = atoi(optarg);
#else
			DHVANI_INFO("Dhvani is compiled without soundtouch support. Default speed will be used.\n");
#endif

			break;
		case 'f':
			if (strcmp(optarg, "WAV") == 0
			    || strcmp(optarg, "wav") == 0
			    || strcmp(optarg, "WAVE") == 0
			    || strcmp(optarg, "wave") == 0) {
				options->output_file_format = DHVANI_WAV_FORMAT;
			}
			if (strcmp(optarg, "OGG") == 0
			    || strcmp(optarg, "ogg") == 0) {
#ifdef WITH_OGGVORBIS
				options->output_file_format = DHVANI_OGG_FORMAT;
#else
				DHVANI_INFO("Dhvani is compiled without ogg voribis support. Defaul sound format(wav) will be used.\n");
				options->output_file_format = DHVANI_WAV_FORMAT;
#endif

			}
			break;

		case 'i':
			flag_stdin = 1;
			break;

		default:
			exit(0);
		}
	}
	if ((!flag_stdin) && (options->isPhonetic == 0) && (!text_option)) {
		fd = dhvani_fopen(argv[argc - 1], "r");
		/* 
                 * if (fd == NULL) {
		 * 	DHVANI_ERROR("Could not open file %s\n",
		 * 		     argv[argc - 1]);
		 * 	exit(0);
		 * }
                 */
	}
	if (options->isPhonetic == 1) {
		/* Phonetic file as input */
		DHVANI_DEBUG("Phonetic mode \n");
		dhvani_speak_phonetic_file(fd, options);
	} else if (text_option == 0 && flag_stdin == 0) {
		DHVANI_DEBUG("Text file mode \n");
		/* Speak the given file */
		dhvani_speak_file(fd, options);
	} else if (!options->isPhonetic && text_option == 1) {
		/* Now option is o speak the given text */
		DHVANI_DEBUG("Text mode \n");
		dhvani_say(text, options);
	} else if (flag_stdin == 1) {
		/* line by line input on stdin. First initialize */
		DHVANI_DEBUG("Interactive mode \n");
		printf
		    ("dhvani interactive mode.Give the string to speak at the prompt");
		printf("\ndhvani>");
		fflush(stdout);
		while (fgets(stdin_text, max, stdin) != NULL) {
			/*say it */
			dhvani_say(stdin_text, options);
			/*display the prompt */
			printf("\ndhvani>");
		}
	}
	if (fd != NULL) {
		fclose(fd);
	}
	return 0;
}
