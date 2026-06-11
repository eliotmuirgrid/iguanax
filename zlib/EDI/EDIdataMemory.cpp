// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIdataMemory
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Saturday 21 January 2023 - 10:27AM
// ---------------------------------------------------------------------------

#include <EDI/EDIdata.h>
#include <EDI/EDIgrammar.h>

#include <COL/COLsafeCount.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// See IX-1442 - some work left to do to verify that we really are cleaning up the HL7 nodes.
// Creating some code to inc the count of nodes and decrement them - in a thread safe manner
// would be a good tool to verify the behavior is as we want it to be.  TODO Eliot

EDIdata::EDIdata(const EDIcollection& iCollection, const COLstring& MessageName) : Collection(iCollection){
   COL_METHOD(EDIdata::EDIdata-message);
   pParentType = Collection.Nodes[MessageName];
   ChildIndex = -1;
   switch(pParentType->Type){
      case EDI_MESSAGE:       Type = EDIN_MESSAGE;       break;
      case EDI_SEGMENT:       Type = EDIN_SEGMENT;       break;
      case EDI_SEGMENT_GROUP: Type = EDIN_SEGMENT_GROUP; break;
      case EDI_COMPOSITE:     Type = EDIN_FIELD;         break;
   }
   Data = MessageName;
   //COLincCount();  // Comment this out in production
}

// Eliot I deliberately didn't trace this constructor to make sure that memory was being correctly destroyed with the parent objects.
EDIdata::EDIdata(const EDIcollection& iCollection, const EDInode* ipParentType, int iChildIndex, EDIdataType iType)
 : Collection(iCollection), pParentType(ipParentType), ChildIndex(iChildIndex), Type(iType) {
   //COL_METHOD(EDIdata::EDIdata);
   //COL_VAR4(EDItypeAsString(pParentType->Type), pParentType->Description, ChildIndex, EDIdataTypeAsString(Type));
   //COLincCount();  // Comment out in production
}

EDIdata::~EDIdata() {
   COL_METHOD(EDIdata::~EDIdata);
   //COLdecCount();  // Comment out in production
}
