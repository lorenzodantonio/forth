#include "parser.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

struct obj *parse_int(struct parser *parser) {
  char buffer[128];
  char *start = parser->cursor;
  char *end;

  if (parser->cursor[0] == '-') {
    parser->cursor++;
  }
  while (isdigit(parser->cursor[0])) {
    parser->cursor++;
  }
  end = parser->cursor;
  int numlen = end - start;
  if (numlen >= 128) {
    return NULL;
  }

  memcpy(buffer, start, numlen);
  buffer[numlen] = '\0';
  int i = atoi(buffer);
  struct obj *obj = obj_int_new(i);
  obj->num = i;

  return obj;
}

int is_symbol_char(int c) {
  char symchars[] = "+-*/%.";
  if (isalpha(c)) {
    return 1;
  } else if (strchr(symchars, c) != NULL) {
    return 1;
  } else {
    return 0;
  }
}

struct obj *parse_string(struct parser *parser) {
  size_t strlen = 0;
  parser->cursor += 3; // skip s"_
  char *start = parser->cursor;
  while (parser->cursor[0] != '"') {
    if (parser->cursor[0] == '\0') {
      fprintf(stderr, "string not closed; program quitting\n");
      exit(0);
      return NULL;
    }
    parser->cursor++;
    strlen++;
  }

  struct obj *o = obj_string_new(start, strlen);
  return o;
}

struct obj *parse_flash_msg(struct parser *parser) {
  parser->cursor += 3; // skip ".( "
  char *start = parser->cursor;
  while (parser->cursor[0] != '\0' && memcmp(parser->cursor, " )", 2) != 0) {
    parser->cursor++;
  }

  if (parser->cursor[0] == '\0') {
    fprintf(stderr, "error: unclosed flash string\n");
    return NULL;
  }

  size_t len = parser->cursor - start;
  struct obj *result = obj_flash_msg_new(start, len);
  parser->cursor += 2; // set cursor over " )"
  return result;
}

// struct obj *parse_word(struct parser *parser) {
//   size_t len = 0;
//   parser->cursor += 2; // skip ": "

//   while (parser->cursor[0] != ';') {
//     if (parser->cursor[0] == '\0') {
//       fprintf(stderr, "word not closed\n");
//       exit(0);
//     } // TODO refactor
//   }
// }

struct obj *parse_symbol(struct parser *parser) {
  char *start = parser->cursor;

  while (parser->cursor[0] && is_symbol_char(parser->cursor[0])) {
    parser->cursor++;
  }
  size_t len = parser->cursor - start;

  struct obj *o = obj_symbol_new(start, len);
  return o;
}

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
      // } else if (parser->cursor[0] && memcmp(parser->cursor, ": ", 2) == 0) {
      //   object = parse_word(parser);
      //   obj_print(object);
      //   continue; // skip program push
    } else {
      parser->cursor++;
    }

    if (object != NULL) {
      obj_list_push(parsed, object);
    }
  }

  return parsed;
}
