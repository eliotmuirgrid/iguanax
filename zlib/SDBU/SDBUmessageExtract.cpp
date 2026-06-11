// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBUmessageExtract
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sunday 14 May 2023 - 05:59PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <FIL/FILutils.h>
#include <SDB/SDBcapi.h>
#include <SDB/SDBcryptoFile.h>
#include <SDB/SDBfileRead.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBmessageId.h>
#include <SDB/SDBreader.h>
#include <SDBU/SDBUencryptionKey.h>
#include <SDBU/SDBUmessageExtract.h>
COL_LOG_MODULE;

static void SDBUextractMatch(const SDBcontent* pContent, bool* pAllowed, SDBmessageId MessageId) {
   COL_FUNCTION(SDBUextractMatch);
   const SDBmessageId& Id = ((const SDBmessage*)pContent)->Id.MessageId;
   if(Id == MessageId) { (*pAllowed) = true; }
}

bool SDBUmessageExtractSingle(const COLstring& LogFile, const SDBmessageId& MessageId, COLlist<SDBcontent*>* pList){
   COL_FUNCTION(SDBUmessageExtractSingle);
   COLauto<COLclosure2<const SDBcontent*, bool*>> pFilter = COLnewClosure2(&SDBUextractMatch, MessageId);
   if (!SDBUmessageExtract(LogFile, pFilter, pList))        { return false; }
   if (pList->size() == 0 || !SDBisMessage(*pList->begin())){ return false; }
   return true;
}

// adapted from SDBparseEncryptedBuffer() in SDBreader
static void SDBUdecryptBuffer(COLstring& Data, const COLstring& EncKey, COLstring* pPlainData) {
   COL_FUNCTION(SDBUdecryptBuffer);
   // TODO: error handling
   do {
      int Amount = SDBparseAndDecrypt(Data.c_str(), Data.size(), EncKey, pPlainData);
      if (Amount > 0) {
         COL_TRC("extracted and decrypted an encryption packet.");
         Data.shift(Amount);
         continue;
      }
      if (-1 == Amount) { COL_TRC("buffer contains a partial message.");  return; }
      COL_TRC("detected data corruption");
      int Delta = SDBfindNextEncMarker(Data.data(), Data.size());
      if (-1 == Delta)  { COL_TRC("buffer contains corrupted data only"); return; }
      COL_TRC("skip corrupted region");
      Data.shift(Delta);
   } while (true);
}

bool SDBUmessageExtract(const COLstring& LogFile, COLauto<COLclosure2<const SDBcontent*, bool*>> pFilter, COLlist<SDBcontent*>* pList){
   COL_FUNCTION(SDBUmessageExtract);
   if (!FILfileExists(LogFile)){
      COL_TRC("No logs exist for this component.");
      return false;
   }
   COLstring Data;
   if (!SDBfileRead(LogFile, &Data)) { COL_TRC("couldn't access for some reason"); return false; }
   const COLstring EncryptionKey = SDBUencryptionKeyGlobal();
   if (!EncryptionKey.is_null()){
      COL_TRC("Encryption enabled, will need to decrypt Data first");
      COLstring PlainData;
      SDBUdecryptBuffer(Data,EncryptionKey, &PlainData);
      if (PlainData.is_null()) { return false; }
      SDBextractMessages(PlainData.c_str(), PlainData.size(), pFilter, pList);
   } else {
      SDBextractMessages(Data.c_str(), Data.size(), pFilter, pList);
   }
   return true;
}