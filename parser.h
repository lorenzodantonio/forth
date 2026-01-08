#pragma once

#include "type.h"

struct parser {
  char *text;
  char *cursor;
};

obj parse_int(struct parser *parser);
obj parse_string(struct parser *parser);
obj parse_flash_msg(struct parser *parser);
obj parse_symbol(struct parser *parser);
obj parse_word(struct parser *parser);

int is_symbol_char(int c);

obj compile(struct parser *parser); // parse program
