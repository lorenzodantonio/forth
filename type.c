#include "type.h"
#include <stdio.h>
#include <string.h>

struct obj *obj_new(int type) {
  struct obj *o = malloc(sizeof(*o));
  o->refs = 0;
  o->type = type;
  return o;
}

void obj_retain(struct obj *o) { o->refs++; };
void obj_release(struct obj *o) {
  o->refs--;
  if (o->refs == 1) {
    obj_free(o);
  }
}

struct obj *obj_string_new(char *s, size_t length) {
  struct obj *o = obj_new(OBJ_TYPE_STR);
  o->string.value = malloc(length + 1);
  o->string.len = length;
  memcpy(o->string.value, s, length + 1);
  o->string.value[length] = 0;
  return o;
}

int obj_string_compare(struct obj *item, struct obj *other) {
  size_t minlen = item->string.len < other->string.len ? item->string.len
                                                       : other->string.len;
  int cmp = memcmp(item->string.value, other->string.value, minlen);

  if (cmp == 0) {
    if (item->string.len == other->string.len)
      return 0;
    else if (item->string.len < other->string.len)
      return -1;
  }

  return cmp;
}

struct obj *obj_string_concat(struct obj *a, struct obj *b) {
  size_t new_len = a->string.len + b->string.len;
  char *ccat = malloc(new_len + 1);
  memcpy(ccat, b->string.value, b->string.len);
  memcpy(ccat + b->string.len, a->string.value,
         a->string.len + 1); // also copy string terminator

  return obj_string_new(ccat, new_len);
}

void obj_string_free(struct obj *o) {
  free(o->string.value);
  free(o);
}

struct obj *obj_symbol_new(char *s, size_t length) {
  struct obj *o = obj_string_new(s, length);
  o->type = OBJ_TYPE_SYMBOL;
  return o;
}

struct obj *obj_flash_msg_new(char *s, size_t length) {
  struct obj *o = obj_string_new(s, length);
  o->type = OBJ_TYPE_FLASH_MSG;
  return o;
}

struct obj *obj_int_new(int n) {
  struct obj *o = obj_new(OBJ_TYPE_INT);
  o->num = n;
  return o;
}

struct obj *obj_list_new(void) {
  struct obj *o = obj_new(OBJ_TYPE_LIST);
  o->list.data = (struct obj **)malloc(sizeof(struct obj *) * 32);
  o->list.len = 0;
  o->list.capacity = 32;

  return o;
}

void obj_list_free(struct obj *o) {
  for (size_t i = 0; i < o->list.len; i++) {
    obj_free(o->list.data[i]);
  }
}

void obj_list_push(struct obj *o, struct obj *item) {
  if (o->list.len == o->list.capacity) {
    size_t new_capacity = o->list.len * 2;
    o->list.data = realloc(o->list.data, sizeof(struct obj *) * new_capacity);
    o->list.capacity = new_capacity;
  }
  o->list.data[o->list.len++] = item;
}

struct obj *obj_list_pop(struct obj *o) {
  return o->list.data[--(o->list).len];
}

struct obj *obj_list_peek(struct obj *o) {
  return o->list.data[o->list.len - 1];
}

struct obj *obj_word_new(char *name, struct obj *subprogram) {
  struct obj *o = obj_list_new();
  o->type = OBJ_TYPE_WORD;
  obj_list_push(o, subprogram);
  return o;
}

void obj_print(struct obj *o) {
  switch (o->type) {
  case OBJ_TYPE_INT:
    printf("%d", o->num);
    break;
  case OBJ_TYPE_STR:
  case OBJ_TYPE_FLASH_MSG:
  case OBJ_TYPE_SYMBOL:
    printf("\"%s\"", o->string.value);
    break;
  case OBJ_TYPE_LIST:
  case OBJ_TYPE_WORD: // check
    for (size_t i = 0; i < o->list.len; i++) {
      obj_print(o->list.data[i]);
    }
    break;
  }
}

void obj_free(struct obj *o) {
  switch (o->type) {
  case OBJ_TYPE_INT:
    break;
  case OBJ_TYPE_STR:
  case OBJ_TYPE_FLASH_MSG:
  case OBJ_TYPE_SYMBOL:
    free(o->string.value);
    break;
  case OBJ_TYPE_LIST:
  case OBJ_TYPE_WORD:
    for (size_t i = 0; i < o->list.len; i++) {
      obj_release(o->list.data[i]);
    }
    free(o->list.data);
    break;
  }
  free(o);
}
