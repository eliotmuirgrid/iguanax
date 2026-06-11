#ifndef __EDI_PARSE_H__
#define __EDI_PARSE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIparse
//
// Description:
//
// Routines to parse HL7 into a EDIdatatree.
//
// Author: Eliot Muir
// Date:   Wednesday 26 October 2022 - 09:37AM
// ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLarray.h>
#include "COL/COLlist.h"

class EDIcollection;
class EDIdata;
class EDInode;
class COLstring;

int EDIparseSegment(EDIdata* pNode, const char* pData, int Size, const COLhashmap<COLstring, bool>& SegmentsInGrammar,
                    COLlist<COLstring>& SkippedNodes, COLarray<COLstring>* pHere);

int EDIparseSegmentCollection(EDIdata* pNode, const char* pData, int Size, const COLhashmap<COLstring, bool>& Segments,
                              COLlist<COLstring>& SkippedNodes, COLarray<COLstring>* pHere);

void EDIgetSegmentsInMessage(EDIcollection* pCollection, EDInode* pRootNode, COLhashmap<COLstring, bool>* pSegments);

void EDIparseSegmentName(COLstring* pName, const char* pData, int Size);

void EDIlistMessages(const EDIcollection& Collection, COLstring* pList);

void EDIreportRules(const EDIcollection& Collection, COLstring* pReport);

// void EDIfindMissingSegments(EDIdata* pNode, EDInode* pMessageGrammar, COLlist<COLstring>& MissingNodes);
void EDIfindMissingSegments(EDIdata* pNode, EDInode* pMessageGrammar, COLlist<COLstring>& MissingNodes, COLvar& Missing, bool GroupRequired = true);

#endif // end of defensive include
