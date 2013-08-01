#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "dhvani_utils.h"
#include "debug.h"

FILE* dhvani_fopen(const char *filename, const char *mode)
{
  char *errorstring = NULL;
  FILE *file = NULL;

  if((file = fopen(filename, mode)) == NULL)
    {
      DHVANI_ERROR("%s - %s", filename, strerror(errno));
      exit(1);
    }
  return file;
}
