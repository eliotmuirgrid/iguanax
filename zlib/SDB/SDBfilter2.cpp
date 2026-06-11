//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBfilter2.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-25, 3:49p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <MATCH/MATCHsearch.h>
#include <SDB/SDBcontent.h>
#include <SDB/SDBfilter2.h>
#include <SDB/SDBmessage.h>
COL_LOG_MODULE;

void SDBsetFilterType(SDBfilterInfo2* pInfo, const int Type, const bool Allow) { pInfo->m_Types[Type] = Allow; }

void SDBfilterAllMessages(SDBfilterInfo2* pInfo) {
   pInfo->m_Types[SDB_MESSAGE] = true;
   pInfo->m_Types[SDB_INFO]    = true;
   pInfo->m_Types[SDB_TRACE]   = true;
   pInfo->m_Types[SDB_DEBUG]   = true;
   pInfo->m_Types[SDB_WARNING] = true;
   pInfo->m_Types[SDB_ERROR]   = true;
   pInfo->m_Types[SDB_SUCCESS] = true;
   COLASSERT(SDB_EOT == 12);  // update this function when a new type is introduced
}

void SDBfilterAfterId2(SDBfilterInfo2* pInfo, const SDBmessageId& Start, const SDBmessageId& End, const bool Exclusive) {
   pInfo->m_Start     = Start;
   pInfo->m_End       = End;
   pInfo->m_Exclusive = Exclusive;
   // disable all non-message content types
   pInfo->m_Types[SDB_MARKER]   = false;
   pInfo->m_Types[SDB_POSMAP]   = false;
   pInfo->m_Types[SDB_POSMAP1]  = false;
   pInfo->m_Types[SDB_POSITION] = false;
   COLASSERT(SDB_EOT == 12);  // update this function when a new type is introduced
}

SDBfilterInfo2 SDBlogFilter2(const SDBmessageId& Start, const SDBmessageId& End, const bool Exclusive) {
   COL_FUNCTION(SDBlogFilter2);
   SDBfilterInfo2 Filter;
   SDBfilterAllMessages(&Filter);
   SDBfilterAfterId2(&Filter, Start, End, Exclusive);
   return Filter;
}

SDBfilterInfo2 SDBqueueFilter2(const SDBmessageId& Start, const SDBmessageId& End, const bool Exclusive) {
   COL_FUNCTION(SDBqueueFilter2);
   SDBfilterInfo2 Filter;
   SDBsetFilterType(&Filter, SDB_MESSAGE);
   SDBfilterAfterId2(&Filter, Start, End, Exclusive);
   Filter.m_Forward = true;
   return Filter;
}

SDBfilterInfo2 SDBjournalFilter2() {
   COL_FUNCTION(SDBjournalFilter2);
   SDBfilterInfo2 Filter;
   SDBfilterAllMessages(&Filter);
   SDBsetFilterType(&Filter, SDB_POSMAP);
   SDBsetFilterType(&Filter, SDB_POSMAP1);
   SDBsetFilterType(&Filter, SDB_POSITION);
   return Filter;
}

static bool SDBfilterMessageId(const SDBfilterInfo2& Filter, const SDBmessageId& Id) {
   COL_FUNCTION(SDBfilterMessageId);
   if(Filter.m_Forward) {
      if(Id > Filter.m_End) return false;
   } else {
      if(Id < Filter.m_End) return false;
   }
   if(Id == Filter.m_Start && !Filter.m_Exclusive) return true;
   if(Filter.m_Forward) { return Id > Filter.m_Start; }
   return Id < Filter.m_Start;
}

static bool SDBfilterRelatedMessages(const SDBfilterInfo2& Filter, const SDBmessageId& Id) {
   COL_FUNCTION(SDBfilterRelatedMessages);
   COL_VAR2(Filter.m_RelatedMessage, Id);
   if(Filter.m_RelatedMessage.isNull()) {
      COL_TRC("Related message not set");
      return true;
   }
   if(Id == Filter.m_RelatedMessage) {
      COL_TRC("Found a related message");
      return true;
   }
   return false;
}

bool SDBfilterMatch2(const SDBcontent* pContent, const SDBfilterInfo2& Filter) {
   COL_FUNCTION(SDBfilterMatch2);
   COL_VAR2(pContent->Type, Filter.m_Types[pContent->Type]);
   if(!Filter.m_Types[pContent->Type]) {
      COL_TRC("wrong type");
      return false;
   }
   if(!SDBisMessage(pContent)) { return true; }
   SDBmessageId Id      = ((const SDBmessage*)pContent)->Id.MessageId;
   SDBmessageId Related = ((const SDBmessage*)pContent)->RelatedId;
   COL_VAR(Related);
   if(Filter.m_OnlyMessages && pContent->Type != SDB_MESSAGE) { return false; }
   if(!SDBfilterMessageId(Filter, Id)) { return false; }
   if(!SDBfilterRelatedMessages(Filter, Related)) { return false; }
   if(Filter.m_Search.is_null()) { return true; }
   COLstring Content(((const SDBmessage*)pContent)->Data);
   Content.toLowerCase();
   COL_VAR(Content);
   COL_VAR2(((const SDBmessage*)pContent)->Data, Filter.m_Search);
   return MATCHsearch(Filter.m_Search, Content.toLowerCase(), Filter.m_Exact, 1000);
}

void SDBfilterMatch2(const SDBcontent* pContent, bool* pAllowed, SDBfilterInfo2 Filter) {
   *pAllowed = SDBfilterMatch2(pContent, Filter);
}