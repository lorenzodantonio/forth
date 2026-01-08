#include "type.h"
#include <stdio.h>
#include <string.h>

void obj_print(obj o) {
  switch (which_type(o)) {
  case OBJ_TYPE_INT:
    printf("%d", obj_to_int(o));
    break;
  case OBJ_TYPE_STR:
  case OBJ_TYPE_FLASH_MSG:
  case OBJ_TYPE_SYMBOL:
    struct string *str = (struct string *)obj_to_ptr(o);
    printf("\"%s\"", str->value);
    break;
  case OBJ_TYPE_LIST:
  case OBJ_TYPE_WORD: // check
    struct list *list = (struct list *)obj_to_ptr(o);
    for (size_t i = 0; i < list->count; i++) {
      obj_print(list->data[i]);
    }
    break;
  }
}

void obj_free(obj o) {
  switch (which_type(o)) {
  case OBJ_TYPE_INT:
    break;
  case OBJ_TYPE_STR:
  case OBJ_TYPE_FLASH_MSG:
  case OBJ_TYPE_SYMBOL:
    struct string *str = (struct string *)obj_to_ptr(o);
    free(str);
    break;
  case OBJ_TYPE_LIST:
  case OBJ_TYPE_WORD:
    struct list *lst = (struct list *)obj_to_ptr(o);
    for (size_t i = 0; i < lst->count; i++) {
      // obj_release(o->list.data[i]);
      // TODO
    }
    free(lst->data);
    break;
  }
  // free(o);
}

const struct trait int_trait = {
    .repr = obj_int_repr,
};

const struct trait str_trait = {
    .repr = obj_string_repr,
};

struct trait traits[] = {
    [OBJ_TYPE_INT] = int_trait,
    [OBJ_TYPE_STR] = str_trait,
};

char *obj_int_repr(obj self) {
  char *buffer = malloc(16);
  if (!buffer)
    return NULL;

  sprintf(buffer, "%lld", (long long)obj_to_int(self));
  return buffer;
}

char *obj_string_repr(obj self) {
  char *buffer = malloc(256);
  sprintf(buffer, "\"%s\"", ((struct string *)obj_to_ptr(self))->value);
  return buffer;
}

// trait related functions
