#pragma once

#include "type.h"

struct parser {
  char *text;
  char *cursor;
};

struct obj *parse_int(struct parser *parser);
struct obj *parse_string(struct parser *parser);
struct obj *parse_flash_msg(struct parser *parser);
struct obj *parse_symbol(struct parser *parser);
struct obj *parse_word(struct parser *parser);

int is_symbol_char(int c);

struct obj *compile(struct parser *parser); // parse program
