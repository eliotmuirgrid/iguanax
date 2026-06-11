/*
** Lexical Analyzer
** See Copyright Notice in lua.h
*/

#ifndef llex_h
#define llex_h

#include "lobject.h"
#include "lzio.h"


#define FIRST_xRESERVED   257

/* maximum length of a reserved word */
#define TOKEN_LEN   (sizeof("function")/sizeof(char))


/*
* WARNING: if you change the order of this enumeration,
* grep "ORDER xRESERVED"
*/
enum xRESERVED {
  /* terminal symbols denoted by reserved words */
  TKX_AND = FIRST_xRESERVED, TKX_BREAK,
  TKX_DO, TKX_ELSE, TKX_ELSEIF, TKX_END, TKX_FALSE, TKX_FOR, TKX_FUNCTION,
  TKX_IF, TKX_IN, TKX_LOCAL, TKX_NIL, TKX_NOT, TKX_OR, TKX_REPEAT,
  TKX_RETURN, TKX_THEN, TKX_TRUE, TKX_UNTIL, TKX_WHILE,
  /* other terminal symbols */
  TKX_CONCAT, TKX_DOTS, TKX_EQ, TKX_GE, TKX_LE, TKX_NE, TKX_NUMBER,
  TKX_NAME, TKX_STRING, TKX_EOS
};

/* number of reserved words */
#define NUM_xRESERVED   (cast(int, TKX_WHILE-FIRST_xRESERVED+1))


/* array with token `names' */
LUAI_DATA const char *const luaxX_tokens [];


typedef union {
  lua_Number r;
  TStringx *ts;
} xSemInfo;  /* semantics information */


typedef struct xToken {
  int token;
  xSemInfo seminfo;
} xToken;


typedef struct luaxLexState {
  int current;  /* current character (charint) */
  int linenumber;  /* input line counter */
  int lastline;  /* line of last token `consumed' */
  xToken t;  /* current token */
  xToken lookahead;  /* look ahead token */
  struct xFuncState *fs;  /* `xFuncState' is private to the parser */
  struct luax_State *L;
  ZIOx *z;  /* input stream */
  Mxbuffer *buff;  /* buffer for tokens */
  TStringx *source;  /* current source name */
  char decpoint;  /* locale decimal point */
} luaxLexState;


LUAXI_FUNC void luaxX_init (luax_State *L);
LUAXI_FUNC void luaxX_setinput (luax_State *L, luaxLexState *ls, ZIOx *z,
                              TStringx *source);
LUAXI_FUNC TStringx *luaxX_newstring (luaxLexState *ls, const char *str, size_t l);
LUAXI_FUNC void luaxX_next (luaxLexState *ls);
LUAXI_FUNC void luaxX_lookahead (luaxLexState *ls);
LUAXI_FUNC void luaxX_lexerror (luaxLexState *ls, const char *msg, int token);
LUAXI_FUNC void luaxX_syntaxerror (luaxLexState *ls, const char *s);
LUAXI_FUNC const char *luaxX_token2str (luaxLexState *ls, int token);


#endif
