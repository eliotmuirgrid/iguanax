// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIparse
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 26 October 2022 - 09:37AM
// ---------------------------------------------------------------------------

#include <EDI/EDIparse.h>
#include <EDI/EDIparseField.h>
#include <EDI/EDIdata.h>
#include <EDI/EDIgrammar.h>

#include <COL/COLvar.h>
#include <COL/COLutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool EDIendOfFieldChar(unsigned char C) {
   switch(C) {
      case '|':
      case '\r':
         return true;
      default:
         return false;
   }
}

void EDIparseExtractField(EDIdata& Child, const char*& pCursor, const char*& i, const char*& pStart, bool IsTerminating, COLarray<COLstring>* pHere) {
   COL_FUNCTION(EDIparseExtractField);
   COL_TRC("We have a non repeating field.");
   while (!EDIendOfFieldChar(*pCursor) && pCursor < i && (!IsTerminating || *pCursor != '\0') && *pCursor != '~'){
      pCursor++;
   }
   EDIparseField(&Child, pStart, pCursor-pStart, pHere);
}

void EDIparseExtractRepeatingField(EDIdata& Child, const char*& pCursor, const char*& i, const char*& pStart, bool IsTerminating, COLarray<COLstring>* pHere) {
   COL_FUNCTION(EDIparseExtractRepeatingField);
   COL_TRC("We have a repeating field.");
   // if IsTerminating==true we check for the null byte, otherwise ignore
   while (!EDIendOfFieldChar(*pCursor) && pCursor < i && (!IsTerminating || *pCursor != '\0')){
      pCursor++;
   }
   EDIparseRepeatingField(&Child, pStart, pCursor-pStart, pHere);
}

void EDIparseExtractChild(EDIdata* pNode, const char*& pCursor, int Count, const char*& i, const char*& pStart, bool IsLastChild, COLarray<COLstring>* pHere) {
   COL_FUNCTION(EDIparseExtractChild);
   COL_VAR(Count);
   EDIdata& Child = (*pNode)[Count];
   if(Child.isRepeat()) {
      pHere->push_back("Repeat " + COLintToString(Count + 1));
      EDIparseExtractRepeatingField(Child, pCursor, i, pStart, IsLastChild, pHere);
      pHere->pop_back();
   } else {
      EDIparseExtractField(Child, pCursor, i, pStart, IsLastChild, pHere);
   }
}

int EDIparseSkipRestOfSegment(const EDIdata* pNode, const char* pData, const char* i) {
   COL_FUNCTION(EDIparseSkipRestOfSegment);
   COL_TRC("Exceeded the the number of segment fields. Skipping rest of segment");
   const char* pSkipBit = i;
   while(*i != '\r' && *i != 0) { i++; }
   int Consumed = i-pData+1;
   COL_DUMP("Segment:", pSkipBit, i - pSkipBit);
   COL_VAR2(Consumed, *pNode);
   return Consumed;
}

int EDIparseMissingNode(const COLstring& Name, const char* pData, int Size, COLlist<COLstring>& SkippedNodes) {
   COL_FUNCTION(EDIparseMissingNode);
   COL_TRC("Node not found in message definition -- skipping");
   SkippedNodes.push_back(Name);
   const char* i = pData;
   while(i <= pData + Size && *i != '\r') { i++; }
   COL_VAR2(Size, i - pData);
   return -1 * (i - pData + 1);
}

void EDIparseSegmentName(COLstring* pName, const char* pData, int Size){
   COL_FUNCTION(EDIparseSegmentName);
   const char* pEnd = pData + Size;
   const char* i = pData;
   while (i != pEnd && *i != '|'){
      i++;
   }
   *pName = COLstring(pData, i-pData);
   COL_TRC(*pName);
}

int EDIparseSegment(EDIdata* pNode, const char* pData, int Size, const COLhashmap<COLstring, bool>& SegmentsInGrammar,
                    COLlist<COLstring>& SkippedNodes, COLarray<COLstring>* pHere) {
   COL_FUNCTION(EDIparseSegment);
   COL_DUMP("Segment Data:", pData, Size);
   COLstring Name;
   EDIparseSegmentName(&Name, pData, Size);
   COL_VAR2(Name, pNode->name());
   if(SegmentsInGrammar.count(Name) == 0) { return EDIparseMissingNode(Name, pData, Size, SkippedNodes); }
   if(Name != pNode->name()) {
      COL_TRC("Segment name does not match");
      return 0;
   }
   const char* i      = pData + Name.size() + 1;
   const char* pEnd   = pData + Size;
   const char* pStart = i;
   int Count = 0;
   while(i <= pEnd && *i != '\r') {
      if(*i == '|') {
         COL_DUMP("Field:", pStart, i - pStart);
         COL_VAR(Count);
         if(Count == pNode->grammarSize()) { return EDIparseSkipRestOfSegment(pNode, pData, i); }
         const char* pCursor = pStart;
         // pHere->push_back(COLintToString(Count + 1));
         pHere->push_back((*pNode)[Count].name());
         EDIparseExtractChild(pNode, pCursor, Count, i, pStart, false, pHere);
         pHere->pop_back();
         i      = pCursor;
         pStart = pCursor + 1;
         if(*i == '\r') {
            int Consumed = i - pData + 1;
            COL_TRC("Hit end of segment");
            COL_DUMP("Segment:", pData, Consumed);
            COL_VAR(*pNode);
            return Consumed;
         }
         Count++;
      }
      i++;
   }
   COL_DUMP("Remaining Field:", pStart, i-pStart);
   if(i - pStart == 0) {
      COL_TRC("Trailing | character - end");
      return i - pData + 1;
   }
   // TODO - TEST boundary condition when the next statement puts us over our limit
   COL_VAR(Count);
   if (Count == pNode->grammarSize()){ return EDIparseSkipRestOfSegment(pNode, pData, i); }
   const char* pCursor = pStart;
   // pHere->push_back(COLintToString(Count + 1));
   pHere->push_back((*pNode)[Count].name());
   EDIparseExtractChild(pNode, pCursor, Count, i, pStart, true, pHere);
   pHere->pop_back();
   COL_TRC("Exiting parse segment");
   COL_VAR2(pNode->size(), pNode->grammarSize());
   COL_VAR(*pNode);
   return i - pData + 1;
}

void EDIparseConsumeChild(EDIdata* pNode, int& Count, int& Consumed) {
   COL_FUNCTION(EDIparseConsumeChild);
   COL_VAR(Consumed);
   if(Consumed <= 0) {
      COL_TRC("Nothing consumed by child collection or node skipped - so we remove the node.");
      (*pNode).Children.remove(pNode->Children.size() - 1);
      // TODO deal with required
   }
   if(Consumed >= 0) {
      Count++;
      COL_TRC("We got our segment group so increasing our Count = " << Count);
   } else { Consumed = Consumed * -1; }
}

int EDIparseSegmentCollection(EDIdata* pNode, const char* pData, int Size, const COLhashmap<COLstring, bool>& Segments,
                              COLlist<COLstring>& SkippedNodes, COLarray<COLstring>* pHere) {
   COL_FUNCTION(EDIparseSegmentCollection);
   COL_VAR(pNode->name());
   COL_DUMP("Data:", pData, Size);
   if(pNode->grammarSize() == 0) {
      COL_TRC("Grammar is 0 -- adding to skipped nodes");
      SkippedNodes.push_back(pNode->name());
      return 0;
   }
   if (pNode->Type == EDIN_SEGMENT){
      int Consumed = EDIparseSegment(pNode, pData, Size, Segments, SkippedNodes, pHere);
      COL_DUMP("Consumed:", pData, ::abs(Consumed)); // Consumed is potentially a negative int if we skipped instead of consumed
      return Consumed;
   }
   int Count = 0;
   int Remaining = Size;
   const char* pStart = pData;
   do {
      COL_VAR(Count);
      EDIdata& Child = (*pNode)[Count];
      if (pNode->isRepeat()){
         pHere->push_back("Repeat " + COLintToString(Count + 1));
      } else {
         pHere->push_back(Child.name());
      }
      int Consumed = EDIparseSegmentCollection(&Child, pStart, Remaining, Segments, SkippedNodes, pHere);
      pHere->pop_back();
      EDIparseConsumeChild(pNode, Count, Consumed);
      if(Consumed == 0) {
         if(pNode->isRepeat()) {
            COL_TRC("End of repeating sequence.");
            break;
         }
      }
      Remaining -= Consumed;
      pStart += Consumed;
      COL_VAR4(Remaining, Count, pNode->isRepeat(), pNode->grammarSize());
   } while(Remaining > 0 && (pNode->isRepeat() || Count < pNode->grammarSize()));

   COL_TRC("Consumed " << Size - Remaining);
   COL_VAR(*pNode);
   return Size - Remaining;
}

// Finds all the segments in the message definition
void EDIgetSegmentsInMessage(EDIcollection* pCollection, EDInode* pRootNode, COLhashmap<COLstring, bool>* pSegments){
   COL_FUNCTION(EDIgetSegmentsInMessage);
   for (auto const & i : pRootNode->Children){
      if (pCollection->Nodes[i.pNode->Name]->Type == EDI_SEGMENT) {
         COL_TRC("Added " << i.pNode->Name);
         pSegments->add(i.pNode->Name, true);
      } else if (pCollection->Nodes[i.pNode->Name]->Type == EDI_SEGMENT_GROUP){
         COL_TRC("Added group " << i.pNode->Name);
         EDIgetSegmentsInMessage(pCollection, pCollection->Nodes[i.pNode->Name], pSegments);
      }
   }
}

void EDIlistMessages(const EDIcollection &Collection, COLstring* pList) {
   COL_FUNCTION(EDIlistMessages);
   for (auto i = Collection.Nodes.cbegin(); i != Collection.Nodes.cend(); i++) {
      if (i->second->Type == EDI_MESSAGE) {
         *pList += i->first + " ";
      }
   }
}

void EDIreportRules(const EDIcollection& Collection, COLstring* pReport){
   COL_FUNCTION(EDIreportRules);
   COLstring& Report = *pReport;
   if (Collection.MatchRules.size() == 0){
      Report += "No matching rules defined.";
      return;
   }
   Report += "Rules defined with wildcard expressions to match to messages:\n";
   for (int i=0; i < Collection.MatchRules.size(); i++){
      auto& Rule = Collection.MatchRules[i];
      Report += COLintToString(i+1) + ". " + Rule.Match + " --> " + Rule.MessageName + "\n";
   }
}

static void EDIfindMissingFields(EDIdata* pSegment, EDInode* pSegmentGrammar, COLlist<COLstring>& MissingNodes, COLvar& Missing, bool ParentRequired = false) {
   COL_FUNCTION(EDIfindMissingFields);
   COL_VAR2(pSegment->name(), pSegment->value());
   COLmap<COLstring, int> FieldMap; // maps field name to child index
   for (int i = 0; i < pSegment->Children.size(); i++) {
      COL_VAR2(i, (*pSegment)[i].name());
      FieldMap[(*pSegment)[i].name()] = i;
   }
   COL_TRC("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
   COL_VAR3(pSegmentGrammar->Name, pSegmentGrammar->Description, pSegmentGrammar->Type);
   for (auto const & i : pSegmentGrammar->Children) {
      COL_VAR4(i.Description, i.Required, i.pNode->Name, i.pNode->Type);
      if (FieldMap.count(i.Description) == 0) {
         COL_TRC("Field '" << i.Description << "' is absent");
         if (i.Required) {
            COL_TRC("Field '" << i.Description << "' is required");
            if (ParentRequired) {
               MissingNodes.push_back(i.Description);
               Missing.push_back(i.Description);
               COL_WRN("Missing required field '" << i.Description << "'");
            } else {
               COL_TRC("it's fine because parent is optional");
               // if the first required child is missing for an optional parent, the parent doesn't exist
               // no need to check the rest of the children
               break;
            }
         } else {
            COL_TRC("Field '" << i.Description << "' is optional");
            COL_TRC("It's fine");
         }
      } else {
         COL_TRC("Field '" << i.Description << "' is found");
         if ((*pSegment)[FieldMap[i.Description]].isLeaf()) {
            COL_TRC("Field '" << i.Description << "' is simple");
            COL_TRC("Field '" << i.Description << "' has value '" << (*pSegment)[FieldMap[i.Description]].value() << "'");
            if (i.Required) {
               COL_TRC("Simple field '" << i.Description << "' is required");
               if ((*pSegment)[FieldMap[i.Description]].value().is_null()) {
                  COL_TRC("Simple field '" << i.Description << "' is empty");
                  if (ParentRequired) {
                     COL_WRN("Empty required field '" << i.Description << "'");
                     MissingNodes.push_back(i.Description);
                     Missing.push_back(i.Description);
                  } else {
                     COL_TRC("it's fine because parent is optional");
                     // if the first required child is missing for an optional parent, the parent doesn't exist
                     // no need to check the rest of the children
                     break;
                  }
               } else {
                  COL_TRC("Simple field '" << i.Description << "' has value");
                  // A required child exists, other required children must also exist even for an optional parent
                  // e.g. MSH["Sending Application"] is optional but MSH["Sending Application"]["Namespace ID"] and MSH["Sending Application"]["Universal ID"] are both required,
                  // either both "Namespace ID" and "Universal ID" exist or neither exists
                  ParentRequired = true;
                  COL_TRC("It's fine");
               }
            } else {
               COL_TRC("Simple field '" << i.Description << "' is optional");
               COL_TRC("It's fine");
            }
         } else {
            COL_TRC("Field '" << i.Description << "' is complex");
            Missing.push_back(COLvar());
            EDIfindMissingFields(&(*pSegment)[FieldMap[i.Description]], i.pNode, MissingNodes, Missing.array().back()[i.Description], i.Required);
            if (Missing.array().back()[i.Description].size() == 0) { Missing.array().pop_back(); }
         }
      }
      COL_TRC("##########################################################################################");
   }
}

void EDIfindMissingSegments(EDIdata* pNode, EDInode* pMessageGrammar, COLlist<COLstring>& MissingNodes, COLvar& Missing, bool GroupRequired) {
   COL_FUNCTION(EDIfindMissingSegments);
   COLmap<COLstring, bool> SegmentMap;
   COL_VAR2(pNode->size(), pNode->isRepeat());
   if (pNode->isRepeat() && pNode->size()) {
      pNode = pNode->Children[0].get();
   }
   for (auto const & i : pNode->Children) {
      COL_VAR(i->name());
      SegmentMap[i->name()] = true;
   }
   for (auto const & i : pMessageGrammar->Children) {
      COL_VAR3(i.pNode->Name, i.pNode->Type, i.Required);
      if (SegmentMap.count(i.pNode->Name) == 0) {
         if (EDI_SEGMENT_GROUP == i.pNode->Type) {
            COL_TRC("Check segment group '" << i.pNode->Name << "'");
            EDIfindMissingSegments(pNode, i.pNode, MissingNodes, Missing, i.Required);
            continue;
         } else {
            COL_TRC("Segment '" << i.pNode->Name << "' is absent");
            if (i.Required) {
               COL_TRC("Segment '" << i.pNode->Name << "' is required");
               if (GroupRequired) {
                  COL_WRN("Missing required field '" << i.Description << "'");
                  Missing.push_back(i.pNode->Name);
               } else {
                  COL_TRC("it's fine because parent group is optional");
                  // if the first required segment is missing for an optional group, the group doesn't exist
                  // no need to check the rest of the children
                  break;
               }
            } else {
               COL_TRC("Segment '" << i.pNode->Name << "' is optional");
               COL_TRC("It's fine");
            }
         }
      } else {
         if (EDI_SEGMENT_GROUP == i.pNode->Type) {
            COL_TRC("Check segment group '" << i.pNode->Name << "'");
            EDIfindMissingSegments(&(*pNode)[i.pNode->Name], i.pNode, MissingNodes, Missing, i.Required);
            continue;
         } else {
            COL_TRC("Segment '" << i.pNode->Name << "' is present");
            COLASSERT(EDI_SEGMENT == i.pNode->Type);
            Missing.push_back(COLvar());
            EDIfindMissingFields(&(*pNode)[i.pNode->Name], i.pNode, MissingNodes, Missing.array().back()[i.pNode->Name], i.Required);
            if (Missing.array().back()[i.pNode->Name].size() == 0) { Missing.array().pop_back(); }
            if (i.Required) {
               // A required segment exists, other required children must also exist even for an optional parent
               // e.g. the INSURANCE group is optional but IN1 and IN2 are both required.
               // either both "IN1" and "IN2" exist or neither exists
               GroupRequired = true;
            }
         }
      }
   }
   // COLcout << Missing.json(1) << newline;
}
