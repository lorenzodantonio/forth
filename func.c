#include "func.h"
#include "context.h"
#include <stdio.h>

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

int display(struct context *ctx, char *name) {
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
