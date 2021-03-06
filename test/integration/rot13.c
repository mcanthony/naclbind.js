#include "rot13.h"
#include <stdlib.h>
#include <string.h>

void nb_var_release(struct PP_Var);
struct PP_Var nb_var_string_create(const char*, uint32_t len);

void rot13(char* s, size_t len) {
  int i;
  for (i = 0; i < len; ++i) {
    if (*s >= 'a' && *s <= 'z') {
      *s = (((*s - 'a') + 13) % 26) + 'a';
    } else if (*s >= 'A' && *s <= 'Z') {
      *s = (((*s - 'A') + 13) % 26) + 'A';
    }

    s++;
  }
}

struct PP_Var char_to_var(char* s) {
  return nb_var_string_create(s, strlen(s));
}

void var_release(struct PP_Var var) {
  nb_var_release(var);
}
