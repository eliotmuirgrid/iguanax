/*
** Lexical Analyzer
** See Copyright Notice in lua.h
*/

#include <COL/COLlog.h>
COL_LOG_MODULE;


#include <ctype.h>
#include <locale.h>
#include <string.h>

#define llex_c
#define LUAX_CORE

#include "lua.h"

#include "ldo.h"
#include "llex.h"
#include "lobject.h"
#include "lparser.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "lzio.h"

#include "ifware.h"

#define next(ls) (ls->current = zxgetc(ls->z))




#define currIsNewline(ls)   (ls->current == '\n' || ls->current == '\r')


/* ORDER xRESERVED */
const char *const luaxX_tokens [] = {
    "and", "break", "do", "else", "elseif",
    "end", "false", "for", "function", "if",
    "in", "local", "nil", "not", "or", "repeat",
    "return", "then", "true", "until", "while",
    "..", "...", "==", ">=", "<=", "~=",
    "<number>", "<name>", "<string>", "<eof>",
    NULL
};


#define save_and_next(ls) (save(ls, ls->current), next(ls))


static void save (luaxLexState *ls, int c) {
  COL_FUNCTION(save);
  COL_VAR((char)ls->current);
  Mxbuffer *b = ls->buff;
  if (b->n + 1 > b->buffsize) {
    size_t newsize;
    if (b->buffsize >= MAX_SIZET/2)
      luaxX_lexerror(ls, "lexical element too long", 0);
    newsize = b->buffsize * 2;
    luaxZ_resizebuffer(ls->L, b, newsize);
  }
  b->buffer[b->n++] = cast(char, c);
}


void luaxX_init (luax_State *L) {
  COL_FUNCTION("luaxX_init");
  int i;
  for (i=0; i<NUM_xRESERVED; i++) {
    COL_VAR(luaxX_tokens[i]);
    TStringx *ts = luaxS_new(L, luaxX_tokens[i]);
    luaxS_fix(ts);  /* reserved words are never collected */
    lua_assert(strlen(luaxX_tokens[i])+1 <= TOKEN_LEN);
    ts->tsv.reserved = cast_byte(i+1);  /* reserved word */
  }
}


#define MAXSRC          80


const char *luaxX_token2str (luaxLexState *ls, int token) {
  if (token < FIRST_xRESERVED) {
    lua_assert(token == cast(unsigned char, token));
    return (iscntrl(token)) ? luaxO_pushfstring(ls->L, "char(%d)", token) :
                              luaxO_pushfstring(ls->L, "%c", token);
  }
  else
    return luaxX_tokens[token-FIRST_xRESERVED];
}


static const char *txtxToken (luaxLexState *ls, int token) {
  switch (token) {
    case TKX_NAME:
    case TKX_STRING:
    case TKX_NUMBER:
      save(ls, '\0');
      return luaxZ_buffer(ls->buff);
    default:
      return luaxX_token2str(ls, token);
  }
}


void luaxX_lexerror (luaxLexState *ls, const char *msg, int token) {
  char buff[MAXSRC];
  luaxO_chunkid(buff, getstr(ls->source), MAXSRC);
  msg = luaxO_pushfstring(ls->L, "%s:%d: %s", buff, ls->linenumber, msg);
  if (token)
    luaxO_pushfstring(ls->L, "%s near " LUAX_QS, msg, txtxToken(ls, token));
  luaxD_throw(ls->L, LUAX_ERRSYNTAX);
}


void luaxX_syntaxerror (luaxLexState *ls, const char *msg) {
  luaxX_lexerror(ls, msg, ls->t.token);
}


TStringx *luaxX_newstring (luaxLexState *ls, const char *str, size_t l) {
  COL_FUNCTION(luaxX_newstring);
  luax_State *L = ls->L;
  TStringx *ts = luaxS_newlstr(L, str, l);
  TXValue *o = luaxH_setstr(L, ls->fs->h, ts);  /* entry for `str' */
  if (ttisnil(o))
    setbvalue(o, 1);  /* make sure `str' will not be collected */
  return ts;
}


static void inclinenumber (luaxLexState *ls) {
  COL_FUNCTION(inclinenumber);
  COL_VAR(ls->linenumber);
  int old = ls->current;
  lua_assert(currIsNewline(ls));
  next(ls);  /* skip `\n' or `\r' */
  if (currIsNewline(ls) && ls->current != old)
    next(ls);  /* skip `\n\r' or `\r\n' */
  if (++ls->linenumber >= MAX_INT)
    luaxX_syntaxerror(ls, "chunk has too many lines");
  COL_VAR3(old, ls->current, ls->linenumber);
}


void luaxX_setinput (luax_State *L, luaxLexState *ls, ZIOx *z, TStringx *source) {
  COL_FUNCTION(luaxX_setinput);
  COL_VAR(source);
  ls->decpoint = '.';
  ls->L = L;
  ls->lookahead.token = TKX_EOS;  /* no look-ahead token */
  ls->z = z;
  ls->fs = NULL;
  ls->linenumber = 1;
  ls->lastline = 1;
  ls->source = source;
  luaxZ_resizebuffer(ls->L, ls->buff, LUAX_MINBUFFER);  /* initialize buffer */
  next(ls);  /* read first char */
}



/*
** =======================================================
** LEXICAL ANALYZER
** =======================================================
*/



static int check_next (luaxLexState *ls, const char *set) {
  COL_FUNCTION(check_next);
  COL_VAR((char)ls->current);
  if (!strchr(set, ls->current)){
    COL_TRC((char)ls->current << " not in set " << set << ", so return false.");
    return 0;
  }
  COL_TRC((char)ls->current << " is in set " << set << ", so return true.");
  save_and_next(ls);   
  return 1;
}


static void buffreplace (luaxLexState *ls, char from, char to) {
  COL_FUNCTION(buffreplace);
  COL_VAR2(from, to);
  size_t n = luaxZ_bufflen(ls->buff);
  char *p = luaxZ_buffer(ls->buff);
  while (n--)
    if (p[n] == from) p[n] = to;
}


static void trydecpoint (luaxLexState *ls, xSemInfo *seminfo) {
  COL_FUNCTION(trydecpoint);
  /* format error: try to update decimal point separator */
  struct lconv *cv = localeconv();
  char old = ls->decpoint;
  ls->decpoint = (cv ? cv->decimal_point[0] : '.');
  buffreplace(ls, old, ls->decpoint);  /* try updated decimal separator */
  if (!luaxO_str2d(luaxZ_buffer(ls->buff), &seminfo->r)) {
    /* format error with correct decimal point: no more options */
    buffreplace(ls, ls->decpoint, '.');  /* undo change (for error message) */
    luaxX_lexerror(ls, "malformed number", TKX_NUMBER);
  }
}


/* LUAX_NUMBER */
static void read_numeral (luaxLexState *ls, xSemInfo *seminfo) {
  COL_FUNCTION(read_numeral); 
  lua_assert(isdigit(ls->current));
  do {
    save_and_next(ls);
    COL_VAR(ls->current);
  } while (isdigit(ls->current) || ls->current == '.');
  if (check_next(ls, "Ee"))  /* `E'? */
    check_next(ls, "+-");  /* optional exponent sign */
  while (isalnum(ls->current) || ls->current == '_')
  {
    COL_VAR(ls->current);
    save_and_next(ls);
  }
  save(ls, '\0');
  buffreplace(ls, '.', ls->decpoint);  /* follow locale for decimal point */
  if (!luaxO_str2d(luaxZ_buffer(ls->buff), &seminfo->r))  /* format error? */
    trydecpoint(ls, seminfo); /* try to update decimal point separator */
}


static int skip_sep (luaxLexState *ls) {
  COL_FUNCTION(skip_sep);
  int count = 0;
  int s = ls->current;
  lua_assert(s == '[' || s == ']');
  save_and_next(ls);
  while (ls->current == '=') {
    save_and_next(ls);
    count++;
  }
  COL_TRC("Found " << count << " = " << " characters.");
  return (ls->current == s) ? count : (-count) - 1;
}


static void read_long_string (luaxLexState *ls, xSemInfo *seminfo, int sep) {
  COL_FUNCTION(read_long_string);
  COL_VAR((char)sep);
  int cont = 0;
  (void)(cont);  /* avoid warnings when `cont' is not used */
  save_and_next(ls);  /* skip 2nd `[' */
  if (currIsNewline(ls))  /* string starts with a newline? */
    inclinenumber(ls);  /* skip it */
  for (;;) {
    switch (ls->current) {
      case EOZ:
        luaxX_lexerror(ls, (seminfo) ? "unfinished long string" :
                                   "unfinished long comment", TKX_EOS);
        break;  /* to avoid warnings */
#if defined(LUAX_COMPAT_LSTR)
      case '[': {
        if (skip_sep(ls) == sep) {
          save_and_next(ls);  /* skip 2nd `[' */
          cont++;
#if LUAX_COMPAT_LSTR == 1
          if (sep == 0)
            luaxX_lexerror(ls, "nesting of [[...]] is deprecated", '[');
#endif
        }
        break;
      }
#endif
      case ']': {
        if (skip_sep(ls) == sep) {
          save_and_next(ls);  /* skip 2nd `]' */
#if defined(LUAX_COMPAT_LSTR) && LUAX_COMPAT_LSTR == 2
          cont--;
          if (sep == 0 && cont >= 0) break;
#endif
          goto endloop;
        }
        break;
      }
      case '\n':
      case '\r': {
        save(ls, '\n');
        inclinenumber(ls);
        if (!seminfo) luaxZ_resetbuffer(ls->buff);  /* avoid wasting space */
        break;
      }
      default: {
        if (seminfo) save_and_next(ls);
        else next(ls);
      }
    }
  } endloop:
  if (seminfo)
    seminfo->ts = luaxX_newstring(ls, luaxZ_buffer(ls->buff) + (2 + sep),
                                     luaxZ_bufflen(ls->buff) - 2*(2 + sep));
}


static void read_string (luaxLexState *ls, int del, xSemInfo *seminfo) {
  COL_FUNCTION(read_string);
  COL_VAR2((char)del, seminfo);

  save_and_next(ls);
  while (ls->current != del) {
    switch (ls->current) {
      case EOZ:
        luaxX_lexerror(ls, "unfinished string", TKX_EOS);
        continue;  /* to avoid warnings */
      case '\n':
      case '\r':
        luaxX_lexerror(ls, "unfinished string", TKX_STRING);
        continue;  /* to avoid warnings */
      case '\\': {
        int c;
        next(ls);  /* do not save the `\' */
        switch (ls->current) {
          case 'a': c = '\a'; break;
          case 'b': c = '\b'; break;
          case 'f': c = '\f'; break;
          case 'n': c = '\n'; break;
          case 'r': c = '\r'; break;
          case 't': c = '\t'; break;
          case 'v': c = '\v'; break;
          case '\n':  /* go through */
          case '\r': save(ls, '\n'); inclinenumber(ls); continue;
          case EOZ: continue;  /* will raise an error next loop */
          default: {
            if (!isdigit(ls->current))
              save_and_next(ls);  /* handles \\, \", \', and \? */
            else {  /* \xxx */
              int i = 0;
              c = 0;
              do {
                c = 10*c + (ls->current-'0');
                next(ls);
              } while (++i<3 && isdigit(ls->current));
              if (c > UCHAR_MAX)
                luaxX_lexerror(ls, "escape sequence too large", TKX_STRING);
              save(ls, c);
            }
            continue;
          }
        }
        save(ls, c);
        next(ls);
        continue;
      }
      default:
        save_and_next(ls);
    }
  }
  save_and_next(ls);  /* skip delimiter */
  seminfo->ts = luaxX_newstring(ls, luaxZ_buffer(ls->buff) + 1,
                                   luaxZ_bufflen(ls->buff) - 2);

  COL_VAR(seminfo->ts);
}


static int llex (luaxLexState *ls, xSemInfo *seminfo) {
  COL_FUNCTION(llex);
  luaxZ_resetbuffer(ls->buff);
  COL_VAR(ls);
  for (;;) {
    switch (ls->current) {
      case '\n':
      case '\r': {
        inclinenumber(ls);
        continue;
      }
      case '-': {
        next(ls);
        if (ls->current != '-') return '-';
        COL_TRC("Got comment.");
        /* else is a comment */
        next(ls);
        if (ls->current == '[') {
          int sep = skip_sep(ls);
          luaxZ_resetbuffer(ls->buff);  /* `skip_sep' may dirty the buffer */
          if (sep >= 0) {
            read_long_string(ls, NULL, sep);  /* long comment */
            luaxZ_resetbuffer(ls->buff);
            continue;
          }
        }
        /* else short comment */
        while (!currIsNewline(ls) && ls->current != EOZ)
          next(ls);
        COL_TRC("End of comment.");
        continue;
      }
      case '[': {
        int sep = skip_sep(ls);
        if (sep >= 0) {
          read_long_string(ls, seminfo, sep);
          return TKX_STRING;
        }
        else if (sep == -1) return '[';
        else luaxX_lexerror(ls, "invalid long string delimiter", TKX_STRING);
      }
      case '=': {
        next(ls);
        if (ls->current != '=') return '=';
        else { next(ls); COL_TRC("== token"); return TKX_EQ;}
      }
      case '<': {
        next(ls);
        if (ls->current != '=') { COL_TRC("< token"); return '<'; }
        else { next(ls); return TKX_LE; }
      }
      case '>': {
        next(ls);
        if (ls->current != '=') { COL_TRC("> token"); return '>'; }
        else { next(ls); COL_TRC(">= token"); return TKX_GE; }
      }
      case '~': {
        next(ls);
        if (ls->current != '=') { COL_TRC("~ token"); return '~'; }
        else { next(ls); return TKX_NE; }
      }
      case '"':
      case '\'': {
        read_string(ls, ls->current, seminfo);
        COL_TRC("String token");
        return TKX_STRING;
      }
      case '.': {
        save_and_next(ls);
        if (check_next(ls, ".")) {
           if (check_next(ls, ".")) { COL_TRC("... token"); return TKX_DOTS;}   /* ... */
           else { COL_TRC(".. token"); return TKX_CONCAT; }   /* .. */
        }
        else if (!isdigit(ls->current)) { COL_TRC(". token"); return '.';}
        else {
          read_numeral(ls, seminfo);
          COL_TRC("Number token.");
          return TKX_NUMBER;
        }
      }
      case EOZ: {
        COL_TRC("EOZ token.");
        return TKX_EOS;
      }
      default: {
        if (isspace(ls->current)) {
          lua_assert(!currIsNewline(ls));
          next(ls);
          continue;
        }
        else if (isdigit(ls->current)) {
          read_numeral(ls, seminfo);
          COL_TRC("Number token = " << seminfo->r);
          return TKX_NUMBER;
        }
        else if (isalpha(ls->current) || ls->current == '_') {
          /* identifier or reserved word */
          TStringx *ts;
          do {
            save_and_next(ls);
          } while (isalnum(ls->current) || ls->current == '_');
          ts = luaxX_newstring(ls, luaxZ_buffer(ls->buff),
                                  luaxZ_bufflen(ls->buff));
          if (ts->tsv.reserved > 0)  /* reserved word? */
          {
            COL_TRC("Reserved word token '" << luaxX_tokens[ts->tsv.reserved-1] << '\'');
            return ts->tsv.reserved - 1 + FIRST_xRESERVED;
          }
          else {
            seminfo->ts = ts;
            COL_TRC("Name token = '" << seminfo->ts << '\'');
            return TKX_NAME;
          }
        }
        else {
          int c = ls->current;
          next(ls);
          COL_TRC((char)c << " token");
          return c;  /* single-char tokens (+ - / ...) */
        }
      }
    }
  }
}


void luaxX_next (luaxLexState *ls) {
  COL_FUNCTION(luaxX_next);
  ls->lastline = ls->linenumber;
  if (ls->lookahead.token != TKX_EOS) {  /* is there a look-ahead token? */
    ls->t = ls->lookahead;  /* use this one */
    ls->lookahead.token = TKX_EOS;  /* and discharge it */
  }
  else
    ls->t.token = llex(ls, &ls->t.seminfo);  /* read next token */
}


void luaxX_lookahead (luaxLexState *ls) {
  COL_FUNCTION(luaxX_lookahead);
  lua_assert(ls->lookahead.token == TKX_EOS);
  ls->lookahead.token = llex(ls, &ls->lookahead.seminfo);
}

