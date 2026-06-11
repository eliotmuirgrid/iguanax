#ifndef __TRN_ARGUMENT_REQUEST_PARSE_H__
#define __TRN_ARGUMENT_REQUEST_PARSE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNargumentRequestParse
//
// Description:
//
// Helper functions for converting COLvar full argument request data to C++ data structure
//
// Author: Eliot Muir
// Date:   Wednesday, November 24th, 2010 @ 01:19:34 PM
//---------------------------------------------------------------------------

#include <COL/COLarray.h>
class TRNargRequest;

void TRNargumentPopulate(const COLvar& ArgRequestData, TRNargRequest* pRequest);
void TRNargumentRequestParse(const COLvar& JsonArray, COLarray<TRNargRequest>* pFullArgumentRequests);

#endif // end of defensive include
