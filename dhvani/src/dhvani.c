/* dhvani.c
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

// This version of the command-line speak program uses the
// libdhvani.so.1  library

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include <getopt.h>
#include "dhvani_lib.h"
#include "languages.h"


static const char *help_text =
        "Usage: dhvani [options] File\n"
        "[options] is any of the following \n"
        "  -l, --lang <languagecode>\t Set the language of the file. If this option is not set, dhvani will detect the language automatically.\n"
        "  -p, --phonetic\t Read Phonetic code.\n"
        "  -t, --text\t Read a text.\n"
        "  -d, --list\t Print the list of supported languages.\n"
        "  -s, --speed <normal|fast>\t Speed of the speech.Default is normal\n"
        "  -o, --output <file>\t Save the speech to a file.\n"
        "  -v, --version\t Print the version.\n"
        "  -i, --stdin\t Read the content from stdin.\n"
        "  -h, --help\t Print this help.\n"
        "File should be in UTF-8 format.\n"
        "Report bugs/suggestions to Santhosh Thottingal<santhosh00@gmail.com>.\n";

/*
Print the Dhvani command line help message.
 */
void
print_usage() {
    printf("Dhvani Indian Language Text To Speech System: %s\n%s",
            dhvani_Info(), help_text);
}

void
strncpy0(char *dest, const char *source, int size) { //====================================================
    if (source != NULL) {
        dest = (char *) malloc(size);
        strncpy(dest, source, size);
        dest[size - 1] = 0;
    }
}

int
main(int argc, char *argv[]) {
    static struct option long_options[] = {
        /* These options set a flag. */
        /* These options don't set a flag.
           We distinguish them by their indices. */
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {"lang", optional_argument, 0, 'l'},
        {"speed", optional_argument, 0, 's'},
        {"output", optional_argument, 0, 'o'},
        {"phonetic", optional_argument, 0, 'p'},
        {"text", optional_argument, 0, 't'},
        {"list", no_argument, 0, 'd'},
        {"stdin", no_argument, 0, 'i'},
        {0, 0, 0, 0}
    };
    int language = 0;
    FILE *fd = 0;
    int arg_itr = 0;
    int phonetic = 0;
    int text_option = 0;
    char *text = NULL;
    int option_index = 0;
    int speed = NORMAL;
    int character;
    int flag_stdin = 0;
    char *outputfile = NULL;
    char instr[1000];
    int argument_character;
    int instrindex = 0;
    int max = 1000;
    char *stdin_text = (char *) malloc(max);
    if (argc == 1) {
        print_usage();
        exit(0);
    }
    while (1) {
        argument_character =
                getopt_long(argc, argv, "idhvpl:o:s:t:", long_options, &option_index);
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
                printf("%s\n", dhvani_Info());
                break;

            case 'p':
                fd = fopen(optarg, "r");
                break;

            case 't':
                //strncpy0(text,optarg,sizeof(text));
                text = optarg;
                text_option = 1;
                break;

            case 'o':
                outputfile = optarg;
                break;

            case 'l':
                language = get_language_code(optarg);
                break;

            case 's':
                if (strcmp(optarg, "fast") == 0) {
                    speed = FAST;
                }
                break;

            case 'i':
                flag_stdin = 1;
                break;

            default:
                exit(0);
        }
    }
    if (!flag_stdin & !text_option) {
        fd = fopen(argv[argc - 1], "r");
        if (fd <= 0) {
            printf("Could not open file %s\n", argv[argc - 1]);
            exit(0);
        }
    }
    if (phonetic == 1) {
        dhvani_speak_phonetic_file(fd);
    } else if (text_option == 0 && flag_stdin == 0) {
        dhvani_speak_file(fd, language, speed, outputfile);
    } else if (text_option == 1) {
        dhvani_say(text, language, speed, outputfile);
    } else if (flag_stdin == 1) {
        // line by line input on stdin
        while (fgets(stdin_text, max, stdin) != NULL) {
            stdin_text[max - 1] = 0;
            dhvani_say(stdin_text, language, speed, outputfile);
        }
    }
    if (fd > 0) {
        fclose(fd);
    }
    return 0;
}
