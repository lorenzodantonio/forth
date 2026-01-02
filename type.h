#pragma once

#include <stdlib.h>

#define OBJ_TYPE_INT 0
#define OBJ_TYPE_STR 1
#define OBJ_TYPE_LIST 2
#define OBJ_TYPE_SYMBOL 3
#define OBJ_TYPE_FLASH_MSG 4

/* types  */

struct obj {
  size_t refs;
  int type;
  union {
    int num;
    struct {
      size_t len;
      char *value;
    } string;
    struct {
      size_t len;
      size_t capacity;
      struct obj **data;
    } list;
  };
};

struct obj *obj_string_new(char *s, size_t length);
int obj_string_compare(struct obj *item, struct obj *other);
struct obj *obj_string_concat(struct obj *a, struct obj *b);

struct obj *obj_symbol_new(char *s, size_t length);
struct obj *obj_flash_msg_new(char *s, size_t length);

struct obj *obj_list_new(void);
void obj_list_push(struct obj *o, struct obj *item);
struct obj *obj_list_pop(struct obj *o);
struct obj *obj_list_peek(struct obj *o);

struct obj *obj_int_new(int n);

void obj_print(struct obj *o);
void obj_free(struct obj *o);

void obj_retain(struct obj *o);
void obj_release(struct obj *o);
