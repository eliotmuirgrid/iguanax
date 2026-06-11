// Hacked source code from Lua - we just wanted the lexer and have cut out everything else.

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "lauxlib.h"

static void *l_alloc (void *ud, void *ptr, size_t osize, size_t nsize) {
  (void)ud;
  (void)osize;
  if (nsize == 0) {
    free(ptr);
    return NULL;
  }
  else
    return realloc(ptr, nsize);
}


static int panic (luax_State *L) {
  (void)L;  /* to avoid warnings */
  fprintf(stderr, "PANIC: unprotected error in call to Lua API (%s)\n",
                   lua_toxstring(L, -1));
  return 0;
}


LUALIB_API luax_State *luaxL_newstate (void) {
  luax_State *L = luax_newstate(l_alloc, NULL);
  if (L) lua_atpanic(L, &panic);
  return L;
}

