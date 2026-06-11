#ifndef __GITE_AZURE_UTILS_H__
#define __GITE_AZURE_UTILS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITEazureUtils
//
// Description:
//
// TODO
//
// Author: Vismay Shah 
// Date:   Thursday 16 May 2024 - 12:53PM
// ---------------------------------------------------------------------------
#include <COL/COLarray.h>
#include <WEB/WEBresponse.h>

bool GITEazureEntitlementsParseUser(const COLvar& UserData, COLstring* pResult);
bool GITEazureParseOrganizations(const WEBresponse& Data, COLarray<COLstring>* pResult, COLstring* pError);
bool GITEazureParseProjects(const WEBresponse& Data, const COLstring& OrganizationName, COLvar* pResult, COLstring* pError);

bool GITEazureExtraLogging();
#endif // end of defensive include
