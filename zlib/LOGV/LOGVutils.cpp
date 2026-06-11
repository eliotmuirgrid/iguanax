
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LOGVutils.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   28/04/25 11:56 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <LND/LNDdebug.h>
#include <LOGV/LOGVutils.h>
#include <TRN/TRNannotationState.h>
#include <TRN/TRNwriteScriptError.h>
COL_LOG_MODULE;

bool LOGVisolateHL7Message(const COLstring& Input, COLstring* pMessage) {
   COL_FUNCTION(LOGVisolateHL7Message);
   COL_TRC_DUMP("data", Input.c_str(), Input.size());
   // Find the start of the HL7 message (MSH segment)
   const size_t Start = Input.find("MSH|");
   if(Start == npos) { return false; }  // No HL7 message found
   // Find the first double newline after MSH (indicating the end of a message) or use the end of string if not found
   size_t end = Input.find("\n\n", Start);
   if(end == npos) { end = Input.size(); }
   *pMessage = Input.substr(Start, end - Start);
   COL_TRC_DUMP("message", pMessage->c_str(), pMessage->size());
   return true;
}

void LOGVparseId(COLvar* pResult) {
   COL_FUNCTION(LOGVparseId);
   COL_VAR(pResult->json(1));
   if(!pResult->exists("BlockData")) { return; }
   const COLvar&   BlockData  = (*pResult)["BlockData"][0];
   const COLstring HtmlString = BlockData["scope"]["d"].asString();
   if(HtmlString.is_null()) { return; }
   const COLstring idKey   = "id='";
   size_t          Pos     = 0;
   int             IdCount = 0;
   COLstring       ParsedId;
   // Loop to find the third occurrence of 'id=' which is our message
   while((Pos = HtmlString.find(idKey, Pos)) != npos) {
      ++IdCount;
      Pos += idKey.size();
      if(IdCount == 3) {
         const size_t EndPos = HtmlString.find("'", Pos);
         if(EndPos != npos) { ParsedId = HtmlString.substr(Pos, EndPos - Pos); }
         break;
      }
   }
   if(!ParsedId.is_null()) { (*pResult)["id"] = ParsedId; }
}