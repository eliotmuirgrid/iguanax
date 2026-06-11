#ifndef __TRN_SAMPLE_GET_H__
#define __TRN_SAMPLE_GET_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNsampleGet
//
// Description:
//
// TRNsampleGet - get sample data from sample data directory - it used to be from a SQLlite database.
//
// Author: Eliot Muir
// Date:   Wednesday, November 17th, 2010 @ 10:38:43 AM
//---------------------------------------------------------------------------
#include <COL/COLstring.h>

COLstring TRNsampleGet(const COLstring& ComponentId, int Id, COLstring* pData, COLstring* pError);
COLstring TRNsampleGet(const COLstring& ComponentId, const COLstring& SampleFile, COLstring* pData, COLstring* pError);
void TRNsampleGetMeta (const COLstring& SampleFile, COLstring* pFunction);

#endif // end of defensive include
