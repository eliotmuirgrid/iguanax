/*
** String table (keeps all strings handled by Lua)
** See Copyright Notice in lua.h
*/


#include <string.h>

#define lstring_c
#define LUAX_CORE

#include "lua.h"

#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"


#include "ifware.h"
#include <COL/COLlog.h>
COL_LOG_MODULE;


void luaxS_resize (luax_State *L, int newsize) {
  GCObjectx **newhash;
  stringtable *tb;
  int i;
  if (G(L)->gcstate == GCSsweepstring)
    return;  /* cannot resize during GC traverse */
  newhash = luaxM_newvector(L, newsize, GCObjectx *);
  tb = &G(L)->strt;
  for (i=0; i<newsize; i++) newhash[i] = NULL;
  /* rehash */
  for (i=0; i<tb->size; i++) {
    GCObjectx *p = tb->hash[i];
    while (p) {  /* for each node in the list */
      GCObjectx *next = p->gch.next;  /* save next */
      unsigned int h = gco2ts(p)->hash;
      int h1 = lmod(h, newsize);  /* new position */
      lua_assert(cast_int(h%newsize) == lmod(h, newsize));
      p->gch.next = newhash[h1];  /* chain it */
      newhash[h1] = p;
      p = next;
    }
  }
  luaxM_freearray(L, tb->hash, tb->size, TStringx *);
  tb->size = newsize;
  tb->hash = newhash;
}


static TStringx *newlstr (luax_State *L, const char *str, size_t l,
                                       unsigned int h) {
  COL_FUNCTION(newlstr);
  TStringx *ts;
  stringtable *tb;
  if (l+1 > (MAX_SIZET - sizeof(TStringx))/sizeof(char))
    luaxM_toobig(L);
  ts = cast(TStringx *, luaxM_malloc(L, (l+1)*sizeof(char)+sizeof(TStringx)));
  ts->tsv.len = l;
  ts->tsv.hash = h;
  ts->tsv.marked = luaxC_white(G(L));
  ts->tsv.tt = LUAX_TSTRING;
  ts->tsv.reserved = 0;
  memcpy(ts+1, str, l*sizeof(char));
  ((char *)(ts+1))[l] = '\0';  /* ending 0 */
  tb = &G(L)->strt;
  h = lmod(h, tb->size);
  ts->tsv.next = tb->hash[h];  /* chain new entry */
  tb->hash[h] = obj2gco(ts);
  tb->nuse++;
  if (tb->nuse > cast(lu_int32, tb->size) && tb->size <= MAX_INT/2)
    luaxS_resize(L, tb->size*2);  /* too crowded */
  return ts;
}


TStringx *luaxS_newlstr (luax_State *L, const char *str, size_t l) {
  COL_FUNCTION(luaxS_newlstr);
  GCObjectx *o;
  unsigned int h = cast(unsigned int, l);  /* seed */
  size_t step = (l>>5)+1;  /* if string is too long, don't hash all its chars */
  size_t l1;
  for (l1=l; l1>=step; l1-=step)  /* compute hash */
    h = h ^ ((h<<5)+(h>>2)+cast(unsigned char, str[l1-1]));
  for (o = G(L)->strt.hash[lmod(h, G(L)->strt.size)];
       o != NULL;
       o = o->gch.next) {
    TStringx *ts = rawgco2ts(o);
    if (ts->tsv.len == l && (memcmp(str, getstr(ts), l) == 0)) {
      /* string may be dead */
      if (isdead(G(L), o)) changewhite(o);
      return ts;
    }
  }
  return newlstr(L, str, l, h);  /* not found */
}


