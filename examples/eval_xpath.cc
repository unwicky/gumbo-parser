#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "xpath_eval.h"

static void read_file(FILE* fp, char** output, int* length) {
  struct stat filestats;
  int fd = fileno(fp);
  fstat(fd, &filestats);
  *length = filestats.st_size;
  *output = (char *)malloc(*length + 1);
  int start = 0;
  int bytes_read;
  while ((bytes_read = fread(*output + start, 1, *length - start, fp))) {
    start += bytes_read;
  }
}

int main(int argc, char** argv) {
  if (argc != 3) {
      printf("evalxpath <html filename> <xpathexpression>\n");
      exit(EXIT_FAILURE);
  }
  const char* filename = argv[1];
  FILE* fp = fopen(filename, "r");
  if (!fp) {
    printf("File %s not found!\n", filename);
    exit(EXIT_FAILURE);
  }
  const char* xpathexpression = argv[2];

  char* input;
  int input_length;
  GumboParser parser;
  parser._options = &kGumboDefaultOptions;
  read_file(fp, &input, &input_length);
  GumboOutput* output = gumbo_parse_with_options(&kGumboDefaultOptions, input, input_length);
  GumboVector nodes = {0};
  gumbo_vector_init(&parser, DEFAULT_VECTOR_SIZE, &nodes);
  gumbo_eval_xpath_from_root(&parser, output->root, xpathexpression, &nodes);
  gumbo_vector_destroy(&parser, &nodes);
  gumbo_destroy_output(&kGumboDefaultOptions, output);
  free(input);
}