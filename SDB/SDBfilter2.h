#ifndef _SDBFILTER2_H
#define _SDBFILTER2_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBfilter2.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-25, 3:49p.m.
//  ---------------------------------------------------------------------------
#include <SDB/SDBcontent.h>
#include <SDB/SDBmessageId.h>

class SDBfilterInfo2 {
  public:
   bool         m_Types[SDB_EOT]{};  // stores which message types are allowed (default: none)
   COLstring    m_Search;
   bool         m_Exact        = false;
   bool         m_Exclusive    = true;
   bool         m_Forward      = true;  // direction - whether we want messages before or after message id
   bool         m_OnlyMessages = false;
   SDBmessageId m_Start;
   SDBmessageId m_End;
   SDBmessageId m_RelatedMessage;
};

// enable or disable filtering on a specific type of messages
void SDBsetFilterType(SDBfilterInfo2* pInfo, int Type, bool Allow = true);
// enable filtering on all types of messages
void SDBfilterAllMessages(SDBfilterInfo2* pInfo);
// filter out messages before the message with the specified id
void SDBfilterAfterId2(SDBfilterInfo2* pInfo, const SDBmessageId& Start, const SDBmessageId& End, bool Exclusive = true);

SDBfilterInfo2 SDBlogFilter2  (const SDBmessageId& Start = SDBmessageId(), const SDBmessageId& End= SDBmessageId(), bool Exclusive = true);
SDBfilterInfo2 SDBqueueFilter2(const SDBmessageId& Start = SDBmessageId(), const SDBmessageId& End= SDBmessageId(), bool Exclusive = true);
SDBfilterInfo2 SDBjournalFilter2();


bool SDBfilterMatch2(const SDBcontent* pContent, const SDBfilterInfo2& Filter);
// The idea behind this interface is that one can make a COLclosure1<const SDBmessage&> with
// it and pass it to a reader routine which is reading messages.
void SDBfilterMatch2(const SDBcontent* pContent, bool* pAllowed, SDBfilterInfo2 Filter);

#endif  // end of defensive include
