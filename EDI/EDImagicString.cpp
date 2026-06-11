// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDImagicString
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Friday 05 September 2025 - 02:58PM
// ---------------------------------------------------------------------------
#include <EDI/EDImagicString.h>
#include <EDI/EDIgrammar.h>
#include <EDI/EDIvmdParser.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void EDIrenameKey(EDIcollection* pOutput, const COLstring& OldKey, const COLstring& NewKey) {
   COL_FUNCTION(EDIrenameKey);
   if (OldKey == NewKey) return;
   EDInode* pNode = pOutput->Nodes[OldKey];
   pNode->Name = NewKey;
   pOutput->Nodes.add(NewKey, pNode);
   pOutput->Nodes.erase(OldKey);
}

// Post-pass: strip magic suffix and rename to base or base_i as needed.
// No structural reuse checks; purely a key normalization pass.
void EDIcalculateMagicStringReplacements(EDIcollection* pOutput, COLmap<COLstring, COLstring>* pReplacements) {
   COL_FUNCTION(EDIcalculateMagicStringReplacements);
   // commenting out for release of 10.1.113
   // COLmap<COLstring, bool> UsedReplacements;
   // for (auto it = pOutput->Nodes.begin(); it != pOutput->Nodes.end(); ++it) {
   //    const COLstring& key = it->first;
   //    EDInode* node = it->second;
   //    int Position = -1;
   //    if ((node->Type != EDI_SEGMENT_GROUP) || (!key.find(&Position, EDI_MAGIC_STR)) ) { continue; } // Exclude nodes that are not segment groups or those without magic strings   
   //    COLstring Base, Rest;  // Group7_$IFW$_REFI12 --> Group7, REFI12
   //    key.split(Base, Rest, EDI_MAGIC_STR); // if Group7 exists, need to add index...
   //    COLstring NewKey = (UsedReplacements.count(Base) == 0) ? Base : EDIresolveDuplicateNameAddIndex(&UsedReplacements, Base);
   //    if (NewKey != key) {
   //       (*pReplacements)[key] = NewKey;
   //       UsedReplacements[NewKey] = true;
   //    }
   // }
}

static void EDIapplyReplacementsToChildren(EDInode* pNode, const COLmap<COLstring, COLstring>& Replacements) {
   COL_FUNCTION(EDIapplyReplacementsToChildren);
   int NumChildren = pNode->Children.size();
   if (NumChildren > 0){
      for (int i=0; i < NumChildren; i++){
         EDInode* pChild = pNode->Children[i].pNode;
         if (pChild->Type == EDI_SEGMENT_GROUP && Replacements.count(pChild->Name)) {
            pChild->Name = Replacements[pChild->Name];
         }
      }
   }
}

void EDIapplyMagicStringReplacements(EDIcollection* pOutput, const COLmap<COLstring, COLstring>& Replacements) {
   COL_FUNCTION(EDIapplyMagicStringReplacements);
   if (!pOutput || Replacements.size() == 0) return;
   // Snapshot keys to have a stable iteration order while mutating the map.
   COLarray<COLstring> Keys;
   for (auto it = pOutput->Nodes.begin(); it != pOutput->Nodes.end(); ++it) { Keys.push_back(it->first); }
   for (int i = 0; i < Keys.size(); ++i) {
      const COLstring& OldKey = Keys[i];
      EDIapplyReplacementsToChildren(pOutput->Nodes[OldKey], Replacements);
      if (Replacements.count(OldKey)) {
         EDIrenameKey(pOutput, OldKey, Replacements[OldKey]);
      }
   }
}