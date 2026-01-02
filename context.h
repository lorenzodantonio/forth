#pragma once
#include "type.h"

struct context;

typedef int (*function)(struct context *ctx, char *name);

struct fn_table_entry {
  struct obj *name;
  function callback;
};

struct fn_table {
  struct fn_table_entry **data;
  size_t count;
};

struct context {
  struct obj *stack;
  struct fn_table functions;
};

struct context *ctx_new(void);
void ctx_free(struct context *ctx);
struct obj *ctx_stack_pop(struct context *ctx);
struct obj *ctx_stack_peek(struct context *ctx);
void ctx_stack_push(struct context *ctx, struct obj *o);
int ctx_stack_check_len(struct context *ctx, size_t minlen);

struct fn_table_entry *get_function_by_name(struct context *ctx,
                                            struct obj *name);

void register_function(struct context *ctx, char *name, function callback);
void call_symbol(struct context *ctx, struct obj *symbol);
