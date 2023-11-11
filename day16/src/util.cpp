#include <cstdio>
#include <cstdlib>

#include "util.h"

void ErrorIf(bool condition, const char* errmsg) {
  if(condition) {
    perror(errmsg);
    exit(EXIT_FAILURE);
  }
} 