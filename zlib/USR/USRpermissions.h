#ifndef __USR_PERMISSIONS_H__
#define __USR_PERMISSIONS_H__
// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: USRpermissions
//
//  Description:
//
//  Author: Matthew Sobkowski
//  Date:   8/9/21, 11:56 AM
// ---------------------------------------------------------------------------
#include "USRmap.h"
#include <COL/COLmap.h>

class COLstring;
class COLvar;

bool USRcheckNeedsAdminUser(COLostream& Out, bool Verbose);
bool USRmapContainsAdmin(COLmap<COLstring, USRuser>* pUserMap);
void USRensureAdminUserExists();
void USRaddAdminUser(COLmap<COLstring, USRuser>* pUserMap, const COLstring& Password);

#endif // end of defensive include
