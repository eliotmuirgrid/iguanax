#ifndef __USR_DELETE_H__
#define __USR_DELETE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRdelete
//
// Description:
//
// API for deleting a user
//
// Author: Matthew Sobkowski
// Date:   Mon 29 Mar 2021 11:13:50 EDT
//---------------------------------------------------------------------------
#include "USRmap.h"
class COLwebRequest;
class SCKloop;

void USRdelete(const COLwebRequest& Request, COLmap<COLstring, USRuser>*  pUserMap, SCKloop* pLoop);

#endif