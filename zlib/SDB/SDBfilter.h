#ifndef __SDB_FILTER_H__
#define __SDB_FILTER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBfilter
//
// Description:
//
// Interface intended to make it reasonable simple to filter messages.
//
// Author: Eliot Muir
// Date:   Thursday 02 March 2023 - 04:12PM
// ---------------------------------------------------------------------------

#include <SDB/SDBcontent.h>
#include <COL/COLarray.h>
#include <SDB/SDBmessageId.h>


// This struct has information to filter with
// It's deliberately meant to be copyable with
// the default assignment operator and copy constructor
class SDBfilterInfo{
public:
   bool Types[SDB_EOT] {}; // stores which message types are allowed (default: none)
   COLstring Search;
   SDBmessageId MessageId;
   bool ExactMatch    = false;
   bool Inclusive     = false;
   bool DirectionNext = true; // direction - whether we want messages before or after message id
   COLuint64 EndTime    = 0;
   COLuint64 StartTime  = 0;
   SDBmessageId RelatedMessage;
   // TODO: regex filter
};

// enable or disable filtering on a specific type of messages
void SDBsetFilterType(SDBfilterInfo* pInfo, int Type, bool Allow = true);
// enable filtering on all types of messages
void SDBfilterAllMessages(SDBfilterInfo* pInfo);
// filter out messages before the message with the specified id
void SDBfilterAfterId(SDBfilterInfo* pInfo, const SDBmessageId& Id, bool Inclusive = false);

SDBfilterInfo SDBlogFilter(const SDBmessageId& MessageId = SDBmessageId(), bool Inclusive = false);
SDBfilterInfo SDBqueueFilter(const SDBmessageId& MessageId = SDBmessageId(), bool Inclusive = false);
SDBfilterInfo SDBqueueFilter(const SDBmessageId& MessageId, bool Inclusive, const COLstring& Search, bool ExactMatch);
SDBfilterInfo SDBjournalFilter();


// The idea behind this interface is that we only look the relevant bytes in the envelope to
// determine whether it's worth the reader's time to extract the message.
// NOTE: This is an optimization for later
// bool SDBenvelopeMatch(const char* pEnvelope, const SDBfilterInfo& Filter);

bool SDBfilterMatch(const SDBcontent* pContent, SDBfilterInfo Filter);
// The idea behind this interface is that one can make a COLclosure1<const SDBmessage&> with
// it and pass it to a reader routine which is reading messages.
void SDBfilterMatch(const SDBcontent* pContent, bool* pAllowed, SDBfilterInfo Filter);

#endif // end of defensive include
