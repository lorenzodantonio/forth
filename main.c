#include "context.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "type.h"

/* compilation */

struct obj *compile(struct parser *parser) {
  struct obj *parsed = obj_list_new();

  while (parser->cursor[0] != 0) {
    while (isspace(parser->cursor[0])) {
      parser->cursor++;
    }

    struct obj *object = NULL;
    if (isdigit(parser->cursor[0]) ||
        (parser->cursor[0] == '-' && isdigit(parser->cursor[1]))) {
      object = parse_int(parser);
    } else if (memcmp(parser->cursor, "s\" ", 3) == 0) {
      object = parse_string(parser);
    } else if (parser->cursor[0] && memcmp(parser->cursor, ".( ", 3) == 0) {
      object = parse_flash_msg(parser);
    } else if (parser->cursor[0] && is_symbol_char(parser->cursor[0])) {
      object = parse_symbol(parser);
      // } else if (p.cursor[0] && memcmp(p.cursor, ": ", 2) == 0) {
      // object = parse_word();
    } else {
      parser->cursor++;
    }

    if (object != NULL) {
      obj_list_push(parsed, object);
      obj_retain(object);
    }
  }

  return parsed;
}

void eval(struct context *ctx, struct obj *obj) {
  switch (obj->type) {
  case OBJ_TYPE_LIST:
    for (size_t i = 0; i < obj->list.len; i++) {
      eval(ctx, obj->list.data[i]);
    }
    break;
  case OBJ_TYPE_FLASH_MSG:
    obj_print(obj);
    break;
  case OBJ_TYPE_SYMBOL:
    symbol_exec(ctx, obj);
    break;
  default:
    ctx_stack_push(ctx, obj);
    obj_retain(obj);
  }
}

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
  struct obj *compiled = compile(&parser);

  free(program_txt);

  eval(ctx, compiled);
  obj_release(compiled);
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
    struct obj *tokenized = compile(&p);
    eval(ctx, tokenized);

    for (size_t i = 0; i < ctx->stack->list.len; i++) {
      struct obj *o = ctx->stack->list.data[i];
      if (i > 0) {
        printf("|");
      }
      obj_print(o);
    }
    printf("\n");

    obj_release(tokenized);
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
