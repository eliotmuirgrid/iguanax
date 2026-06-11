// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIcopy
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 09 October 2024 - 12:44PM
// ---------------------------------------------------------------------------

#include <EDI/EDIcopy.h>
#include <EDI/EDIgrammar.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void EDIcopy(const EDIcollection& Source, EDIcollection* pDestination, const COLstring& NodeName){
   COL_FUNCTION(EDIcopy);
   COL_VAR(NodeName);
   const EDInode* pParent = Source.Nodes[NodeName];

   EDInode* pNewNode = new EDInode(pParent->Name, pParent->Description, pParent->Type);
   pDestination->Nodes.add(NodeName, pNewNode);
   for (int i=0; i < pParent->Children.size(); i++){
      COLstring ChildName = pParent->Children[i].pNode->Name;
      if (!pDestination->Nodes.find(ChildName)){
         EDIcopy(Source, pDestination, ChildName);
      }
      EDIchild& Child = pNewNode->Children.push_back(EDIchild(pDestination->Nodes.find(ChildName).value(), 
      pParent->Children[i].Description, pParent->Children[i].Required, pParent->Children[i].Repeats));
   }
}