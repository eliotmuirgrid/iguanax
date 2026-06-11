#ifndef __EDI_RANDOM_H__
#define __EDI_RANDOM_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIrandom
//
// Description:
//
// Create a random EDImessage
//
// Author: Eliot Muir 
// Date:   Wednesday 09 October 2024 - 12:23PM
// ---------------------------------------------------------------------------

#include <EDI/EDIgrammar.h> // to get EDItype

class EDIcollection;
class COLstring;

void EDIremoveRepeatedNodes(EDIcollection* pCollection, EDInode* pNode);

EDInode* EDIsegmentGroupAdd(EDIcollection* pCollection, const COLstring& Name);
EDInode* EDIsegmentAdd     (EDIcollection* pCollection, const COLstring& Name);


EDInode* EDIaddRandomNode(EDIcollection* pCollection, const COLstring& Name, const COLstring& Description, EDItype NodeType, EDItype CT1, EDItype CT2, int ChildCount, const COLstring& Prefix);

#endif // end of defensive include
