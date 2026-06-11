#ifndef __TRNC_INIT_H__
#define __TRNC_INIT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNCinit
//
// Description: 
//
// Code to initialize components.
//
// Author: Eliot Muir
// Date:   Tue 12 Jul 2022 15:28:20 EDT
//---------------------------------------------------------------------------

#include "COL/COLarray.h"
struct CFGmap;
class COLstring;
class IWEBconfig;

void TRNCinit(CFGmap* pMap, IWEBconfig* pWebConfig, COLarray<COLstring>* pInvalid);

#endif // end of defensive include
