#ifndef __TRN_SAMPLE_DATA_H__
#define __TRN_SAMPLE_DATA_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNsampleData
//
// Description: 
//
// Function to fetch sample data and if a .meta file is present, the function to call
//
// Author: Eliot Muir
// Date:   Tuesday, November 16th, 2010 @ 02:58:17 PM
//---------------------------------------------------------------------------
class COLstring;
class COLvar;

void TRNsampleDataPopulate(const COLvar& Params, const COLstring& ComponentId, COLstring* pData, COLstring* pFunction);

#endif // end of defensive include
