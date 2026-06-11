#ifndef __USR_SET_ADMIN_H__
#define __USR_SET_ADMIN_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRsetAdmin
//
// Description:
//
// Routine to set admin user and password. 
//
// Author: Tyler Brown 
// Date:   Friday 18 August 2023 - 01:28PM
// ---------------------------------------------------------------------------
#include <COL/COLmap.h>
class COLstring;
class COLostream;
class USRuser;

void USRsaveAdmin(const COLmap<COLstring, USRuser>& UserMap);
void USRsetAdmin(const COLstring& Password, COLostream& OutStream);
void USRsetAdminPassword(const COLstring& Password);


#endif // end of defensive include
