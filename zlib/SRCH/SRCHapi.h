#ifndef __SRCH_API_H__
#define __SRCH_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SRCHapi
//
// Description: Registration function for search API
//
// Author: Eliot Muir
//---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

void SRCHapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

#endif  // end of defensive include