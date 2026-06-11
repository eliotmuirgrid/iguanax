#ifndef __TPJ_API_H__
#define __TPJ_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TPJapi
//
// Description:
//
// Author: Vismay Shah
//---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

void TPJapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);
#endif // end of defensive include
