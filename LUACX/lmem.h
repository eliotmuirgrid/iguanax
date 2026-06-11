/*
** Interface to Memory Manager
** See Copyright Notice in lua.h
*/

#ifndef lmem_h
#define lmem_h


#include <stddef.h>

#include "llimits.h"
#include "lua.h"

#define MEMERRMSG   "not enough memory"


#define luaxM_reallocv(L,b,on,n,e) \
   ((cast(size_t, (n)+1) <= MAX_SIZET/(e)) ?  /* +1 to avoid warnings */ \
      luaxM_realloc_(L, (b), (on)*(e), (n)*(e)) : \
      luaxM_toobig(L))

#define luaxM_freemem(L, b, s)   luaxM_realloc_(L, (b), (s), 0)
#define luaxM_free(L, b)      luaxM_realloc_(L, (b), sizeof(*(b)), 0)
#define luaxM_freearray(L, b, n, t)   luaxM_reallocv(L, (b), n, 0, sizeof(t))

#define luaxM_malloc(L,t)   luaxM_realloc_(L, NULL, 0, (t))
#define luaxM_new(L,t)      cast(t *, luaxM_malloc(L, sizeof(t)))
#define luaxM_newvector(L,n,t) \
      cast(t *, luaxM_reallocv(L, NULL, 0, n, sizeof(t)))

#define luaxM_growvector(L,v,nelems,size,t,limit,e) \
          if ((nelems)+1 > (size)) \
            ((v)=cast(t *, luaxM_growaux_(L,v,&(size),sizeof(t),limit,e)))

#define luaxM_reallocvector(L, v,oldn,n,t) \
   ((v)=cast(t *, luaxM_reallocv(L, v, oldn, n, sizeof(t))))


LUAXI_FUNC void *luaxM_realloc_ (luax_State *L, void *block, size_t oldsize,
                                                          size_t size);
LUAXI_FUNC void *luaxM_toobig (luax_State *L);
LUAXI_FUNC void *luaxM_growaux_ (luax_State *L, void *block, int *size,
                               size_t size_elem, int limit,
                               const char *errormsg);

#endif

