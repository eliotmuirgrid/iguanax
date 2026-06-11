#ifndef __USR_API_H__
#define __USR_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRapi
//
// Description:
//
// API for user framework.
//
// Author: Matthew Sobkowski
// Date:   Mon 29 Mar 2021 11:13:50 EDT
//---------------------------------------------------------------------------
#include "USRmap.h"
#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

class COLstring;
class COLvar;
class SCKloop;
struct PRMpermission;


void USRapiStartup(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, COLmap<COLstring, USRuser>* pUserMap);
void USRapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SCKloop* pLoop, COLmap<COLstring,
                                                                                                              USRuser>* pUserMap);

#endif // end of defensive include