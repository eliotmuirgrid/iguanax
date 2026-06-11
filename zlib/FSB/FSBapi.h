#ifndef __FSB_API_H__
#define __FSB_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: FSBapi
//
// Author: Shuo Hou
//
// Description:
//
// set up apis used by file browser
//---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

void FSBapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);


#endif // end of defensive include
