//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBSretrieveState.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-20, 3:14p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <SDBS/SDBSretrieveState.h>
COL_LOG_MODULE;
SDBSretrieveArgs::SDBSretrieveArgs(const COLstring& TaskGuid, const COLstring& ComponentId, SDBfilterInfo2 Filter,
                                   int Limit, const COLstring& Search, const COLstring& TagSearch,
                                   bool IncludeNonMatches, bool Formatted, bool Minify, bool Relevant)
    : m_Limit(Limit),
      m_Component(ComponentId),
      m_Search(Search),
      m_TagSearch(TagSearch),
      m_IncludeNonMatches(IncludeNonMatches),
      m_Formatted(Formatted),
      m_Minify(Minify),
      m_Relevant(Relevant),
      m_TaskGuid(TaskGuid),
      m_LogFilter(Filter) {}

SDBSretrieveState::SDBSretrieveState()
    : m_JobRunning(false),
      m_EndOfLogs(false),
      m_MaxReached(false),
      m_NextFileTime(""),
      m_TaskGuid("") {}