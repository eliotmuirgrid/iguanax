#ifndef __USRSETROLES_H__
#define __USRSETROLES_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: USRsetRoles.h
//
//  Description: Set roles for a user -- requires EDIT_USERS permission
//
//  Author: Matthew Sobkowski
//  Date:   27/02/24 11:50 AM
//  ---------------------------------------------------------------------------
#include "USRmap.h"
class COLwebRequest;
class SCKloop;

void USRsetRoles(const COLwebRequest& Request, COLmap<COLstring, USRuser>*  pUserMap, SCKloop* pLoop);

#endif // end of defensive include
