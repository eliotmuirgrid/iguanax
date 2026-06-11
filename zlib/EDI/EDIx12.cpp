//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: EDIx12.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/10/23 2:22 PM
//  ---------------------------------------------------------------------------
#include <EDI/EDIx12.h>
#include <EDI/EDIdata.h>
#include <EDI/EDIgrammar.h>
#include <EDI/EDIparse.h>

#include <COL/COLlog.h>
#include <COL/COLutils.h>
COL_LOG_MODULE;

static void EDIx12DefaultDelimiters(char* pElementDelimiter, char* pSegmentDelimiter, char* pSubElementDelimiter) {
   COL_FUNCTION(EDIx12DefaultDelimiters);
   *pElementDelimiter = '*';
   *pSegmentDelimiter = '~';
   *pSubElementDelimiter = ':';
}

static bool EDIx12ParseDelimiters(COLstring* pMessage, char* pElementDelimiter, char* pSegmentDelimiter, char* pSubElementDelimiter, COLstring* pError) {
   COL_FUNCTION(EDIx12ParseDelimiters);
   COLstring& Message = *pMessage;
   *pElementDelimiter = Message[3];
   int MessageSize = Message.size();
   int ElementCount = 0, DelimiterIndex = 0;
   for(int i = 0; i < MessageSize; i++) {
      if(Message[i] == *pElementDelimiter && ++ElementCount == 16) { //x12 segment and subelement delims found after 16th element
         DelimiterIndex = i + 1;
         break;
      }
   }

   if(MessageSize < DelimiterIndex + 1) {
      *pError = "Preprocessing failed -- invalid ISA segment length";
      return false;
   }
   *pSubElementDelimiter = Message[DelimiterIndex];
   *pSegmentDelimiter    = Message[DelimiterIndex + 1];
   return true;
}

bool EDIx12Preprocess(COLstring* pMessage, COLstring* pError) {
   COL_FUNCTION(EDIx12Preprocess);
   COLstring& Message = *pMessage;
   char ElementDelimiter, SegmentDelimiter, SubElementDelimiter;

   if(Message.substr(0, 3) != "ISA") {
      COL_TRC("No ISA found -- defaulting delimiters");
      EDIx12DefaultDelimiters(&ElementDelimiter, &SegmentDelimiter, &SubElementDelimiter);
   } else {
      COL_TRC("ISA exists -- parsing delimiters");
      bool Success = EDIx12ParseDelimiters(pMessage, &ElementDelimiter, &SegmentDelimiter, &SubElementDelimiter, pError);
      if(!Success) { return false; }
   }


   COL_VAR3(SegmentDelimiter, ElementDelimiter, SubElementDelimiter);
   int Mapping[256] = {0};
   Mapping[SegmentDelimiter]    = '\r';
   Mapping[ElementDelimiter]    = '|';
   Mapping[SubElementDelimiter] = '^';

   // Replace delimiter characters using lookup table
   char* pBuffer = Message.get_buffer();
   while(*pBuffer) {
      if(Mapping[*pBuffer] != 0) { *pBuffer = (char) Mapping[*pBuffer]; }
      pBuffer++;
   }

   Message.replace("\r\n", "\n");
   Message.replace('\n', '\r');
   return true;
}

bool EDIx12ParseWithRules(const EDIcollection& Collection, const COLstring& Data, COLstring* pMatched) {
   COL_FUNCTION(EDIx12ParseWithRules);
   if(Data.substr(0, 3) != "ISA") {
      *pMatched = "Required ISA segment not found at the start of the message.";
      return false;
   }
   for(const auto& Rule : Collection.MatchRules) {
      if(Rule.MessageName.equalsIgnoreCase("ignore")) { continue; }
      COL_TRC("Message matched rule: " + Rule.Match + " ,using message: " + Rule.MessageName);
      *pMatched = Rule.MessageName;
      return true;
   }
   EDIreportRules(Collection, pMatched);
   return false;
}

bool EDIx12Parse(const EDIcollection& Collection, COLstring* pData, COLstring* pMessage, COLstring* pErr) {
   COL_FUNCTION(EDIx12Parse);
   COL_TRC_DUMP("Data before:", pData->c_str(), pData->size());
   if(!EDIx12Preprocess(pData, pErr)) { return false; }
   COL_TRC_DUMP("Data after processing:", pData->c_str(), pData->size());

   bool EmptyMatchRules = Collection.MatchRules.size() == 0;
   if(pMessage->is_null() && EmptyMatchRules) {
      *pErr = "No matching rules were found in the VMD file.\nPlease update the VMD to include matching rules,\nor to parse using this VMD, provide a message name to match.";
   } else if(pMessage->is_null() && !EmptyMatchRules && !EDIx12ParseWithRules(Collection, *pData, pMessage)) {
      *pErr = *pMessage;
   } else if(!pMessage->is_null() && (Collection.Nodes.count(*pMessage) == 0 || EDI_MESSAGE != Collection.Nodes[*pMessage]->Type)) {
      COLstring List;
      EDIlistMessages(Collection, &List);
      *pErr = "Message name " + *pMessage + " does not exist. Try one of these instead: " + List;
   }
   // no else case because that means we have a valid pMatch that exists in the collection match rules
   return true;
}