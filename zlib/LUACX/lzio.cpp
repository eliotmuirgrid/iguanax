/*
** a generic input stream interface
** See Copyright Notice in lua.h
*/


#include <string.h>

#define lzio_c
#define LUAX_CORE

#include "lua.h"

#include "llimits.h"
#include "lmem.h"
#include "lstate.h"
#include "lzio.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;


int luaxZ_fill (ZIOx *z) {
  COL_FUNCTION(luaxZ_fill);
  size_t size;
  luax_State *L = z->L;
  const char *buff;
  lua_unlock(L);
  buff = z->reader(L, z->data, &size);
  COL_TRC((buff != NULL ? COLstring("Read ") + COLintToString(size) + " bytes from reader." : COLstring("No data returned.")));
  lua_lock(L);
  if (buff == NULL || size == 0) return EOZ;
  z->n = size - 1;
  z->p = buff;
  return char2int(*(z->p++));
}


int luaxZ_lookahead (ZIOx *z) {
   COL_FUNCTION(luaxZ_lookahead);
   if (z->n == 0) {
    if (luaxZ_fill(z) == EOZ)
      return EOZ;
    else {
      z->n++;  /* luaxZ_fill removed first byte; put back it */
      z->p--;
    }
  }
  return char2int(*z->p);
}


void luaxZ_init (luax_State *L, ZIOx *z, luax_Reader reader, void *data) {
  COL_FUNCTION(luaxZ_init);
  z->L = L;
  z->reader = reader;
  z->data = data;
  z->n = 0;
  z->p = NULL;
}


/* --------------------------------------------------------------- read --- */
size_t luaxZ_read (ZIOx *z, void *b, size_t n) {
  COL_FUNCTION(luaxZ_read);
  while (n) {
    size_t m;
    if (luaxZ_lookahead(z) == EOZ)
      return n;  /* return number of missing bytes */
    m = (n <= z->n) ? n : z->n;  /* min. between n and z->n */
    memcpy(b, z->p, m);
    z->n -= m;
    z->p += m;
    b = (char *)b + m;
    n -= m;
  }
  return 0;
}

/* ------------------------------------------------------------------------ */
char *luaxZ_openspace (luax_State *L, Mxbuffer *buff, size_t n) {
  COL_FUNCTION(luaxZ_openspace);
  if (n > buff->buffsize) {
    if (n < LUAX_MINBUFFER) n = LUAX_MINBUFFER;
    luaxZ_resizebuffer(L, buff, n);
  }
  return buff->buffer;
}


