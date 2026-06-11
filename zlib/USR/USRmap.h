#ifndef __USR_MAP_H__
#define __USR_MAP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRmap
//
// Description:
//
// Collection which has our users and groups.
//
// Author: Matthew Sobkowski
// Date:   Mon 29 Mar 2021 11:13:50 EDT
//---------------------------------------------------------------------------
#include <COL/COLmap.h>
#include <USR/USRuser.h>

class COLstring;
class COLvar;

void USRload                   (COLmap<COLstring, USRuser>*  pUserMap);
COLvar USRmapToVarUsers        (COLmap<COLstring, USRuser>*  pUserMap, bool withPassword);
COLvar USRmapToVarLocalUsers   (COLmap<COLstring, USRuser>* pUserMap, bool withPassword);
bool USRmapAuthenticate        (COLmap<COLstring, USRuser>*  pUserMap, const COLstring& Username, const COLstring& Password);

#endif // end of defensive include