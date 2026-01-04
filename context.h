#pragma once
#include "type.h"

#define FUNC_TYPE_BUILTIN 0
#define FUNC_TYPE_USERDEF 1

struct context;

typedef int (*function)(struct context *ctx, char *name);

struct word {
  struct obj *name;
  int deftype;
  union {
    function builtin_func;
    struct obj *user_func;
  };
};

struct dictionary {
  struct word **words;
  size_t count;
  size_t capacity;
};

struct context {
  struct obj *stack;
  struct dictionary dict;
};

struct context *ctx_new(void);
void ctx_free(struct context *ctx);
struct obj *ctx_stack_pop(struct context *ctx);
struct obj *ctx_stack_peek(struct context *ctx);
void ctx_stack_push(struct context *ctx, struct obj *o);
int ctx_stack_check_len(struct context *ctx, size_t minlen);

struct word *ctx_dict_find(struct context *ctx, struct obj *name);
void ctx_dict_add_builtin(struct context *ctx, char *name, function callback);
void ctx_dict_add_userdef(struct context *ctx, char *name,
                          struct obj *callback);

void symbol_exec(struct context *ctx, struct obj *symbol);
void eval(struct context *ctx, struct obj *o);
