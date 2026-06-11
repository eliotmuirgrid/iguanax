/*
** Garbage Collector
** See Copyright Notice in lua.h
*/

#ifndef lgc_h
#define lgc_h


#include "lobject.h"


/*
** Possible states of the Garbage Collector
*/
#define GCSpause   0
#define GCSpropagate   1
#define GCSsweepstring   2
#define GCSsweep   3
#define GCSfinalize   4


/*
** some userful bit tricks
*/
#define resetbits(x,m)   ((x) &= cast(lux_byte, ~(m)))
#define setbits(x,m)   ((x) |= (m))
#define testbits(x,m)   ((x) & (m))
#define bitmask(b)   (1<<(b))
#define bit2mask(b1,b2)   (bitmask(b1) | bitmask(b2))
#define l_setbit(x,b)   setbits(x, bitmask(b))
#define resetbit(x,b)   resetbits(x, bitmask(b))
#define testbit(x,b)   testbits(x, bitmask(b))
#define set2bits(x,b1,b2)   setbits(x, (bit2mask(b1, b2)))
#define reset2bits(x,b1,b2)   resetbits(x, (bit2mask(b1, b2)))
#define test2bits(x,b1,b2)   testbits(x, (bit2mask(b1, b2)))



/*
** Layout for bit use in `marked' field:
** bit 0 - object is white (type 0)
** bit 1 - object is white (type 1)
** bit 2 - object is black
** bit 3 - for userdata: has been finalized
** bit 3 - for tables: has weak keys
** bit 4 - for tables: has weak values
** bit 5 - object is fixed (should not be collected)
** bit 6 - object is "super" fixed (only the main thread)
*/


#define WHITE0BIT   0
#define WHITE1BIT   1
#define BLACKBIT   2
#define FINALIZEDBIT   3
#define KEYWEAKBIT   3
#define VALUEWEAKBIT   4
#define FIXEDBIT   5
#define SFIXEDBIT   6
#define WHITEBITS   bit2mask(WHITE0BIT, WHITE1BIT)


#define iswhite(x)      test2bits((x)->gch.marked, WHITE0BIT, WHITE1BIT)
#define isblack(x)      testbit((x)->gch.marked, BLACKBIT)
#define isgray(x)   (!isblack(x) && !iswhite(x))

#define otherwhite(g)   (g->currentwhite ^ WHITEBITS)
#define isdead(g,v)   ((v)->gch.marked & otherwhite(g) & WHITEBITS)

#define changewhite(x)   ((x)->gch.marked ^= WHITEBITS)
#define gray2black(x)   l_setbit((x)->gch.marked, BLACKBIT)

#define valiswhite(x)   (iscollectable(x) && iswhite(gcvalue(x)))

#define luaxC_white(g)   cast(lux_byte, (g)->currentwhite & WHITEBITS)


#define luaxC_checkGC(L) { \
  condhardstacktests(luaxD_reallocstack(L, L->stacksize - EXTRA_STACK - 1)); \
  if (G(L)->totalbytes >= G(L)->GCthreshold) \
   luaxC_step(L); }


#define luaxC_barrier(L,p,v) { if (valiswhite(v) && isblack(obj2gco(p)))  \
   luaxC_barrierf(L,obj2gco(p),gcvalue(v)); }

#define luaxC_barriert(L,t,v) { if (valiswhite(v) && isblack(obj2gco(t)))  \
   luaxC_barrierback(L,t); }

#define luaxC_objbarrier(L,p,o)  \
   { if (iswhite(obj2gco(o)) && isblack(obj2gco(p))) \
      luaxC_barrierf(L,obj2gco(p),obj2gco(o)); }

#define luaxC_objbarriert(L,t,o)  \
   { if (iswhite(obj2gco(o)) && isblack(obj2gco(t))) luaxC_barrierback(L,t); }

LUAXI_FUNC size_t luaxC_separateudata (luax_State *L, int all);
LUAXI_FUNC void luaxC_callGCTM (luax_State *L);
LUAXI_FUNC void luaxC_freeall (luax_State *L);
LUAXI_FUNC void luaxC_step (luax_State *L);
LUAXI_FUNC void luaxC_fullgc (luax_State *L);
LUAXI_FUNC void luaxC_link (luax_State *L, GCObjectx *o, lux_byte tt);
LUAXI_FUNC void luaxC_linkupval (luax_State *L, xUpVal *uv);
LUAXI_FUNC void luaxC_barrierf (luax_State *L, GCObjectx *o, GCObjectx *v);
LUAXI_FUNC void luaxC_barrierback (luax_State *L, xTable *t);


#endif
