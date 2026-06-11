// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIprune
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 10 October 2024 - 09:30AM
// ---------------------------------------------------------------------------

#include <EDI/EDIprune.h>
#include <EDI/EDIgrammar.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void EDIcat(EDIcollection* pCollection, EDInode* pNode, COLmap<EDInode*, COLstring>* pUsedMap){
   COL_FUNCTION(EDIcat);
   pUsedMap->add(pNode, "");
   for (int i=0; i < pNode->Children.size(); i++){
      if (pUsedMap->count(pNode->Children[i].pNode) == 0){
         EDIcat(pCollection, pNode->Children[i].pNode, pUsedMap);
      }
   }
}

void EDIprune(EDIcollection* pCollection){
   COL_FUNCTION(EDIprune);
   COLmap<EDInode*, COLstring> UsedMap;
   for (auto i=pCollection->Nodes.begin(); i != pCollection->Nodes.end(); i++){
      if (i->second->Type == EDI_MESSAGE){
         EDIcat(pCollection, i->second, &UsedMap);
      }
   }
   COLarray<COLstring> UnusedList;
   for (auto i=pCollection->Nodes.begin(); i != pCollection->Nodes.end(); i++){
      if (UsedMap.count(i->second) == 0){
         UnusedList.push_back(i->first);
      }
   }
   for (int i=0; i < UnusedList.size(); i++){
      delete pCollection->Nodes[UnusedList[i]];
      pCollection->Nodes.removeKey(UnusedList[i]);
      COL_TRC("Removed " << UnusedList[i]);
   } 
}