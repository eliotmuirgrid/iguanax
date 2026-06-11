//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testGlobMatch.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   5/31/23 10:10 AM
//  ---------------------------------------------------------------------------
#include <COL2example/testGlobMatch.h>

#include <UNIT/UNITapp.h>

#include <COL/COLstring.h>
#include <COL/COLutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testGlobs() {
   UNIT_ASSERT(COLglobMatch("fred", "fred"));
   UNIT_ASSERT(COLglobMatch("fred barney", "fred"));
   UNIT_ASSERT(COLglobMatch("fred barney", "barney"));
   UNIT_ASSERT(COLglobMatch("fred* barney*", "barney"));
   UNIT_ASSERT(COLglobMatch("fred* barney*", "fred"));
   UNIT_ASSERT(COLglobMatch("*", "fred"));
   UNIT_ASSERT(COLglobMatch("**", "fred"));
   UNIT_ASSERT(COLglobMatch("abc *** def", "fred"));
   UNIT_ASSERT(COLglobMatch("abc ***d def", "fred"));
   UNIT_ASSERT(!COLglobMatch("abc ***d def -fred", "fred"));
   UNIT_ASSERT(!COLglobMatch("abc ***d def -f*d xyz", "fred"));
   UNIT_ASSERT(!COLglobMatch("abc **d def -f*d xyz", "fred"));
   UNIT_ASSERT(COLglobMatch("abc ***d def -f*d xyz *d", "fred"));
   UNIT_ASSERT(COLglobMatch("f* b*", "barney"));
   UNIT_ASSERT(COLglobMatch("fred*", "fred"));
   UNIT_ASSERT(COLglobMatch("f*", "fred"));
   UNIT_ASSERT(!COLglobMatch("* -fred", "fred"));
   UNIT_ASSERT(COLglobMatch("* -barney", "fred"));
   UNIT_ASSERT(COLglobMatch("abc *dy -def xyz -fredd", "freddy"));
   UNIT_ASSERT(COLglobMatch("abc **dy -def xyz -fredd", "freddy"));
   UNIT_ASSERT(COLglobMatch("abc ***dy -def xyz -fredd", "freddy"));
   UNIT_ASSERT(!COLglobMatch("abc ***dy -def xyz -freddy", "freddy"));
   UNIT_ASSERT(!COLglobMatch("abc ***dy -def xyz -freddy fredd", "freddy"));
   UNIT_ASSERT(COLglobMatch("abc *ddy -def xyz -fredd", "freddy"));
   UNIT_ASSERT(COLglobMatch("ab*abc*def -def xyz", "abcabcdef"));
   UNIT_ASSERT(COLglobMatch("ab*abc*def -def xyz", "abcabc2def"));
   UNIT_ASSERT(COLglobMatch("ab*abc*def abc def", "abdefabcdedededef"));
   UNIT_ASSERT(COLglobMatch("ab*abc*def* abc def", "abdefabcdedededefk"));
   UNIT_ASSERT(COLglobMatch("abc *r*??* -def xyz -fredd", "freddy"));
   UNIT_ASSERT(COLglobMatch("abc *r*?? -def xyz -fredd", "freddy"));
   UNIT_ASSERT(COLglobMatch("*Abc", "AbxAbc"));
   UNIT_ASSERT(COLglobMatch("*Abc*b*", "AbcAbc"));
   UNIT_ASSERT(COLglobMatch("?*Abc", "AbcAbc"));
   UNIT_ASSERT(COLglobMatch("*Abc", "AbcAbc"));
   UNIT_ASSERT(COLglobMatch(" *Abc ", "AbcAbc"));
   UNIT_ASSERT(COLglobMatch("abc f*r*?? -def xyz -fredd", "freddy"));
   UNIT_ASSERT(COLglobMatch("abc ?r*d?? -def xyz -fredd", "freddy"));
   UNIT_ASSERT(COLglobMatch("abc ?r*d? -def xyz -fredd", "freddy"));
   UNIT_ASSERT(!COLglobMatch("*???", "12"));
   UNIT_ASSERT(COLglobMatch("*???", "123"));
   UNIT_ASSERT(COLglobMatch(" *??? ", "123"));
   UNIT_ASSERT(COLglobMatch("*???", "1234"));
   UNIT_ASSERT(COLglobMatch(" *??? ", "1234"));
   UNIT_ASSERT(COLglobMatch("?*3??", "1333"));
   UNIT_ASSERT(COLglobMatch("?*?3?", "1333"));
   UNIT_ASSERT(COLglobMatch("?*??3", "1333"));
   UNIT_ASSERT(!COLglobMatch("?*??3", "333"));
   UNIT_ASSERT(COLglobMatch("abc **d def", "fred"));
   UNIT_ASSERT(COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "ABc") == true);
   UNIT_ASSERT(COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "CAB") == true);
   UNIT_ASSERT(COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "ABCdef") == false);
   UNIT_ASSERT(COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "ABCde") == true);
   UNIT_ASSERT(COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "CARP") == false);
   UNIT_ASSERT(COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "CAT") == false);
   UNIT_ASSERT(COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "CATS") == true);
   UNIT_ASSERT(COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "BIRD") == true);
   UNIT_ASSERT(COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "BIRDs") == false);
   UNIT_ASSERT(COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "CART") == false);
   UNIT_ASSERT(!COLglobMatch("", ""));
   UNIT_ASSERT(!COLglobMatch(" -FOO* ", "FOO"));
   UNIT_ASSERT(!COLglobMatch("  ", ""));
   UNIT_ASSERT(!COLglobMatch("", ""));
   UNIT_ASSERT(!COLglobMatch("-", ""));
   UNIT_ASSERT(!COLglobMatch("  -  - -", "abc"));
   UNIT_ASSERT(!COLglobMatch("-", "abc"));
   UNIT_ASSERT(!COLglobMatch("a b c", "  "));
   UNIT_ASSERT(!COLglobMatch("-*", "abc"));
   UNIT_ASSERT(!COLglobMatch(" * -* ", "abc"));
   UNIT_ASSERT(COLglobMatch(" * -* *", "abc"));
   UNIT_ASSERT(COLglobMatch("*", "abc"));
   UNIT_ASSERT(COLglobMatch("?*", "abc"));
   UNIT_ASSERT(COLglobMatch("??*", "abc"));
   UNIT_ASSERT(COLglobMatch("???*", "abc"));
   UNIT_ASSERT(COLglobMatch("*???*", "abc"));
   UNIT_ASSERT(!COLglobMatch("*????*", "abc"));
   UNIT_ASSERT(!COLglobMatch("??", "abc"));
   UNIT_ASSERT(COLglobMatch("???", "abc"));
   UNIT_ASSERT(!COLglobMatch("????", "abc"));
   UNIT_ASSERT(!COLglobMatch("?? ", "abc"));
   UNIT_ASSERT(COLglobMatch("??? ", "abc"));
   UNIT_ASSERT(!COLglobMatch("???? ", "abc"));
   UNIT_ASSERT(!COLglobMatch("???*? ", "abc"));
   UNIT_ASSERT(!COLglobMatch("????*c", "abc"));
   UNIT_ASSERT(COLglobMatch("*", ""));
   UNIT_ASSERT(COLglobMatch("* ", ""));
   UNIT_ASSERT(!COLglobMatch("*c", ""));
   UNIT_ASSERT(COLglobMatch("*c", "c"));
   UNIT_ASSERT(!COLglobMatch("??*c", "a"));
   UNIT_ASSERT(!COLglobMatch("??*c", "a"));
   UNIT_ASSERT(!COLglobMatch("???*c", "ab"));
   UNIT_ASSERT(!COLglobMatch("????*c", "abc"));
   UNIT_ASSERT(!COLglobMatch("????*", "abc"));
   UNIT_ASSERT(COLglobMatch("* ", "abc"));
   UNIT_ASSERT(COLglobMatch("?* ", "abc"));
   UNIT_ASSERT(COLglobMatch("??* ", "abc"));
   UNIT_ASSERT(COLglobMatch("???* ", "abc"));
   UNIT_ASSERT(COLglobMatch("*???* ", "abc"));
   UNIT_ASSERT(!COLglobMatch("*????* ", "abc"));
   UNIT_ASSERT(!COLglobMatch("?? ", "abc"));
   UNIT_ASSERT(COLglobMatch("??? ", "abc"));
   UNIT_ASSERT(!COLglobMatch("???? ", "abc"));
   UNIT_ASSERT(!COLglobMatch("??", "abc"));
   UNIT_ASSERT(COLglobMatch("???", "abc"));
   UNIT_ASSERT(!COLglobMatch("????  ", "abc"));
   UNIT_ASSERT(!COLglobMatch("???*?", "abc"));
   UNIT_ASSERT(!COLglobMatch("????*c ", "abc"));
   UNIT_ASSERT(COLglobMatch("* ", ""));
   UNIT_ASSERT(COLglobMatch("*", ""));
   UNIT_ASSERT(!COLglobMatch("*c ", ""));
   UNIT_ASSERT(COLglobMatch("*c ", "c"));
   UNIT_ASSERT(!COLglobMatch("??*c ", "a"));
   UNIT_ASSERT(!COLglobMatch("??*c ", "a"));
   UNIT_ASSERT(!COLglobMatch("???*c ", "ab"));
   UNIT_ASSERT(!COLglobMatch("????*c ", "abc"));
   UNIT_ASSERT(!COLglobMatch("????* ", "abc"));
   UNIT_ASSERT(!COLglobMatch("???*?", "abc"));
   UNIT_ASSERT(!COLglobMatch("???*? ", "abc"));
   UNIT_ASSERT(COLglobMatch("test/pattern?with?spaces", "test/pattern with spaces"));
   UNIT_ASSERT(COLglobMatch("test/pattern?with?spaces regular/test", "test/pattern with spaces"));
   UNIT_ASSERT(COLglobMatch("*Test?with?space* *regular*", "test/[Test with spaces]"));
   UNIT_ASSERT(COLglobMatch("*Test?with?space* *regular*", "test/[some Test with spaces 1]"));
   UNIT_ASSERT(COLglobMatch("*Test?with?space* *regular*", "test/[another Test with spaces 2]"));
   UNIT_ASSERT(!COLglobMatch("*Test?with?space* *regular*", "test/[No match with spaces 2]"));
   UNIT_ASSERT(COLglobMatch("*Test?with?space* *regular*", "regular/test"));
}

void testGlobMatch(UNITapp* pApp) {
   pApp->add("test/globs", &testGlobs);
}
