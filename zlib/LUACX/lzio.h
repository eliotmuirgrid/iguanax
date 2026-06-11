/*
** Buffered streams
** See Copyright Notice in lua.h
*/


#ifndef lzio_h
#define lzio_h

#include "lua.h"

#include "lmem.h"


#define EOZ   (-1)         /* end of stream */

typedef struct Ziox ZIOx;

#define char2int(c)   cast(int, cast(unsigned char, (c)))

#define zxgetc(z)  (((z)->n--)>0 ?  char2int(*(z)->p++) : luaxZ_fill(z))

typedef struct Mxbuffer {
  char *buffer;
  size_t n;
  size_t buffsize;
} Mxbuffer;

#define luaxZ_initbuffer(L, buff) ((buff)->buffer = NULL, (buff)->buffsize = 0)

#define luaxZ_buffer(buff)   ((buff)->buffer)
#define luaxZ_sizebuffer(buff)   ((buff)->buffsize)
#define luaxZ_bufflen(buff)   ((buff)->n)

#define luaxZ_resetbuffer(buff) ((buff)->n = 0)


#define luaxZ_resizebuffer(L, buff, size) \
   (luaxM_reallocvector(L, (buff)->buffer, (buff)->buffsize, size, char), \
   (buff)->buffsize = size)

#define luaxZ_freebuffer(L, buff)   luaxZ_resizebuffer(L, buff, 0)


LUAXI_FUNC char *luaxZ_openspace (luax_State *L, Mxbuffer *buff, size_t n);
LUAXI_FUNC void luaxZ_init (luax_State *L, ZIOx *z, luax_Reader reader,
                                        void *data);
LUAXI_FUNC size_t luaxZ_read (ZIOx* z, void* b, size_t n);   /* read next n bytes */
LUAXI_FUNC int luaxZ_lookahead (ZIOx *z);



/* --------- Private Part ------------------ */

struct Ziox {
  size_t n;         /* bytes still unread */
  const char *p;      /* current position in buffer */
  luax_Reader reader;
  void* data;         /* additional data */
  luax_State *L;         /* Lua state (for reader) */
};


LUAXI_FUNC int luaxZ_fill (ZIOx *z);

#endif
