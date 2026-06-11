#ifndef __USRSETPASSWORD_H__
#define __USRSETPASSWORD_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: USRsetPassword.h
//
//  Description: Set password of a user
//    -- those with EDIT_USERS permission can edit all passwords
//    -- users may also change their own password
//  Author: Matthew Sobkowski
//  Date:   27/02/24 11:50 AM
//  ---------------------------------------------------------------------------
#include "USRmap.h"
class COLwebRequest;
class SCKloop;

void USRsetPassword(const COLwebRequest& Request, COLmap<COLstring, USRuser>*  pUserMap, SCKloop* pLoop);

#endif // end of defensive include
