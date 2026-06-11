//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: REXfind
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, December 14th, 2012 @ 11:00:20 PM
//---------------------------------------------------------------------------

#include "REXfind.h"
#include "REXoptionMasks.h"

#include <PCRE/pcre.h>

#include <COL/COLostream.h>
#include <COL/COLerror.h>
#include <COL/COLsink.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#define COMPILE_MASK (PUBLIC_COMPILE_OPTIONS)
#define EXECUTE_MASK (PUBLIC_EXEC_OPTIONS)
#define STUDY_MASK   (PUBLIC_STUDY_OPTIONS)
// This define is used to limit the maximum array size that holds substring offsets
// It must be a multiple of 3, as PCRE uses the first 2/3 as a return value, and last 1/3 as scratch
#define MAX_OFFSET_VECTOR_SIZE   ((1 << 10) * 3)

// These ints are used to indicate invalid positions, they must be negative
static const int REX_FIND_POSITION_UNINITIALIZED = -1;
static const int REX_FIND_POSITION_FINISHED = -2;

REXfind::REXfind()
: m_pHandle(NULL), m_pStudyHandle(NULL), m_pSubstrings(NULL), m_SubStringSize(0), m_Unmatched(0,0),
  m_Position(REX_FIND_POSITION_UNINITIALIZED), m_pSubject(NULL), m_SubjectSize(0), m_Options(REXfind::REX_DEFAULT) {
   COL_METHOD(REXfind::REXfind);
}

REXfind::~REXfind(){
   COL_METHOD(REXfind::~REXfind);
   pcre_free_rex(m_pHandle);
   pcre_free_rex(m_pStudyHandle);
   delete []m_pSubstrings;
}

void REXfind::init(const char* pPattern, int Options) {
   COL_METHOD(REXfind::init);
   COL_VAR2(pPattern, Options);

   pcre_free_rex(m_pHandle);
   pcre_free_rex(m_pStudyHandle);
   m_pHandle = NULL;
   m_pStudyHandle = NULL;
   delete []m_pSubstrings;
   m_pSubstrings = NULL;

   // If PCRE cannot compile the expression, it can fill in details of the error to supplied params
   const char* pErrorMessage = NULL;
   int ErrorOffset = 0;
   int ErrorCode = 0;

   m_Options = Options;
   m_pHandle = pcre_compile2(pPattern, m_Options & COMPILE_MASK,
                             &ErrorCode, &pErrorMessage, &ErrorOffset, /* Throw error details into these */
                             NULL /* Use default pcre character tables */);
   COL_VAR2(m_pHandle, ErrorCode);
   if (ErrorCode) {
      // Intercept a few error codes and provide custom error messages for them
      // e.g. An error from an unavailable feature will give an error saying it wasn't compiled in - not useful for an end-user
      switch(ErrorCode) {
      case 21:
         pErrorMessage = "Internal error: Ran out of memory";
         break;
      case 37:
         pErrorMessage = "Escape sequences \\L, \\l, \\N{name}, \\U, and \\u are not supported\nUse groups (e.g. [a-z] for lowercase) for \\L, \\l, \\U, \\u." \
            "\nNamed groups are defined and referenced with angle-brackets or single-quotes (e.g. (?<group>.*) or (\\k'group')).";
         break;
      case 52:
         pErrorMessage = "Internal error: Regular expression is too complex";
         break;
      }
      COL_TRC("Failed to build regex: " << pErrorMessage);
      COL_VAR2(pErrorMessage, ErrorOffset);
      COL_ERROR_STREAM_PLAIN("Regular expression error at character " << ErrorOffset << ": " << pErrorMessage, COLerror::PreCondition);
   }
   // Get some additional details and keep them on hand, so they don't have to be recalculated
   m_pStudyHandle = pcre_study_rex((const pcre*)m_pHandle, m_Options & STUDY_MASK,
                                   &pErrorMessage /* Throw error details into here */);
   // It's possible that the study handle cannot be created because there is no optimization possible
   // If there is a legitimate error, the error message is set appropriately
   COL_VAR(m_pStudyHandle);
#ifdef COL_ENABLE_TRACING
   if (!m_pStudyHandle) {
      COL_TRC("No extra optimizations could be performed over compiled regex");
   }
#endif
   if (pErrorMessage) {
      COL_TRC("Failed to build regex study: " << pErrorMessage);
      COL_VAR2(pErrorMessage, ErrorOffset);
      throw COLerror(pErrorMessage, COLerror::PreCondition);
   }

   pcre_fullinfo((const pcre*)m_pHandle, NULL /* Study isn't required for this query */,
                 PCRE_INFO_CAPTURECOUNT, &m_NumMatchGroups);
   // PCRE needs 3x the number of match groups + 1 (for global match) for the offset vector
   int DesiredOffsetVectorSize = 3;  // a sane default
   if (m_NumMatchGroups > 0) {
      DesiredOffsetVectorSize = (m_NumMatchGroups + 1) * 3;
   }
   COL_VAR2(DesiredOffsetVectorSize, MAX_OFFSET_VECTOR_SIZE);
   if (DesiredOffsetVectorSize > MAX_OFFSET_VECTOR_SIZE) {
      throw COLerror("Regular expression has too many match groups.", COLerror::PreCondition);
   }
   m_SubStringSize = DesiredOffsetVectorSize;

   m_pSubstrings = new int[m_SubStringSize];
}

void REXfind::matchBegin(const char* pSubject, int Size) {
   COL_METHOD(REXfind::matchBegin);
   m_pSubject = pSubject;
   m_SubjectSize = Size;
   COL_VAR2(pSubject, Size);
   m_Position = 0;
}

bool REXfind::matchNext() {
   COL_METHOD(REXfind::matchNext);
   COL_VAR3(m_pSubject, m_SubjectSize, m_Position);
   COLASSERT(m_Position != REX_FIND_POSITION_UNINITIALIZED);  // Did you forget to call matchBegin?
   if (m_Position == REX_FIND_POSITION_FINISHED) {
      COL_TRC("No more matches");
      return false;
   }
   int CountOfMatch = pcre_exec_rex((const pcre*)m_pHandle, (const pcre_extra*)m_pStudyHandle,
                                    m_pSubject, m_SubjectSize, m_Position, m_Options & EXECUTE_MASK,
                                    m_pSubstrings, m_SubStringSize /* Specific match group info */);
   COL_VAR(CountOfMatch);
   if (CountOfMatch == 0) {
      // This shouldn't really happen since the init method calculates the right size for the offset vector
      // But it doesn't hurt to check again
      throw COLerror("Regular expression has too many match groups.", COLerror::PreCondition);
   }
   if (CountOfMatch < 0) {
      COL_TRC("No more matches");
      m_Unmatched.pData = m_pSubject + m_Position;
      m_Unmatched.Size = m_SubjectSize - m_Position;
      m_Position = REX_FIND_POSITION_FINISHED;
      return false;
   }
   m_Unmatched.pData = m_pSubject + m_Position;
   m_Unmatched.Size = m_pSubstrings[0] - m_Position;
   m_Position = m_pSubstrings[1];
   COL_VAR(m_pSubject+m_Position);
   return true;
}

REXresult REXfind::unmatched() const {
   COL_METHOD(REXfind::unmatched);
   COL_VAR(m_Unmatched);
   return m_Unmatched;
}

REXresult REXfind::match(int i) const {
   COL_METHOD(REXfind::match);
   COL_VAR2(i, m_pSubstrings[i*2]);
   COLASSERT(i >= 0 && i < m_SubStringSize);
   COLASSERT(m_pSubstrings[i*2] != -1);
   REXresult Result(m_pSubject + m_pSubstrings[i*2], m_pSubstrings[i*2+1] - m_pSubstrings[i*2]);
   COL_VAR2(Result.Size, Result);
   return Result;
}

int REXfind::numMatchGroups() const {
   COL_METHOD(REXfind::numMatchGroups);
   COL_VAR(m_NumMatchGroups);
   return m_NumMatchGroups;
}

bool REXfind::matchPresent(int i) const{
   COL_METHOD(REXfind::matchPresent);
   COL_VAR(i);
   COLASSERT(i >= 0 && i < m_SubStringSize);
   COL_VAR2(m_pSubstrings[i*2], (bool)(m_pSubstrings[i*2] != -1));
   return (m_pSubstrings[i*2] != -1);
}

COLostream& operator<<(COLostream& Stream, const REXresult& Result) {
   Stream.sink()->write(Result.pData, Result.Size);
   return Stream;
}
