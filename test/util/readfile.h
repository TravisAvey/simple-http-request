#ifndef __READFILE_H__
#define __READFILE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *readFile(const char *filename) {
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  int length = ftell(file);

  fseek(file, 0, SEEK_SET);

  char *string = (char *)calloc(length + 1, sizeof(char) * (length + 1));
  if (string == NULL) {
    free(string);
    return NULL;
  }

  char c;
  int i = 0;
  while ((c = fgetc(file)) != EOF) {
    string[i] = c;
    i++;
  }

  string[i - 1] = '\0';

  fclose(file);

  return string;
}

#endif
