#include "context.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "type.h"

void run_file(char *fp) {
  FILE *fptr = fopen(fp, "r");

  fseek(fptr, 0, SEEK_END);
  long file_size = ftell(fptr);

  printf("File size: %ld\n", file_size);

  char *program_txt = malloc(file_size + 1);
  fseek(fptr, 0, SEEK_SET);
  fread(program_txt, file_size, 1, fptr);
  program_txt[file_size] = 0;

  struct context *ctx = ctx_new();
  struct parser parser = {.cursor = program_txt, .text = program_txt};
  obj compiled = compile(&parser);

  free(program_txt);

  eval(ctx, compiled);
  // obj_release(compiled);

  printf("\n");
}

void repl(void) {
  char line[1024];
  printf("REPL mode on (type \\q to exit)\n");
  struct context *ctx = ctx_new();
  while (1) {
    printf("forth> ");
    memset(line, 0, sizeof(line));
    if (!fgets(line, sizeof(line), stdin))
      break;

    if (strncmp(line, "\\q", 2) == 0)
      break;

    struct parser p = {.text = line, .cursor = line};
    obj tokenized = compile(&p);
    eval(ctx, tokenized);

    for (size_t i = 0; i < ctx->stack->count; i++) {
      obj o = ctx->stack->data[i];
      if (i > 0) {
        printf("|");
      }
      obj_print(o);
    }
    printf("\n");
    // obj_release(tokenized);
  }
}

int main(int argc, char **argv) {
  fprintf(stderr, "%d count\n", argc);

  switch (argc) {
  case 2:
    printf("Running from file:\n");
    run_file(argv[1]);
    break;
  case 1:
    repl();
    break;
  default:
    fprintf(stderr, "Error in parametrs\n");
    fprintf(stderr, "Usage %s <filename>", argv[0]);
    return -1;
  }

  return 0;
}
