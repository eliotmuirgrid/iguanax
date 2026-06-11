#ifndef __USR_LIST_H__
#define __USR_LIST_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRlist
//
// Description:
//
// API for listing all users
//
// Author: Matthew Sobkowski
// Date:   Mon 29 Mar 2021 11:13:50 EDT
//---------------------------------------------------------------------------
#include <COL/COLmap.h>

class COLvar;
class COLwebRequest;
class USRuser;

void USRlist(const COLwebRequest& Request, COLmap<COLstring, USRuser>* pUserMap);

#endif // end of defensive include