#ifndef _SDBSSEARCHSTATE_H
#define _SDBSSEARCHSTATE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBSretrieveState.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-20, 3:14p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLvar.h>
#include <SDB/SDBfilter2.h>
#include <SDB/SDBfile.h>
class SCKloop;

class SDBSretrieveItem {
public:
   COLstring m_TaskGuid;
   COLarray<COLvar> m_Results;
   bool              m_Terminate  = false;
   bool              m_EndOfLogs  = false;
   bool              m_MaxReached = false;
   COLstring         m_NextFile   = "";
};

class SDBSretrieveArgs {
  public:
   SDBSretrieveArgs(const COLstring& TaskGuid, const COLstring& ComponentId, SDBfilterInfo2 Filter, int Limit,
                    const COLstring& Search, const COLstring& TagSearch, bool IncludeNonMatches, bool Formatted,
                    bool Minify, bool Relevant);
   int            m_Limit;
   COLstring      m_Component;
   COLstring      m_Search;
   COLstring      m_TagSearch;
   bool           m_IncludeNonMatches;
   bool           m_Formatted;
   bool           m_Minify;
   bool           m_Relevant;
   SDBfile        m_LogFile;
   COLstring      m_TaskGuid;
   SDBfilterInfo2 m_LogFilter;
};

class SDBSretrieveState {
  public:
   explicit SDBSretrieveState();
   bool      m_JobRunning;
   bool      m_EndOfLogs;
   bool      m_MaxReached;
   COLstring m_Component;
   COLstring m_NextFileTime;
   COLstring m_TaskGuid;
   COLvar    m_Result;
};

#endif // end of defensive include
