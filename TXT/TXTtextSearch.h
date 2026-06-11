#ifndef __TXT_TEXT_SEARCH_H__
#define __TXT_TEXT_SEARCH_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TXTtextSearch
//
// Description:
//
// A text searching class that supports regex and boolean expressions.
// The grammar used is similar to the one Google uses for their search
// engine, except that pipes (|) cannot be used in place of "OR".  This
// was done considering how common pipes are in HL7.
//
// Query ::= Conjunction
//         | Conjunction 'OR' Query
//
// Conjunction ::= Expr
//               | Expr Conjunction
//
// Expr ::= ''              (empty, always succeeds)
//        | '-' Expr        (no space is necessary)
//        | 'NOT' Expr
//        | '/' ... '/'     (a pcre regex*)
//        | '"' ... '"'     (a quoted phrase*)
//        | [^ \t"()]+      (a word)
//        | '(' Query ')'
//
// * Slashes (/) and double-quotes (") may occur inside regexes or quoted
//   phrases, respectively, as long as they are preceeded by a back-
//   slashes (\)--for phrases, the number of back-slashes must be odd.
//
// Keywords are case sensitive lose their magic with other special
// characters inside double-quotes.  E.g., OR is a keyword, but "OR" and
// 'or' are not.  Inside quotes, backslashes can be used to escape
// double-quotes, backslashes and anything else, but they do not create
// special characters (E.g., "\n" will match 'n', not newlines)--regexes
// can be used to do that.
//
//
// Author: Eric Mulvaney
// Date:   Wednesday, April 15th, 2009 @ 09:19:27 AM
//
//---------------------------------------------------------------------------

#include <REX/REXmatcher.h>

class COLstring;
template <class T> class COLarray;


struct TXTtextSearchItem;

class TXTtextSearch
{
public:
   TXTtextSearch();
   ~TXTtextSearch();

   TXTtextSearch(const TXTtextSearch&);
   TXTtextSearch& operator = (const TXTtextSearch&);

   void init(const COLstring& Query);

   bool matches(const char*) const;

   REXmatcher matchedAreaRegex() const;

   // Note that the pointers returned here will be invalidated
   // if you call init() again.
   //
   void getInvalidRegexes(COLarray<const REXmatcher*>&) const;

   // Returns true only if this text search will match all entries.
   // Note that this may return false even if the query will match all
   // strings.  For example, if the query is "/.*/", it will match all
   // strings, but matchesAll() will still return false.  But it will
   // never return true if the query does not match all strings.
   //
   bool matchesAll() const;


   // returns false if could not make an equivalent
   // sqlite extended syntax MATCH query, usually
   // when the txt searcher has a regular expression
   //
   // False is also returned if any term is less than the MinimumTermSize
   //
   bool toSqliteMatch(COLstring& Out, COLuint32 MinimumTermSize) const;

private:

   COLarray<TXTtextSearchItem>* pSearchItems;
};


#endif // end of defensive include
