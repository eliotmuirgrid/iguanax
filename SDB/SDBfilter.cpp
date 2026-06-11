// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBfilter
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday 02 March 2023 - 04:12PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <MATCH/MATCHsearch.h>
#include <SDB/SDBcontent.h>
#include <SDB/SDBenvelope.h>
#include <SDB/SDBfilter.h>
#include <SDB/SDBmessage.h>
COL_LOG_MODULE;

void SDBsetFilterType(SDBfilterInfo* pInfo, int Type, bool Allow) {
   pInfo->Types[Type] = Allow;
}

void SDBfilterAllMessages(SDBfilterInfo* pInfo) {
   // TODO: ugly. What if we add a new type in SDBcontent?
   // better to create a message type iterator function in SDBmessage and use it here
   pInfo->Types[SDB_MESSAGE] = true;
   pInfo->Types[SDB_TRACE  ] = true;
   pInfo->Types[SDB_INFO   ] = true;
   pInfo->Types[SDB_DEBUG  ] = true;
   pInfo->Types[SDB_WARNING] = true;
   pInfo->Types[SDB_ERROR  ] = true;
   pInfo->Types[SDB_SUCCESS] = true;
   COLASSERT(SDB_EOT == 12);   // update this function when a new type is introduced
}

void SDBfilterAfterId(SDBfilterInfo* pInfo, const SDBmessageId& Id, bool Inclusive) {
   pInfo->MessageId = Id;
   pInfo->Inclusive = Inclusive;
   // disable all non-message content types
   pInfo->Types[SDB_MARKER  ] = false;
   pInfo->Types[SDB_POSMAP  ] = false;
   pInfo->Types[SDB_POSMAP1 ] = false;
   pInfo->Types[SDB_POSITION] = false;
   COLASSERT(SDB_EOT == 12);   // update this function when a new type is introduced
}

SDBfilterInfo SDBlogFilter(const SDBmessageId& MessageId, bool Inclusive) {
   COL_FUNCTION(SDBlogFilter);
   SDBfilterInfo Filter;
   SDBfilterAllMessages(&Filter);
   SDBfilterAfterId(&Filter, MessageId, Inclusive);
   return Filter;
}

SDBfilterInfo SDBqueueFilter(const SDBmessageId& MessageId, bool Inclusive) {
   COL_FUNCTION(SDBqueueFilter);
   SDBfilterInfo Filter;
   SDBsetFilterType(&Filter, SDB_MESSAGE);
   SDBfilterAfterId(&Filter, MessageId, Inclusive);
   Filter.DirectionNext = true;
   return Filter;
}

SDBfilterInfo SDBqueueFilter(const SDBmessageId& MessageId, bool Inclusive, const COLstring& Search, bool ExactMatch) {
   COL_FUNCTION(SDBqueueFilter);
   SDBfilterInfo Filter;
   SDBsetFilterType(&Filter, SDB_MESSAGE);
   SDBfilterAfterId(&Filter, MessageId, Inclusive);
   Filter.DirectionNext = true;
   Filter.Search = Search;
   Filter.ExactMatch = ExactMatch;
   return Filter;
}

SDBfilterInfo SDBjournalFilter() {
   COL_FUNCTION(SDBjournalFilter);
   SDBfilterInfo Filter;
   SDBfilterAllMessages(&Filter);
   SDBsetFilterType(&Filter, SDB_POSMAP);
   SDBsetFilterType(&Filter, SDB_POSMAP1);
   SDBsetFilterType(&Filter, SDB_POSITION);
   return Filter;
}

// bool SDBenvelopeMatch(const char* pEnvelope, int BufferSize, const SDBfilterInfo& Filter) {
//    COL_FUNCTION(SDBenvelopeMatch);
//    COLint64 Type = SDBenvelopeContentType(pEnvelope, BufferSize);
//    COLASSERT(Type < SDB_EOT);
//    if (!Filter.Types[Type]) { return false; }
//    // TODO: need to check if the Matches string matches as well.
//    // this interface is only useful when we filter by types only.
//    // more complexity just to save a few cycles. not really worth it
//    return true;
// }

static bool SDBfilterMessageId(const SDBfilterInfo& Filter, SDBmessageId Id){
   COL_FUNCTION(SDBfilterMessageId);
   if( Id.DateTime > Filter.StartTime && Filter.StartTime != 0 ) { COL_TRC("Greater than start time"); return false; }
   if( Id.DateTime < Filter.EndTime)                             { COL_TRC("bad end time");            return false; }
   if( Filter.MessageId.isNull())                                {                                     return true;  }
   if( Filter.MessageId.DateTime == 0 )                          {                                     return true;  }
   if( Filter.DirectionNext && Id <  Filter.MessageId)           { COL_TRC("less id");                 return false; }
   if(!Filter.DirectionNext && Id >  Filter.MessageId)           { COL_TRC("greater id");              return false; }
   if(!Filter.Inclusive     && Id == Filter.MessageId)           { COL_TRC("equal id");                return false; }
   return true;
}

static bool SDBfilterRelatedMessages(const SDBfilterInfo& Filter, SDBmessageId Id){
   COL_FUNCTION(SDBfilterRelatedMessages);
   SDBmessageId RelatedMessage = Filter.RelatedMessage;
   COL_VAR2(RelatedMessage, Id);
   if (RelatedMessage.isNull()) { COL_TRC("Related message not set"); return true; }
   if (Id == RelatedMessage)    { COL_TRC("Found a related message"); return true; }
   return false;
}

bool SDBfilterMatch(const SDBcontent* pContent, SDBfilterInfo Filter) {
   COL_FUNCTION(SDBfilterMatch);
   COL_VAR2(pContent->Type, Filter.Types[pContent->Type]);
   if(!Filter.Types[pContent->Type]) {
      COL_TRC("wrong type");
      return false;
   }
   if(!SDBisMessage(pContent)) { return true; }
   SDBmessageId Id      = ((const SDBmessage*)pContent)->Id.MessageId;
   SDBmessageId Related = ((const SDBmessage*)pContent)->RelatedId;
   if(!SDBfilterMessageId(Filter, Id)) { return false; }
   if(!SDBfilterRelatedMessages(Filter, Related)) { return false; }
   if(Filter.Search.is_null()) { return true; }
   COLstring Content(((const SDBmessage*)pContent)->Data);
   Content.toLowerCase();
   COL_VAR(Content);
   COL_VAR2(((const SDBmessage*)pContent)->Data, Filter.Search);
   return MATCHsearch(Filter.Search, Content.toLowerCase(), Filter.ExactMatch, 1000);
}

void SDBfilterMatch(const SDBcontent* pContent, bool* pAllowed, SDBfilterInfo Filter) {
   *pAllowed = SDBfilterMatch(pContent, Filter);
}
