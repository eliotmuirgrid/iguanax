//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: REXmatcher
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tue 08/17/1999
//
//---------------------------------------------------------------------------

// Note that we are not using precompiled headers.  This might be something
// that should be changed.... on the other hand we only have one cpp file in
// this library.  So at this stage it may not be worth changing over.

#include "REXmatcher.h"
#include "REXoptionMasks.h"

#include <COL/COLerror.h>
#include <COL/COLstring.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <ctype.h>
#include <memory.h>
#include <string.h>

#include <PCRE/pcre.h>

#ifdef REX_DEBUG
#  define REX_LOG(Message)       { COLstring RexLog; COLostream Stream(RexLog); Stream << __FILE__ << ':' << __LINE__ << ' ' << Message << ::newline; COLcout << RexLog; }
#else
#  define REX_LOG(Message)
#endif

#define COMPILE_MASK (PUBLIC_COMPILE_OPTIONS)
#define EXECUTE_MASK (PUBLIC_EXEC_OPTIONS)
#define STUDY_MASK   (PUBLIC_STUDY_OPTIONS)

class REXmatcherPrivate
{
public:
   REXmatcherPrivate()
   :  pCompiledExpression(0),
      pStudiedExpression(0),
      ErrorOffset(0),
      StoredOptions(REXmatcher::REXoptionDefault),
      ValidExpression(false)
   {}
   ~REXmatcherPrivate();

   pcre*  pCompiledExpression;
   pcre_extra* pStudiedExpression;

   COLstring StoredPattern;
   COLstring LastCompileError;
   COLuint32 ErrorOffset;

   int StoredOptions;

   bool ValidExpression;

   void freeCompiledExpression();

   COLstring expandBackReferences(const COLstring& ReplacementPattern,
                                  const COLstring& SubjectString,
                                  int* Offsets,
                                  int CountOfSubstring,
                                  COLstring (*pFilter)(const COLstring&)) const;
private:
   REXmatcherPrivate(const REXmatcherPrivate&);
   REXmatcherPrivate& operator=(const REXmatcherPrivate&);
};

const char REX_MATCHER_BACK_REFERENCE_CHAR = '$';

// There's no point in growing the offset array when
// the maximum size we allow is so small.
//
const int REX_MATCHER_OFFSETS_SIZE = 255;  // (3*85) This is supposed to be a multiple of three.

REXmatcherPrivate::~REXmatcherPrivate()
{
   // freeCompiledExpression throws no C++ exceptions so we're good.
   freeCompiledExpression();
}

void REXmatcherPrivate::freeCompiledExpression()
{
   if (pCompiledExpression != NULL)
   {
      // no return value so we cannot check for an error
      pcre_free_rex(pCompiledExpression);
      pCompiledExpression = NULL;
   }
   if (pStudiedExpression != NULL)
   {
      // no return value so we cannot check for an error
      pcre_free_rex(pStudiedExpression);
      pStudiedExpression = NULL;
   }
}

REXmatcher::REXmatcher()
{
   pMember = new REXmatcherPrivate();
}

REXmatcher::REXmatcher(const REXmatcher& Orig)
{
   pMember = new REXmatcherPrivate();
   init(Orig.pMember->StoredPattern.c_str(), Orig.pMember->StoredOptions);
}

REXmatcher& REXmatcher::operator=(const REXmatcher& Orig)
{
   if (this != &Orig)
   {
      delete pMember;
      pMember = new REXmatcherPrivate();
      init(Orig.pMember->StoredPattern, Orig.pMember->StoredOptions);
   }
   return *this;
}

REXmatcher::REXmatcher
(
   const COLstring& Pattern,
   int options
)
{
   pMember = new REXmatcherPrivate();
   init(Pattern, options);
}

REXmatcher::~REXmatcher()
{
   delete pMember;
}

COLstring REXmatcher::init
(
   const COLstring& Pattern,
   int options
)
{
   pMember->ValidExpression = false;
   pMember->LastCompileError.clear();
   pMember->StoredPattern = Pattern;
   pMember->StoredOptions = options;

   pMember->freeCompiledExpression();

   const char* pErrorMessage = NULL;

   // int used to be compatible with pcre api
   int ErrorOffset;

   pMember->pCompiledExpression = pcre_compile_rex(
              Pattern.c_str(),
              options & COMPILE_MASK,
              &pErrorMessage,
              &ErrorOffset,
              NULL);  // NULL == use default character tables

   if (pMember->pCompiledExpression == NULL)
   {
      COLPRECONDITION(pErrorMessage != NULL);
      pMember->LastCompileError = pErrorMessage;
      pMember->ErrorOffset = ErrorOffset;

      return  pMember->LastCompileError;
   }

   pMember->pStudiedExpression = pcre_study_rex(
             pMember->pCompiledExpression,
             options & STUDY_MASK, // default options
             &pErrorMessage);   // Error message returned here

   // may return NULL which would mean no extra info
   if (pErrorMessage != NULL)
   {
      // pcre_study failed
      pMember->freeCompiledExpression();
      pMember->LastCompileError = pErrorMessage;
      pMember->ErrorOffset = 0;

      return  pMember->LastCompileError;
   }
   pMember->ValidExpression = true;
   // success!
   return COLstring();
}

bool REXmatcher::isExpressionValid() const
{
   return pMember->ValidExpression;
}

// Returns the last compile error if any.
const COLstring& REXmatcher::lastCompileError() const
{
   return pMember->LastCompileError;
}

// This is the location of the compile error
COLuint32 REXmatcher::errorOffset() const
{
   return pMember->ErrorOffset;
}

const COLstring& REXmatcher::pattern() const
{
   return pMember->StoredPattern;
}

bool REXmatcher::doesMatch(const char* pData) const
{
   COLPRECONDITION(pMember->pCompiledExpression != NULL);
   COLPRECONDITION(pData != NULL);

   // using native types for compatibility with pcre api
   int ReturnCode;
   ReturnCode = pcre_exec_rex(pMember->pCompiledExpression,
                              pMember->pStudiedExpression,
                              pData,
                              strlen(pData),
                              0,  // we match from the start of the string
                              pMember->StoredOptions & EXECUTE_MASK,
                              NULL,
                              0);
   // if ReturnCode is -2 or less that indicates a bad option passed to the
   // pcre_exec or an internal pcre error, or we ran out of memory
   COLPOSTCONDITION(ReturnCode >= -1);

   return (ReturnCode >= 0);
}

// This is more efficient to call than doesMatch if you know the string length since
// it avoids a call to strlen
bool REXmatcher::doesMatchWithLength(const char* pData, COLuint32 Length) const
{
   COLPRECONDITION(pMember->pCompiledExpression != NULL);
   COLPRECONDITION(pData != NULL);

   // using native types for compatibility with pcre api
   int ReturnCode;
   ReturnCode = pcre_exec_rex(pMember->pCompiledExpression,
                              pMember->pStudiedExpression,
                              pData,
                              Length,
                              0,  // we match from the start of the string
                              pMember->StoredOptions & EXECUTE_MASK,
                              NULL,
                              0);
   // if ReturnCode is -2 or less that indicates a bad option passed to the
   // pcre_exec or an internal pcre error, or we ran out of memory
   COLPOSTCONDITION(ReturnCode >= -1);

   return (ReturnCode >= 0);
}

bool REXmatcher::findMatch
(
   const char* pString,
   COLindex StartIndex,
   COLindex& StartOfPattern,
   COLindex& EndOfPattern
) const
{
   return findMatchWithLength(pString, strlen(pString), StartIndex, StartOfPattern, EndOfPattern);
}

bool REXmatcher::findMatchWithLength
(
   const char* pString,
   COLuint32 Length,
   COLindex StartIndex,
   COLindex& StartOfPattern,
   COLindex& EndOfPattern
) const
{
   COL_METHOD(REXmatcher::findMatchWithLength);
   COLPRECONDITION(pMember->pCompiledExpression != NULL);
   COLPRECONDITION(pString != NULL);

   bool FoundMatch = false;
   // Using native types for pcre API
   int pOffsets[REX_MATCHER_OFFSETS_SIZE];

   int ReturnCode = pcre_exec_rex(pMember->pCompiledExpression,
                                  pMember->pStudiedExpression,
                                  pString,
                                  Length,
                                  StartIndex,
                                  pMember->StoredOptions & EXECUTE_MASK,
                                  pOffsets,
                                  REX_MATCHER_OFFSETS_SIZE);

   COL_VAR(ReturnCode);
   if (ReturnCode < -1) {
      COL_ERR("REXmatcher::findMatchWithLength pcre_exec returned " << ReturnCode <<
              ", String = " << COLstring(pString, Length) << ", Length = " << Length << ", StartIndex = " << StartIndex);
   }

   // if ReturnCode is -2 or less that indicates a bad option passed to the
   // pcre_exec or an internal pcre error, or we ran out of memory
   COLPOSTCONDITION(ReturnCode >= -1);

   if (ReturnCode >= 0)
   {
      StartOfPattern = pOffsets[0];
      EndOfPattern = pOffsets[1] - 1;
      FoundMatch = true;
   }
   else
   {
      StartOfPattern = npos;
      EndOfPattern = npos;
   }

   return FoundMatch;
}


static COLstring simpleReplace(const char*, size_t, void* pContext)
{
   return *(const COLstring*)pContext;
}

COLuint32 REXmatcher::regexReplace
(
   COLstring& SubjectString,
   const COLstring& ReplacementPattern,
   COLstring (*pFilter)(const COLstring&),
   COLuint32 MaxReplacements
) const
{
   return functionalReplace(SubjectString, simpleReplace, pFilter, (void*)&ReplacementPattern, MaxReplacements);
}

COLuint32 REXmatcher::regexReplace
(
   COLstring& Output,
   const COLstring& Subject,
   const COLstring& ReplacementPattern,
   COLstring (*pFilter)(const COLstring&)
) const
{
   return functionalReplace(Output, Subject, simpleReplace, pFilter, (void*)&ReplacementPattern);
}

COLuint32 REXmatcher::functionalReplace
(
   COLstring& InOutSubject,
   COLstring (*pReplace)(const char*, size_t, void*),
   COLstring (*pFilter)(const COLstring&),
   void* pContext,
   COLuint32 MaxReplacements
) const
{
   COLstring Output;
   COLuint32 Result = functionalReplace(Output, InOutSubject, pReplace, pFilter, pContext, MaxReplacements);
   Output.swap(&InOutSubject);
   return Result;
}

inline static void REXapplyFilter(COLstring& Output, COLstring (*pFilter)(const COLstring&),
                                  const COLstring& Subject, size_t From, size_t Size)
{
   COL_FUNCTION(REXapplyFilter);
   if(!pFilter) {
      COL_TRC("No filter");
      COL_VAR3(Subject, From, Size);
      Output.append(Subject.c_str() + From, Size);
   } else {
      char *pStart = (char*)Subject.c_str() + From;
      char *pLast = pStart + Size, Last = *pLast;
      try {
         *pLast = '\0';
         COL_VAR2(Size, pStart);
         COLstring Substr(pStart, Size);
         //COLconstString Substr(pStart, Size);
         Output.append(pFilter(Substr));
         *pLast = Last;
      } catch(...) {
         *pLast = Last;
         throw;
      }
   }
}

COLuint32 REXmatcher::functionalReplace
(
   COLstring& ResultString,
   const COLstring& SubjectString,
   COLstring (*pReplace)(const char*, size_t, void*),
   COLstring (*pFilter)(const COLstring&),
   void* pContext,
   COLuint32 MaxReplacements
) const
{
   COL_METHOD(REXmatcher::functionalReplace);
   ResultString.clear();

   COLASSERT(pReplace != NULL);
   COLASSERT(pMember->pCompiledExpression != NULL);

   COLuint32 CountOfReplace = 0;
   // using native types for compatibility for pcre api
   size_t Position = 0;

   int pOffsets[REX_MATCHER_OFFSETS_SIZE];

   do
   {
      // using native types for compatibility for pcre api

      int ReturnCode = pcre_exec_rex(pMember->pCompiledExpression,
                                     pMember->pStudiedExpression,
                                     SubjectString.c_str(),
                                     SubjectString.size(),
                                     Position,
                                     pMember->StoredOptions & EXECUTE_MASK,
                                     pOffsets,
                                     REX_MATCHER_OFFSETS_SIZE);

      if (ReturnCode < 0)
      {
         break;
      }

      COLstring ReplacementPattern = pReplace(SubjectString.c_str() + pOffsets[0], pOffsets[1] - pOffsets[0], pContext);

      COLstring ReplacementString = pMember->expandBackReferences(ReplacementPattern,
                                                         SubjectString,
                                                         pOffsets,
                                                         ReturnCode,
                                                         pFilter);
      CountOfReplace++;
      REXapplyFilter(ResultString, pFilter, SubjectString, Position, pOffsets[0] - Position);

      ResultString.append(ReplacementString);

      Position = pOffsets[1];

   // We matched an empty sub-string (epsilon).  We limit ourselves
   // do doing this once, so we don't loop indefinitely.
   // There is no point to continue the search when we hit the end of string.
   } while (pOffsets[1] > pOffsets[0] && Position < SubjectString.size() &&
            (MaxReplacements == 0 || CountOfReplace < MaxReplacements));

   size_t SizeOfTail = SubjectString.size() - Position;
   REXapplyFilter(ResultString, pFilter, SubjectString, Position, SizeOfTail);

   return CountOfReplace;
}

COLstring REXmatcherPrivate::expandBackReferences
(
   const COLstring& ReplacementPattern,
   const COLstring& SubjectString,
   int* pOffsets,          // using native types for compatibility for pcre api
   int CountOfSubstring,
   COLstring (*pFilter)(const COLstring&)
) const
{
   COLPRECONDITION(pOffsets != NULL);

   COLstring ReplacementString(ReplacementPattern);

   // using native types for compatibility for pcre api
   size_t Position = 0;
   size_t SearchIndex = 0;

   while (true)
   {
      Position = ReplacementString.find(COLstring(1, REX_MATCHER_BACK_REFERENCE_CHAR),SearchIndex);

      if (Position == npos)
      {
         break;
      }

      if (isdigit(ReplacementString[Position+1]))
      {
         const char* BackReferenceString = NULL;

         // using native types for compatibility for pcre api
         int SubstringIndex = atoi(ReplacementString.substr(Position+1,1).c_str());

         if (pcre_get_substring(SubjectString.c_str(),
                                pOffsets,
                                CountOfSubstring,
                                SubstringIndex,
                                &BackReferenceString) < 0)
         {
            pcre_free_substring(BackReferenceString);
            COL_ERROR_STREAM("Error finding back reference",COLerror::PostCondition);
         }
         else
         {
            COLstring CapturedString = pFilter ? pFilter(BackReferenceString) : BackReferenceString;
            pcre_free_substring(BackReferenceString);

            ReplacementString.replace(ReplacementString.substr(Position,2), CapturedString);

            SearchIndex = Position + CapturedString.size();
         }
      }
      else
      {
         Position++;
      }
   }

   return ReplacementString;
}

int operator==(const REXmatcher& Lhs, const REXmatcher& Rhs)
{
   return Lhs.pattern() == Rhs.pattern();
}

int operator!=(const REXmatcher& Lhs, const REXmatcher& Rhs)
{
   return Lhs.pattern() != Rhs.pattern();
}

COLostream& operator<<(COLostream& Stream , const REXmatcher& Matcher)
{
   Stream << "Regex: '" << Matcher.pattern() << '\'';
   return Stream;
}
