#ifndef __TRNC_EDITTAGS_H__
#define __TRNC_EDITTAGS_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: TRNCeditTags.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   14/11/23 2:07 PM
//  ---------------------------------------------------------------------------
#include <COL/COLarray.h>
#include <COL/COLmap.h>

class COLstring;
class COLwebRequest;
struct CFGmap;
class SCKloop;

void TRNCeditTagsUpdate(COLarray<COLstring>* pTags, const COLarray<COLstring>& AddVec, const COLmap<COLstring, bool>& Lookup);
void TRNCeditTags(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif 
