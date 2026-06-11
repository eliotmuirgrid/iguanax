#ifndef __REX_MATCHER_H__
#define __REX_MATCHER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: REXmatcher
//
// Description:
//
// Simple C++ wrapper class over regular expression matching C library.
//
// Author: Eliot Muir
// Date:   Tue 08/17/1999 
//
//---------------------------------------------------------------------------

class REXmatcherPrivate;
class COLstring;

#include <COL/COLminimumInclude.h>

class REXmatcher
{
public:

   enum REXoption
   {
      REXoptionDefault = 0,
      CaseInsensitive = 0x0001,
      NotEmpty = 0x0400,
      DotAll = 0x0004  // Uses PCRE_DOTALL option: "If this modifier is set, a
                       // dot metacharacter in the pattern matches all
                       // characters, including newlines. Without it, newlines
                       // are excluded."
   };

   /**
    * Default constructor
    */
   REXmatcher();

   /**
    * Creates a new REXmatcher and compiles the
    * specified regular expression (Pattern).  Does not
    * throw exceptions.  Applications will need to check
    * isExpressionValid to see if the regex compiled or not.
    */
   REXmatcher(const COLstring& Pattern, int options = REXoptionDefault);

   REXmatcher(const REXmatcher& Orig);
   REXmatcher& operator=(const REXmatcher& Orig);
   /**
    * Default destructor
    */
   virtual ~REXmatcher();

   /**
    * Initializes the REXmatcher for matching the 
    * specified Pattern.  If the expression is invalid, the error message is returned
    * otherwise an empty string is returned.  Does not throw exceptions
    */
   COLstring init(const COLstring& Pattern, int options = REXoptionDefault);
   
   const COLstring& pattern() const;

   // Returns true if the regular expression was initialized correctly.
   bool isExpressionValid() const;

   // Returns the last compile error if any.
   const COLstring& lastCompileError() const;

   // This is the location of the compile error
   COLuint32 errorOffset() const;

   /**
    * Returns true if the specified pString matches
    * the REXmatcher's current regular expression,
    * false otherwise.
    */
   virtual bool doesMatch(const char* pString) const;

   // This is more efficient to call than doesMatch if you know the string length since
   // it avoids a call to strlen - note this is a deliberate choice not make this an overloaded method
   bool doesMatchWithLength(const char* pString, COLuint32 Length) const;

   /**
    * Returns true if the specified pString matches
    * the REXmatcher's current regular expression,
    * false otherwise.  StartIndex indicates the postion
    * in pString to start the regex search.  
    * If a match is found StartOfPattern and EndOfPattern
    * are set to the start index and the end index of the matching
    * substring respectively.  If no match is found, they are set
    * to npos.
    */
   bool findMatch(const char* pString,
                        COLindex StartIndex,
                        COLindex& StartOfPattern, 
                        COLindex& EndOfPattern) const;

   // This is more efficient to call than findMatch if you know the string length since
   // it avoids a call to strlen.  Alos, if pString contains embedded null characters,
   // this is helpful.
   bool findMatchWithLength(const char* pString,
                                  COLuint32 Length,
                                  COLindex StartIndex,
                                  COLindex& StartOfPattern,
                                  COLindex& EndOfPattern) const;

   /**
    * Performs a PERL style regular expression search and
    * replace on the specified SubjectString. e.g.:
    *
    * InOutSubject =~ s/Pattern/Replacement/
    *
    * where Pattern is the REXmatcher's current regular expression
    * and Replacement is the specified ReplacementPattern.
    * ReplacementPattern may contain regex back references (e.g. $1).
    *
    * pFilter, if not NULL, is applied to all unmatched areas of
    * InOutSubject as well as captured areas before replacement.
    *
    * regexReplace returns the number of substitutions performed.
    *
    * This can throw exceptions.
    */
   COLuint32 regexReplace(COLstring& InOutSubject, 
                          const COLstring& ReplacementPattern,
                          COLstring (*pFilter)(const COLstring&) = NULL,
                          COLuint32 MaxReplacements = 0) const;

   // Useful if you can't, or don't want to modify the subject string.
   COLuint32 regexReplace(COLstring& Output,
                          const COLstring& Subject, 
                          const COLstring& ReplacementPattern,
                          COLstring (*pFilter)(const COLstring&) = NULL) const;

   // Performs a PERL-style search/replace as above, except that
   // the replacement pattern is obtained per incident by calling
   // pReplace() with the position and length of the current match
   // and the supplied context.  Although the C-string passed to
   // this function is NUL terminated, it will probably be longer
   // than you expect; always consider the length indicated.
   //
   COLuint32 functionalReplace
   (
      COLstring& InOutSubject,
      COLstring (*pReplace)(const char*, size_t, void*),
      COLstring (*pFilter)(const COLstring&) = NULL,
      void* pContext = NULL,  // The context passed to pReplace.
      COLuint32 MaxReplacements = 0
   ) const;

   // Useful if you can't, or don't want to modify the subject string.
   COLuint32 functionalReplace
   (
      COLstring& Output,
      const COLstring& Subject,
      COLstring (*pReplace)(const char*, size_t, void*),
      COLstring (*pFilter)(const COLstring&) = NULL,
      void* pContext = NULL,  // The context passed to pReplace.
      COLuint32 MaxReplacements = 0
   ) const;

private:
   REXmatcherPrivate* pMember;
};

int operator==(const REXmatcher& Lhs, const REXmatcher& Rhs);
int operator!=(const REXmatcher& Lhs, const REXmatcher& Rhs);
COLostream& operator<<(COLostream& Stream, const REXmatcher& Matcher);

#endif // end of defensive include
