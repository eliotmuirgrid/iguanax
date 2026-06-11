#ifndef __TRNC_SETNAME_H__
#define __TRNC_SETNAME_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: TRNCsetName
//
// Author: Matthew Sobkowski
//
// Sets the name of a component
//---------------------------------------------------------------------------
class COLwebRequest;
struct CFGmap;
class SCKloop;

void TRNCsetName(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif