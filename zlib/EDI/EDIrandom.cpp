// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIrandom
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 09 October 2024 - 12:23PM
// ---------------------------------------------------------------------------

#include <EDI/EDIrandom.h>
#include <EDI/EDIgrammar.h>

#include <COL/COLmath.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool EDIisCircularReference(EDIcollection& Collection, EDInode* pParent, EDInode* pChild){
   COL_FUNCTION(EDIisCircularReference);
   if (pParent == pChild){
      return true;
   }
   for (int i=0; i < pChild->Children.size(); i++){
      COL_VAR2(pChild->Name, pChild->Children[i].pNode->Name);
      if (EDIisCircularReference(Collection, pParent, pChild->Children[i].pNode)){
         return true;
      }
   }
   return false;
}

EDInode* EDIfindRandomNode(EDIcollection& Collection, EDItype Type1, EDItype Type2, EDInode* pParent){
   COL_FUNCTION(EDIfindRandomNode);
   int R = COLmath::randomNumber(Collection.Nodes.size());
   EDInode* pTarget = NULL;
   for (auto i = Collection.Nodes.cbegin(); i != Collection.Nodes.cend(); i++){
      R--;
      if (i->second->Type == Type1 || i->second->Type == Type2){ 
         if (!EDIisCircularReference(Collection, pParent, i->second)) {
            pTarget = i->second;
         } 
      }
      if (pTarget && R <=0 ){
         COL_TRC("Found " << pTarget->Name);
         return pTarget;
      }      
   }
   return pTarget;
}

EDInode* EDIaddRandomNode(EDIcollection* pCollection, const COLstring& Name, const COLstring& Description, EDItype NodeType, EDItype ChildType1, EDItype ChildType2, int ChildCount, const COLstring& Prefix){
   COL_FUNCTION(EDIaddRandomNode);
   pCollection->Nodes.add(Name, new EDInode(Name, Description, NodeType));
   EDInode* pNewNode = pCollection->Nodes[Name];
   for (int i=0; i < ChildCount; i++){
      EDInode* pRandomComposite = EDIfindRandomNode(*pCollection, ChildType1, ChildType2, pNewNode);
      pNewNode->Children.push_back(EDIchild(pRandomComposite, Prefix + " " + COLintToString(i+1), false, false));
   }
   return pNewNode;
}

void EDIremoveRepeatedNodes(EDIcollection* pCollection, EDInode* pNode){
   COL_FUNCTION(EDIremoveRepeatedNodes);
   COLmap<EDInode*, COLstring> Dups;
   for (int i=pNode->Children.size()-1; i >=0; i--){
      EDIchild& Child = pNode->Children[i];
      if (Dups.count(Child.pNode) > 0){
         pNode->Children.remove(i);
      } else {
         Dups.add(Child.pNode, "");
      }
   }
}

void EDInodeRandomizeAttributes(EDInode* pNode){
   COL_FUNCTION(EDInodeRandomizeAttributes);
   for (int i=0; i < pNode->Children.size(); i++){
      pNode->Children[i].Required = COLmath::randomNumber(2) == 1;
      pNode->Children[i].Repeats = COLmath::randomNumber(2)  == 1;
   }
}

EDInode* EDIsegmentGroupAdd(EDIcollection* pCollection, const COLstring& Name){
   COL_FUNCTION(EDIsegmentGroupAdd);
   EDInode* pNode = EDIaddRandomNode(pCollection, Name, "Segment Group", EDI_SEGMENT_GROUP, EDI_SEGMENT, EDI_SEGMENT_GROUP, 4, "Child");
   EDIremoveRepeatedNodes(pCollection, pNode);
   EDInodeRandomizeAttributes(pNode);
   pNode->Children[0].Required = true;
   return pNode;
}

EDInode* EDIsegmentAdd(EDIcollection* pCollection, const COLstring& Name){
   COL_FUNCTION(EDIsegmentAdd);
   int FieldNumber = 10 + COLmath::randomNumber(6);
   COL_VAR(FieldNumber);
   EDInode* pNode = EDIaddRandomNode(pCollection, Name, "Custom Segment", EDI_SEGMENT, EDI_COMPOSITE, EDI_COMPOSITE, FieldNumber, "Field");
   EDInodeRandomizeAttributes(pNode);
   return pNode;
}
