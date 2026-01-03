#pragma once

struct context;

int calc(struct context *ctx, char *name);
int display(struct context *ctx, char *name);
int dup(struct context *ctx, char *name);
int string_concat(struct context *ctx, char *name);
