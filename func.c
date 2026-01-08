#include "func.h"
#include "context.h"
#include <stdio.h>

int calc(struct context *ctx, char *name) {
  if (!ctx_stack_check_len(ctx, 2)) {
    fprintf(stderr, "found %d\n", ctx->stack->count);
    fprintf(stderr, "invalid parameters count\n");
    return -1;
  }

  int a = obj_to_int(ctx_stack_pop(ctx));
  int b = obj_to_int(ctx_stack_pop(ctx));

  obj result;
  switch (name[0]) {
  case '+':
    result = obj_int_new(a + b);
    break;
  case '-':
    result = obj_int_new(a - b);
    break;
  default:
    fprintf(stderr, "not implemented yet\n");
  }
  // obj_release(a);
  // obj_release(b);
  ctx_stack_push(ctx, obj_int_new(result));

  return 0;
}

int display(struct context *ctx, char *name) {
  if (!ctx_stack_check_len(ctx, 1)) {
    return -1;
  }

  obj o = ctx_stack_pop(ctx);
  char *s = traits[which_type(o)].repr(o);
  printf("%s", s);
  free(s);

  return 0;
}

int dup(struct context *ctx, char *name) {
  if (!ctx_stack_check_len(ctx, 1)) {
    return -1;
  }
  // struct obj *target = ctx_stack_peek(ctx);
  // ctx_stack_push(ctx, target);
  // target->refs++;
  return 0;
}

int string_concat(struct context *ctx, char *name) {
  if (!ctx_stack_check_len(ctx, 2)) {
    return -1;
  }

  // value a = ctx_stack_pop(ctx);
  // value b = ctx_stack_pop(ctx);

  // value result = obj_string_concat(a, b);

  // ctx_stack_push(ctx, result);
  // obj_release(a);
  // obj_release(b);
  return 0;
}
