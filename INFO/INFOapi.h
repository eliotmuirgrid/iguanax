#ifndef __INFO_API_H__
#define __INFO_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INFOapi
//
// Description:
//
// API for info stats for IguanaX monitoring
//
// Author: Matthew Sobkowski
// Date:   Tue 11 May 2021 1:03:45 EDT
//---------------------------------------------------------------------------

#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

void INFOapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap);

#endif // end of defensive include