#ifndef __TRN_ANNOTATION_NODE_H__
#define __TRN_ANNOTATION_NODE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNannotationNode
//
// Description:
//
// Code related to the annotation of Node tree type data - HL7/X12 and DB trees. 
//
// Author: Vismay Shah 
// Date:   Thursday 04 May 2023 - 10:37AM
// ---------------------------------------------------------------------------
#include <NOD/NODaddress.h>

class TRNfunctionCall;

void TRNstandardOnAssignedChild(TRNfunctionCall* pCurrent, int Line, const NODaddress& Address, const COLstring& Value, NODaddress& LastDestination);
void TRNstandardOnNodeCopy(TRNfunctionCall* pCurrent, int Line, const NODaddress& Source, const NODaddress& Destination, NODaddress& LastDestination, NODaddress& LastSource);
void TRNstandardOnNodeCleared(TRNfunctionCall* pCurrent, int Line, const NODaddress& Destination, NODaddress& LastDestination);

void TRNmapAddressCacheClear(NODaddress* pLastDestination, NODaddress* pLastSource);

#endif // end of defensive include