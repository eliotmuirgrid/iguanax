#ifndef SDBSLOGDETAILSEARCHSTATE_H
#define SDBSLOGDETAILSEARCHSTATE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBSlogDetailSearchState.h
//
//  Description: Similar to SDBSretrieveState but for specific log message highlighting
//
//  Author: Matthew Sobkowski
//  Date:   01/08/25 11:37 AM
//  ---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLvar.h>
#include <CHNK/CHNKviewString.h>
#include <memory>
class SCKloop;

class SDBSdetailSearchItem {
public:
   COLstring m_TaskGuid;
   COLvar    m_Result;
   bool      m_Running    = false;
   int       m_Count      = 0;
   SDBSdetailSearchItem() = default;
};

class SDBSdetailSearchArgs {
  public:
   SDBSdetailSearchArgs(const COLstring& Component, const int Index, const COLstring& TaskGuid,
                        const COLuint64 Time, const COLstring& Search, const COLuint64 ChunkId,
                        const CHNKviewMode& Mode)
       : m_TaskGuid(TaskGuid),
         m_Component(Component),
         m_Search(Search),
         m_Index(Index),
         m_Time(Time),
         m_ChunkId(ChunkId),
         m_ViewMode(Mode) {};
   COLstring    m_TaskGuid;
   COLstring    m_Component;
   COLstring    m_Search;
   int          m_Index;
   COLuint64    m_Time;
   COLuint64    m_ChunkId;
   CHNKviewMode m_ViewMode;
   int          m_Increment = 10;
   int          m_Remaining = 1000;  // only want 1000 results for search

   void increment() { this->m_ChunkId += m_Increment; };
};

using SDBSdetailSearchCallback = COLauto<COLclosure3<bool, std::shared_ptr<COLvar>, std::shared_ptr<SDBSdetailSearchArgs>>>;

void SDBSdetailSearchWorker(std::shared_ptr<SDBSdetailSearchArgs> pArgs, SDBSdetailSearchCallback pCallback, SCKloop* pLoop);

#endif // end of defensive include
