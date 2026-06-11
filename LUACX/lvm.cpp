// Highly hacked cut down copy of

#include <stdio.h>
#include <string.h>

#include "lua.h"

#include "ldebug.h"
#include "lstring.h"
#include "lvm.h"


#include "ifware.h"
#include <COL/COLlog.h>
COL_LOG_MODULE;

#define toxstring(L,o) ((txtype(o) == LUAX_TSTRING) || (luaxV_toxstring(L, o)))

const TXValue *luaxV_toxnumber (const TXValue *obj, TXValue *n) {
  COL_FUNCTION(luaV_toxnumber);
  lua_Number num;
  if (ttisnumber(obj)) return obj;
  if (ttisstring(obj) && luaxO_str2d(svalue(obj), &num)) {
    setnvalue(n, num);
    return n;
  }
  else
    return NULL;
}


int luaxV_toxstring (luax_State *L, StkIdx obj) {
  COL_FUNCTION(luaV_toxstring);
  if (!ttisnumber(obj))
  {
    COL_TRC("Not a number to return 0");
    return 0;
  }
  else {
    char s[LUAI_MAXNUMBER2STR];
    lua_Number n = nvalue(obj);
    lua_number2str(s, LUAI_MAXNUMBER2STR, n);
    setsvalue2s(L, obj, luaxS_new(L, s));
    return 1;
  }
}

void luaxV_concat (luax_State *L, int total, int last) {
  COL_FUNCTION(luaxV_concat);
  do {
    StkIdx top = L->base + last + 1;
    int n = 2;  /* number of elements handled in this pass (at least 2) */
    if (tsvalue(top-1)->len == 0)  /* second op is empty? */
      (void)toxstring(L, top - 2);  /* result is first op (as string) */
    else {
      /* at least two string values; get as many as possible */
      size_t tl = tsvalue(top-1)->len;
      char *buffer;
      int i;
      /* collect total length */
      for (n = 1; n < total && toxstring(L, top-n-1); n++) {
        size_t l = tsvalue(top-n-1)->len;
        if (l >= MAX_SIZET - tl) luaxG_runerror(L, "string length overflow");
        tl += l;
      }
      buffer = luaxZ_openspace(L, &G(L)->buff, tl);
      tl = 0;
      for (i=n; i>0; i--) {  /* concat all strings */
        size_t l = tsvalue(top-i)->len;
        memcpy(buffer+tl, svalue(top-i), l);
        tl += l;
      }
      setsvalue2s(L, top-n, luaxS_newlstr(L, buffer, tl));
    }
    total -= n-1;  /* got `n' strings to create 1 new */
    last -= n-1;
  } while (total > 1);  /* repeat until only 1 result left */
}
