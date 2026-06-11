#ifndef __CMP_API_H__
#define __CMP_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPapi
//
// Description:
// General purpose (CRUD) APIs for components.
//
// Author: Tyler Brown
//---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

void CMPapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

#endif // end of defensive include