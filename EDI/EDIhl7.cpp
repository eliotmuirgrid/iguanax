//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: EDIhl7.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/10/23 2:22 PM
//  ---------------------------------------------------------------------------
#include <EDI/EDIhl7.h>
#include <EDI/EDIdata.h>
#include <EDI/EDIgrammar.h>
#include <EDI/EDIparse.h>

#include <COL/COLlog.h>
#include <COL/COLutils.h>
COL_LOG_MODULE;

bool EDIhl7Preprocess(COLstring* pMessage, COLstring* pError){
   COL_FUNCTION(EDIhl7Preprocess);
   COLstring& Message = *pMessage;
   if (Message.size() < 9){
      *pError = "Message is too small.";
      return false;
   }
   if(Message.substr(0, 3) != "MSH") {
      *pError = "MSH segment is either missing or not at the start";
      return false;
   }

   if (Message[3] != '|' || Message[4] != '^' || Message[5] != '~' || Message[6] != '\\' || Message[7] != '&') {
      COL_TRC("Custom delimiters used -- preprocessing to defaults");
      const char Field       = Message[3];
      const char SubField    = Message[4];
      const char Repeat      = Message[5];
      const char Escape      = Message[6];
      const char SubSubField = Message[7];

      int Mapping[256] = {0};
      Mapping[Field]       = '|';
      Mapping[SubField]    = '^';
      Mapping[Repeat]      = '~';
      Mapping[Escape]      = '\\';
      Mapping[SubSubField] = '&';

      char* pBuffer = Message.get_buffer();
      while (*pBuffer) {
         if (Mapping[*pBuffer] != 0) { *pBuffer = (char) Mapping[*pBuffer]; }
         pBuffer++;
      }
   }

   Message.replace("\r\n", "\n");
   Message.replace('\n', '\r');

   Message.remove(0, 8);
   Message.prepend("MSH||");
   return true;
}

void EDIpostProcess(EDIdata* pNode){
   COL_FUNCTION(EDIpostProcess);
   EDIdata& Message = *pNode;
   if (Message.grammarSize() >= 1 && Message[0].grammarSize() > 2){
      COL_TRC("Setting MSH separator characters");
      Message[0][0].setValue("|");
      Message[0][1].setValue("^~\\&");
   }
}

bool EDIgetMsh9(const COLstring& Data, COLstring* pResult) {
   COL_FUNCTION(EDIgetMsh9);
   COLstring MSH, Remainder;
   Data.split(MSH, Remainder, "\r");  // TODO - optimize by not allocating temporary
   COL_DUMP("Data", Data.c_str(), Data.size());
   // First split on the Field Separator to get MSH.9
   COLstring Field;
   int i = 0;
   do {
      i++;
      COL_VAR(i);
      if (!MSH.split(Field, MSH, "|")) {
         *pResult = "Required MSH.9 field not present.";
         return false;
      }
      COL_VAR2(Field, MSH);
      if (i == 10) {
         *pResult = Field;
         return true;
      }
   } while (true);
}

bool EDIcheckMshFirst(const EDIcollection& Collection) {
   COL_FUNCTION(EDIcheckMshFirst);
   if(Collection.Nodes.count("MSH") == 0) { return false; }
   for(auto it = Collection.Nodes.cbegin(); it != Collection.Nodes.cend(); it++) {
      if(it->second->Type == EDI_MESSAGE) {
         const COLarray<EDIchild>& Children = it->second->Children;
         if(Children.size() > 0 && Children[0].pNode->Name != "MSH") { return false; } // IX-4162 changed if conditions to allow for empty message defs
         COL_VAR2(it->first, Children[0].pNode->Name);
      }
   }
   return true;
}

bool EDIparseMshWithRules(const EDIcollection& Collection, const COLstring& Data, COLstring* pMatched) {
   COL_FUNCTION(EDIparseMshWithRules);
   if(Data.substr(0, 3) != "MSH") {
      *pMatched = "Required MSH segment not found at the start of the message.";
      return false;
   }
   COLstring MSH9;
   if (!EDIgetMsh9(Data, &MSH9)){
      *pMatched = MSH9;
      return false;
   }
   for (const auto &Rule: Collection.MatchRules) {
      if (COLglobMatch(Rule.Match.c_str(), MSH9.c_str())) {
         COL_TRC("Message matched rule: " + Rule.Match + " ,using message: " + Rule.MessageName);
         *pMatched = Rule.MessageName;
         return true;
      }
   }
   // TODO - could put the list of matching rules into the error message
   *pMatched = "No matching rules exist that match a message with MSH.9 = '" + MSH9 + "'\n";
   EDIreportRules(Collection, pMatched);
   return false;
}

bool EDIparseMsh(const EDIcollection& Collection, COLstring* pData, COLstring* pMatch, COLstring* pErr) {
   COL_FUNCTION(EDIparseMsh);
   COL_TRC("Preprocess the message");
   COL_TRC_DUMP("Data before:", pData->c_str(), pData->size());
   COLstring Error;
   if(!EDIhl7Preprocess(pData, pErr)) { return false; }
   COL_TRC_DUMP("Data after processing:", pData->c_str(), pData->size());
   if(!EDIcheckMshFirst(Collection)) {
      // ELIOT - TODO it would make more sense to do this in the caching logic - the vmd is always going to be wrong
      // Also the language doesn't clearly communicate that the problem is grammar, not the message itself.
      *pErr = "MSH segment is either missing or not at the start";
      return false;
   }

   bool EmptyMatchRules = Collection.MatchRules.size() == 0;
   if(pMatch->is_null() && EmptyMatchRules) {
      *pErr = "No matching rules were found in the VMD file.\nPlease update the VMD to include matching rules,\nor to parse using this VMD, provide a message name to match.";
   } else if(pMatch->is_null() && !EmptyMatchRules && !EDIparseMshWithRules(Collection, *pData, pMatch)) {
      *pErr = *pMatch;
   } else if(!pMatch->is_null() && (Collection.Nodes.count(*pMatch) == 0 || EDI_MESSAGE != Collection.Nodes[*pMatch]->Type)) {
      COLstring List;
      EDIlistMessages(Collection, &List);
      *pErr = "Message name " + *pMatch + " does not exist. Try one of these instead: " + List;
   }
   // no else case because that means we have a valid pMatch that exists in the collection match rules
   return true;
}