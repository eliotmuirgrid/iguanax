#ifndef __TRN_ARGUMENT_RESPONSE_H__
#define __TRN_ARGUMENT_RESPONSE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNargumentResponse
//
// Description:
//
// Code for setting the response data for an argument and for writing it out to a COLvar.
//
// Author: Vismay Shah 
// Date:   Monday 01 May 2023 - 01:19PM
// ---------------------------------------------------------------------------
#include "TRNargumentRequest.h"

void TRNargumentPopulateResponse  (TRNarg& Arg, TRNargRequest& Request);
void TRNargumentWriteResponses(const COLarray<TRNargRequest>& FullArgumentRequests, COLvar* pResponse);

#endif // end of defensive include
