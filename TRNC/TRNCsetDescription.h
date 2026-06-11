#ifndef __TRNC_SET_DESCRIPTION_H__
#define __TRNC_SET_DESCRIPTION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: TRNCsetDescription
//
// Author: Matthew Sobkowski
//
// Sets the description of a component
//---------------------------------------------------------------------------
class COLwebRequest;
struct CFGmap;
class SCKloop;

void TRNCsetDescription(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif