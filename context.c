#include "context.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int ctx_stack_check_len(struct context *ctx, size_t minlen) {
  return ctx->stack->list.len >= minlen;
}

int calc(struct context *ctx, char *name) {
  if (!ctx_stack_check_len(ctx, 2)) {
    fprintf(stderr, "found %d\n", ctx->stack->list.len);
    fprintf(stderr, "invalid parameters count\n");
    return -1;
  }

  struct obj *a = ctx_stack_pop(ctx);
  struct obj *b = ctx_stack_pop(ctx);

  int result;

  switch (name[0]) {
  case '+':
    result = a->num + b->num;
    break;
  case '-':
    result = a->num - b->num;
    break;
  default:
    fprintf(stderr, "not implemented yet\n");
  }
  obj_release(a);
  obj_release(b);
  ctx_stack_push(ctx, obj_int_new(result));

  return 0;
}

int print(struct context *ctx, char *name) {
  if (!ctx_stack_check_len(ctx, 1)) {
    return -1;
  }

  struct obj *o = ctx_stack_pop(ctx);
  obj_print(o);
  obj_release(o);
  return 0;
}

int dup(struct context *ctx, char *name) {
  if (!ctx_stack_check_len(ctx, 1)) {
    return -1;
  }
  struct obj *target = ctx_stack_peek(ctx);
  ctx_stack_push(ctx, target);
  target->refs++;
  return 0;
}

int string_concat(struct context *ctx, char *name) {
  if (!ctx_stack_check_len(ctx, 2)) {
    return -1;
  }

  struct obj *a = ctx_stack_pop(ctx);
  struct obj *b = ctx_stack_pop(ctx);

  struct obj *result = obj_string_concat(a, b);

  ctx_stack_push(ctx, result);
  obj_release(a);
  obj_release(b);
  return 0;
}

struct context *ctx_new(void) {
  struct context *ctx = malloc(sizeof(*ctx));
  ctx->dict.words = malloc(sizeof(struct word *) * 16);
  ctx->dict.count = 0;
  ctx->stack = obj_list_new();

  ctx_dict_add(ctx, "+", calc);
  ctx_dict_add(ctx, "-", calc);
  ctx_dict_add(ctx, ".", print);
  ctx_dict_add(ctx, "concat", string_concat);
  ctx_dict_add(ctx, "dup", dup);

  return ctx;
}

struct obj *ctx_stack_pop(struct context *ctx) {
  struct obj *lastin = obj_list_pop(ctx->stack);
  return lastin;
}

struct obj *ctx_stack_peek(struct context *ctx) {
  return obj_list_peek(ctx->stack);
}

void ctx_stack_push(struct context *ctx, struct obj *o) {
  obj_list_push(ctx->stack, o);
}

void ctx_free(struct context *ctx) {
  if (ctx->stack->list.len != 0) {
    fprintf(stderr, "Warning: %zu items left unprocessed in the stack\n",
            ctx->stack->list.len);
  }
  obj_free(ctx->stack);
}

struct word *ctx_dict_find(struct context *ctx, struct obj *name) {
  for (size_t i = 0; i < ctx->dict.count; i++) {
    struct word *w = ctx->dict.words[i];
    if (obj_string_compare(name, w->name) == 0) {
      return w;
    }
  }
  return NULL;
}

void ctx_dict_add(struct context *ctx, char *name, function callback) {
  struct obj *oname = obj_string_new(name, strlen(name));
  struct word *w = ctx_dict_find(ctx, oname);
  if (w != NULL) {
    w->callback = callback;
  } else {
    struct word *w = malloc(sizeof(*w));
    w->name = oname;
    w->callback = callback;
    ctx->dict.words[ctx->dict.count++] = w;
  }
}

void call_symbol(struct context *ctx, struct obj *symbol) {
  struct word *w = ctx_dict_find(ctx, symbol);
  if (w == NULL) {
    fprintf(stderr, "symbol %s not found\n", symbol->string.value);
    return;
  }
  int n = w->callback(ctx, symbol->string.value);
  if (n == -1) {
    fprintf(stderr, "an error occurred calling symbol: %s\n",
            symbol->string.value);
  }
}
