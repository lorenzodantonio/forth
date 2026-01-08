#include "context.h"
#include "func.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int ctx_stack_check_len(struct context *ctx, size_t minlen) {
  return ctx->stack->count >= minlen;
}

struct context *ctx_new(void) {
  struct context *ctx = malloc(sizeof(*ctx));
  ctx->dict.words = malloc(sizeof(struct word *) * 16);
  ctx->dict.capacity = 16;
  ctx->dict.count = 0;

  // Inizializzazione Ring 0 dello stack
  ctx->stack = list_new(32);

  ctx_dict_add_builtin(ctx, "+", calc);
  ctx_dict_add_builtin(ctx, "-", calc);
  ctx_dict_add_builtin(ctx, ".", display);
  ctx_dict_add_builtin(ctx, "concat", string_concat);
  ctx_dict_add_builtin(ctx, "dup", dup);

  return ctx;
}

obj ctx_stack_pop(struct context *ctx) {
  return ctx->stack->data[--ctx->stack->count];
}

obj ctx_stack_peek(struct context *ctx) {
  return ctx->stack->data[ctx->stack->count - 1];
}

void ctx_stack_push(struct context *ctx, obj v) { list_push(ctx->stack, v); }

void ctx_free(struct context *ctx) {
  if (ctx->stack->count != 0) {
    fprintf(stderr, "Warning: %zu items left unprocessed in the stack\n",
            ctx->stack->count);
  }
}

struct word *ctx_dict_find(struct context *ctx, obj name) {
  for (size_t i = 0; i < ctx->dict.count; i++) {
    struct word *w = ctx->dict.words[i];
    struct string *sw = (struct string *)obj_to_ptr(w->name);
    struct string *s = (struct string *)obj_to_ptr(name);
    if (strcmp(sw->value, s->value) == 0) {
      return w;
    }
  }
  return NULL;
}

struct word *_ctx_reserve(struct context *ctx, char *name) {
  obj oname = obj_string_new(name, strlen(name));
  struct word *w = ctx_dict_find(ctx, oname);
  if (w != NULL) {
    return w;
  }

  if (ctx->dict.count == ctx->dict.capacity) {
    size_t new_capacity = ctx->dict.capacity * 2;
    ctx->dict.words =
        realloc(ctx->dict.words, sizeof(struct word *) * new_capacity);
    ctx->dict.capacity = new_capacity;
  }
  w = malloc(sizeof(*w));
  w->name = oname;
  ctx->dict.words[ctx->dict.count++] = w;
  return w;
}

void ctx_dict_add_builtin(struct context *ctx, char *name, function callback) {
  struct word *w = _ctx_reserve(ctx, name);
  w->deftype = FUNC_TYPE_BUILTIN;
  w->builtin_func = callback;
}

void ctx_dict_add_userdef(struct context *ctx, char *name, obj callback) {
  struct word *w = _ctx_reserve(ctx, name);
  w->deftype = FUNC_TYPE_USERDEF;
  w->user_func = callback;
}

void symbol_exec(struct context *ctx, obj symbol) {
  struct word *w = ctx_dict_find(ctx, symbol);
  struct string *s = obj_to_ptr(symbol);

  if (w == NULL) {
    fprintf(stderr, "symbol %s not found\n", s->value);
    return;
  }

  switch (w->deftype) {
  case FUNC_TYPE_BUILTIN: {
    int result = w->builtin_func(ctx, s->value);
    if (result == -1) {
      fprintf(stderr, "an error occurred calling symbol: %s\n", s->value);
    }
    break;
  }
  case FUNC_TYPE_USERDEF:
    eval(ctx, w->user_func);
    break;
  }
}

void eval(struct context *ctx, obj o) {
  uint16_t t = which_type(o);
  switch (t) {
  case OBJ_TYPE_LIST: {
    struct list *l = obj_to_ptr(o);
    for (size_t i = 0; i < l->count; i++) {
      eval(ctx, l->data[i]);
    }
    break;
  }
  case OBJ_TYPE_SYMBOL:
    symbol_exec(ctx, o);
    break;
  case OBJ_TYPE_WORD:
    obj name = obj_list_pop(o);
    struct string *str = (struct string *)obj_to_ptr(name);
    obj clean_list = ((obj)OBJ_TYPE_LIST << 48) | (o & 0xFFFFFFFFFFFF);
    ctx_dict_add_userdef(ctx, str->value, clean_list);
    break;
  default:
    ctx_stack_push(ctx, o);
    break;
  }
}
