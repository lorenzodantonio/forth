#include "context.h"
#include "func.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int ctx_stack_check_len(struct context *ctx, size_t minlen) {
  return ctx->stack->list.len >= minlen;
}

struct context *ctx_new(void) {
  struct context *ctx = malloc(sizeof(*ctx));
  ctx->dict.words = malloc(sizeof(struct word *) * 16);
  ctx->dict.capacity = 16;
  ctx->dict.count = 0;
  ctx->stack = obj_list_new();

  ctx_dict_add_builtin(ctx, "+", calc);
  ctx_dict_add_builtin(ctx, "-", calc);
  ctx_dict_add_builtin(ctx, ".", display);
  ctx_dict_add_builtin(ctx, "concat", string_concat);
  ctx_dict_add_builtin(ctx, "dup", dup);

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

struct word *_ctx_reserve(struct context *ctx, char *name) {
  struct obj *oname = obj_string_new(name, strlen(name));
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
  oname->refs++;
  ctx->dict.words[ctx->dict.count++] = w;
  return w;
}

void ctx_dict_add_builtin(struct context *ctx, char *name, function callback) {
  struct word *w = _ctx_reserve(ctx, name);
  w->deftype = FUNC_TYPE_BUILTIN;
  w->builtin_func = callback;
}

void ctx_dict_add_userdef(struct context *ctx, char *name,
                          struct obj *callback) {
  struct word *w = _ctx_reserve(ctx, name);
  w->deftype = FUNC_TYPE_USERDEF;
  w->user_func = callback;
}

void symbol_exec(struct context *ctx, struct obj *symbol) {
  struct word *w = ctx_dict_find(ctx, symbol);
  if (w == NULL) {
    fprintf(stderr, "symbol %s not found\n", symbol->string.value);
    return;
  }

  switch (w->deftype) {
  case FUNC_TYPE_BUILTIN:
    int result = w->builtin_func(ctx, symbol->string.value);
    if (result == -1) {
      fprintf(stderr, "an error occurred calling symbol: %s\n",
              symbol->string.value);
    }
    break;
  case FUNC_TYPE_USERDEF:
    eval(ctx, w->user_func);
    break;
  }
}

void eval(struct context *ctx, struct obj *obj) {
  switch (obj->type) {
  case OBJ_TYPE_LIST:
    for (size_t i = 0; i < obj->list.len; i++) {
      eval(ctx, obj->list.data[i]);
    }
    break;
  case OBJ_TYPE_FLASH_MSG:
    obj_print(obj);
    break;
  case OBJ_TYPE_SYMBOL:
    symbol_exec(ctx, obj);
    break;
  case OBJ_TYPE_WORD:
    struct obj *name = obj_list_pop(obj);
    obj->type = OBJ_TYPE_LIST;
    ctx_dict_add_userdef(ctx, name->string.value, obj);
    // obj_retain(obj);
    break;
  default:
    ctx_stack_push(ctx, obj);
    obj_retain(obj);
  }
}
