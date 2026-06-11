#ifndef __USR_ADD_H__
#define __USR_ADD_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRadd
//
// Description:
//
// API for adding a user
//
// Author: Eliot Muir/Orkhan Amikishiyev
//---------------------------------------------------------------------------
#include <COL/COLmap.h>

class SCKloop;
class COLwebRequest;
class SCKloop;
class USRuser;

void USRadd(const COLwebRequest& Request, COLmap<COLstring, USRuser>*  pUserMap, SCKloop* pLoop);

bool USRaddUser(COLmap<COLstring, USRuser>*  pUserMap, USRuser& User, const COLstring& RawPassword, COLstring* pError);

#endif // end of defensive include