#ifndef __REX_FIND_H__
#define __REX_FIND_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: REXfind
//
// Description:
//
// REXfind - new simpler wrapper around pcre regex library.
//
// Author: Eliot Muir
// Date:   Friday, December 14th, 2012 @ 11:00:20 PM
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class REXresult{
public:
   REXresult(const char* ipData, int iSize) : pData(ipData), Size(iSize) {} 
   const char* pData;
   int Size;
};

COLostream& operator<<(COLostream& Stream, const REXresult& Result);

class REXfind{
public:
   // REXoptions wrap around the PCRE options, so files using REX don't need to refer to PCRE options
   enum REXoption{
      REX_DEFAULT = 0,
      REX_CASE_INSENSITIVE = 0x00000001, /* PCRE_CASELESS */
      REX_NOTEMPTY = 0x00000400, /* PCRE_NOTEMPTY */
      REX_DOT_ALL = 0x00000004, /* PCRE_DOTALL */
      REX_MULTILINE = 0x00000002, /* PCRE_MULTILINE */
      REX_UNGREEDY = 0x00000200, /* PCRE_UNGREEDY */
      REX_ANCHORED = 0x00000010, /* PCRE_ANCHORED */
      REX_UNICODE = 0x00000800, /* PCRE_UTF8/16/32 */
      REX_ALLOW_DUPLICATE_NAMES = 0x00080000, /* PCRE_DUPNAMES */
      REX_IGNORE_COMMENTS = 0x00000008, /* PCRE_EXTENDED */
      // this is a weird one - gives an error if a character following a backslash has no extra meaning
      // can be useful for writing forward-compatible code, so users are given explicit errors on these implicit \x sequences
      REX_EXTRA = 0x00000040, /* PCRE_EXTRA */
   };
   REXfind();
   ~REXfind();

   // Throws if the expression fails to compile
   void init(const char* pExpression, int Options=REX_DEFAULT);

   void matchBegin(const char* pSubject, int Size);   // Call this to begin a search
   bool matchNext();                                  // Call until it returns false to find matches

   // The following API can be used to access the values from captures in the regex using the pascal string REXresult.
   // The integers passed into these functions should correspond to the tokens normally used in substitution patterns
   // for accessing captures, e.g. $1, $2, etc.
   REXresult match(int i) const;   
   REXresult unmatched() const;
   bool matchPresent(int i) const;

   // Returns the current position in the subject string, which is the index of the first character past the current match.
   int position() const { return m_Position; }

   int numMatchGroups() const;
private:
   void* m_pHandle;
   void* m_pStudyHandle;
   int* m_pSubstrings;
   int m_SubStringSize;
   int m_Position;
   const char* m_pSubject;
   int m_SubjectSize;
   int m_Options;
   int m_NumMatchGroups;

   REXresult m_Unmatched;

   REXfind(const REXfind& Orig); // not allowed
   REXfind& operator=(const REXfind& Orig); // not allowed
};

#endif // end of defensive include
