#ifndef __TRNC_SAVE_CONFIG_H__
#define __TRNC_SAVE_CONFIG_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNCsaveConfig
//
// Label: Save function for async saving of TRNC config
//
// Author: Matthew Sobkowski
// Date:   Wednesday, June 15, 2022
//---------------------------------------------------------------------------
#include <COL/COLweb.h>

class SCKloop;
struct CFGconfig;

// saves CFGcore to components/configurations/<guid>.json
void TRNCsaveConfig(const COLstring& Username, const CFGconfig& Config, SCKloop* pLoop, const COLaddress& Address);

#endif // end of defensive include
