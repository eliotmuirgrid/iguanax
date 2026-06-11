#ifndef _SDBUPARAMS_H
#define _SDBUPARAMS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBUparams.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-09-04, 2:58p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLstring.h>
#include <SDB/SDBfilter2.h>

class SDBUparams {
public:
   SDBUparams(const COLstring& Component, const COLvar& RequestData);
   int          m_Limit;              // Maximum number of log entries to retrieve
   COLstring    m_Component;          // Component
   COLstring    m_Search;             // This cannot be set to LogFilter search since it would remove m_IncludeNonMatches functionality
   COLstring    m_TagSearch;          // Tag-based filter
   bool         m_IncludeNonMatches;  // Whether to apply text matching
   bool         m_Formatted;          // Whether we want to highlight and have url markup
   bool         m_Minify;             // Whether or not we had the content to the result
   bool         m_Relevant;           // when true we only search 24hrs from first match
   COLstring    m_Job;                // Asynchronous job identifier
   time_t       m_Timeout;            // limits time spent for component card and log browser's get the last 3 error messages requests
   SDBfilterInfo2 m_LogFilter;
};

bool SDBSnonCriticalJobTimedOut(const SDBUparams& Params);

#endif  // end of defensive include
