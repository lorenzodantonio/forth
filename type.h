#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OBJ_TYPE_INT 0
#define OBJ_TYPE_STR 1
#define OBJ_TYPE_LIST 2
#define OBJ_TYPE_SYMBOL 3
#define OBJ_TYPE_FLASH_MSG 4
#define OBJ_TYPE_WORD 5

/* Compact type system */
typedef uint64_t obj; // atom

static inline int64_t obj_to_int(obj v) {
  return (int64_t)(v & 0xFFFFFFFFFFFF);
}
static inline void *obj_to_ptr(obj v) {
  return (void *)(uintptr_t)(v & 0xFFFFFFFFFFFF);
};
static inline uint16_t which_type(obj v) { return v >> 48; };

static inline obj obj_int_new(int n) {
  return ((obj)OBJ_TYPE_INT << 48 | ((uint16_t)n & 0xFFFFFFFFFFFF));
}

char *obj_int_repr(obj self);

struct string {
  size_t refs;
  size_t len;
  char value[];
};

static inline struct string *string_new(char *buffer, size_t len) {
  struct string *str = malloc(sizeof(struct string) + len + 1);
  memcpy(str->value, buffer, len);
  str->value[len] = '\0';
  str->len = len;
  str->refs = 1;
  return str;
};

static inline obj obj_string_new(char *buffer, size_t len) {
  struct string *str = string_new(buffer, len);
  return ((obj)OBJ_TYPE_STR << 48) | ((uintptr_t)str & 0xFFFFFFFFFFFF);
}

char *obj_string_repr(obj value);

struct list {
  size_t refs;
  size_t count;
  size_t capacity;
  obj *data;
};

static inline struct list *list_new(size_t initial_capacity) {
  struct list *lst = malloc(sizeof(*lst));
  lst->refs = 1;
  lst->count = 0;
  lst->data = malloc(sizeof(obj) * initial_capacity);
  lst->capacity = initial_capacity;
  return lst;
}

static inline void list_push(struct list *lst, obj entry) {
  if (lst->count == lst->capacity) {
    size_t new_capacity = lst->capacity * 2;
    lst->data = realloc(lst->data, sizeof(obj) * new_capacity);
    lst->capacity = new_capacity;
  }
  // consider retain role
  lst->data[lst->count++] = entry;
}

static inline obj list_pop(struct list *lst) {
  return lst->data[(lst->count--) - 1];
}

static inline obj obj_list_new(size_t initial_capacity) {
  struct list *lst = list_new(initial_capacity);
  return ((obj)OBJ_TYPE_LIST << 48) | ((uintptr_t)lst & 0xFFFFFFFFFFFF);
}

static inline void obj_list_push(obj lst, obj entry) {
  struct list *l = obj_to_ptr(lst);
  return list_push(l, entry);
}

static inline obj obj_list_pop(obj lst) {
  struct list *l = obj_to_ptr(lst);
  return list_pop(l);
}

static inline obj obj_list_peek(obj lst) {
  struct list *l = obj_to_ptr(lst);
  return l->data[l->count - 1];
}

static inline obj obj_symbol_new(char *buffer, size_t len) {
  struct string *str = string_new(buffer, len);
  return ((obj)OBJ_TYPE_SYMBOL << 48 | ((uintptr_t)str & 0xFFFFFFFFFFFF));
}

static inline obj obj_word_new(size_t count) {
  struct list *lst = list_new(count);
  return ((obj)OBJ_TYPE_LIST << 48) | ((uintptr_t)lst & 0xFFFFFFFFFFFF);
}

static inline obj obj_flash_msg_new(char *buffer, size_t len) {
  struct string *str = string_new(buffer, len);
  return ((obj)OBJ_TYPE_FLASH_MSG << 48 | ((uintptr_t)str & 0xFFFFFFFFFFFF));
}

void obj_print(obj o);

// interface system

struct context;
struct trait {
  char *(*repr)(obj self);
};

extern const struct trait int_trait;
extern const struct trait str_trait;
extern struct trait traits[16];

// typedef void (*show)(struct context *ctx, obj self);
// typedef void (*unary_do)(struct context *ctx, obj self);
// typedef void (*binary_do)(struct context *ctx, obj self, obj other);
// typedef void (*free)(obj self);
