//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TXTencodingUtils
//
// Description: Simple filters for getting around OS filename weirdness
//
//
// Author: Bret Dawson
// Date:   5 Nov. 2015
//
//---------------------------------------------------------------------------
#include "TXTprecomp.h"
#pragma hdrstop

#include "TXTutils.h"
#include "TXTencoding.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <COL/COLstring.h>
#include <ctype.h>


void TXTnormalizeUtf8(const COLstring& Original, COLstring* pNormal) {
   COL_FUNCTION(TXTnormalizeUtf8);
   COLstring& Normal = *pNormal;

   Normal = Original;
   
   COL_VAR(COLtoAscii(Original, 'x'));
   COL_VAR(COLtoAscii(Normal, 'x'));
}

