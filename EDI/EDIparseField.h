#ifndef __EDI_PARSE_FIELD_H__
#define __EDI_PARSE_FIELD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIparseField
//
// Description:
//
// For tracing convenience we separate out these functions.
//
// Author: Eliot Muir 
// Date:   Thursday 16 March 2023 - 12:23PM
// ---------------------------------------------------------------------------
#include <COL/COLarray.h>
class EDIdata;

void EDIparseSubField        (EDIdata* pNode, const char* pData, int Size, COLarray<COLstring>* pHere);
void EDIparseField           (EDIdata* pNode, const char* pData, int Size, COLarray<COLstring>* pHere);
void EDIparseRepeatingField  (EDIdata* pNode, const char* pData, int Size, COLarray<COLstring>* pHere);

#endif // end of defensive include
