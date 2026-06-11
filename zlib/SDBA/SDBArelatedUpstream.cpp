// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBArelatedUpstream
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Monday 02 October 2023 - 02:30PM
// ---------------------------------------------------------------------------
#include <SDBU/SDBUmessageExtract.h>
#include <SDBU/SDBUutils.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SDB/SDBcontent.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBmessageId.h>
#include <SDBA/SDBArelatedUpstream.h>
COL_LOG_MODULE;

static void SDBUtruncateNthInstance(COLstring* pSource, int Count, const COLstring& Pattern){
   COL_FUNCTION(SDBUtruncateNthInstance);
   size_t FindPosition = 0;
   for (int i=0; i < Count; i++){
      FindPosition = pSource->find(Pattern, FindPosition)+1;
   }
   pSource->setSize(FindPosition-1);
   COL_TRC("Truncated string to " << FindPosition);
}

static bool SDBUcontentTrimAndEscape(COLvar* pDest, const COLstring& Data, const int Threshold, const int MaxLines){
   COL_FUNCTION(SDBUcontentTrimAndEscape);
   bool Truncrated;
   COLstring Out;
   if (Data.size() > Threshold){
      Out = Data.substr(0, Threshold);
      Truncrated = true;
   } else {
      Out = Data;
      Truncrated = false;
   }
   size_t Count = Out.replace("\n", "\n"); // TODO find better way count newlines for truncation
   if (Count > MaxLines) {
      SDBUtruncateNthInstance(&Out, MaxLines, "\n");
   }
   *pDest = Out;
   return Truncrated;
}

static void SDBArelatedUpstreamImpl(const COLstring& Component, int Index, COLuint64 Time, const COLaddress& Address) {
   COL_FUNCTION(SDBArelatedUpstreamImpl);
   COL_VAR3(Component, Index, Time);
   COLstring Error;
   SDBmessage* pMessage = SDBUextractMessage(Component, Index, Time, &Error);
   if (!pMessage) { return COLrespondError(Address, Error); }
   SDBmessageId RelatedId = pMessage->RelatedId;
   COL_VAR(RelatedId);
   delete pMessage;

   COLvar Results;
   Results.setArrayType();
   while (!RelatedId.isNull()) {
      pMessage = SDBUextractMessage(RelatedId.ComponentId, RelatedId.Index, RelatedId.DateTime, &Error);
      if (!pMessage) { return COLrespondError(Address, Error); }
      COLvar Result;
      SDBUcontentToVar(*pMessage, &Result);
      SDBUcontentTrimAndEscape(&Result["content"], pMessage->Data, 1000, 5);
      Results.push_back(Result);
      RelatedId = pMessage->RelatedId;
      COL_VAR(RelatedId);
      delete pMessage;
   }
   COLrespondSuccess(Address, Results);
}

// /related/upstream
void SDBArelatedUpstream(const COLwebRequest& Request) {
   COL_FUNCTION(SDBArelatedUpstream);
   const COLvar& Input = Request.Data;
   COLstring Component = COLvarRequiredString(Input, "component");
   COLstring Error;
   // TODO - must check for permission to view logs of the upstream components
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, ROLid::VIEW_LOGS, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::VIEW_LOGS));
   }

   int       Index     = COLvarRequiredInt(Input, "id");
   COLuint64 Time      = atoll(COLvarRequiredString(Input, "time").c_str());
   COL_VAR3(Component, Index, Time);
   SDBArelatedUpstreamImpl(Component, Index, Time, Request.Address);
}