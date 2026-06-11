#ifndef __TRN_ARGUMENT_IN_CACHE_H__
#define __TRN_ARGUMENT_IN_CACHE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNargumentInCache
//
// Description:
//
// Helper for checking whether the argument cache contains the specified request
//
// Author: Eliot Muir
// Date:   Wednesday, November 24th, 2010 @ 01:19:34 PM
//
//---------------------------------------------------------------------------
#include "TRNargumentRequest.h"

COLhashmapPlace TRNargumentInCache(COLhashmap<COLstring, TRNarg>& Cache, TRNargRequest& Request);

#endif // end of defensive include