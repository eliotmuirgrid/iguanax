//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNargumentInCache
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, November 24th, 2010 @ 01:19:34 PM
//---------------------------------------------------------------------------
#include <TRN/TRNargumentInCache.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLhashmapPlace TRNargumentInCache(COLhashmap<COLstring, TRNarg>& Cache, TRNargRequest& Request){
   COL_FUNCTION(TRNargumentInCache);
   return Cache.find(Request.m_Id);
}
