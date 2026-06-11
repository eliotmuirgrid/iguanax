#ifndef __TRNC_CHECKINVALID_H__
#define __TRNC_CHECKINVALID_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNCcheckInvalid
//
// Description:
//
// Check if init had any invalid components when dashboard loaded
//
// Author: Matthew Sobkowski
//---------------------------------------------------------------------------
class COLwebRequest;
struct CFGmap;
class SCKloop;

// checks if TRNCinit had errors -- sets pMap->HasInvalid to false after first report
void TRNCcheckInvalid(const COLwebRequest& Request, CFGmap* pMap);

#endif
