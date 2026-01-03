#pragma once
#include "type.h"

struct context;

typedef int (*function)(struct context *ctx, char *name);

struct word {
  struct obj *name;
  function callback;
};

struct dictionary {
  struct word **words;
  size_t count;
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
void ctx_dict_add(struct context *ctx, char *name, function callback);

void symbol_exec(struct context *ctx, struct obj *symbol);
