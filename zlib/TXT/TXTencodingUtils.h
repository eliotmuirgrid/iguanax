#ifndef __TXT_ENCODING_UTILS_H__
#define __TXT_ENCODING_UTILS_H__
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

#include <COL/COLminimumInclude.h>
#include <COL/COLauto.h>


class COLstring;
template <typename A1, typename A2> class COLclosure2;

void TXTnormalizeUtf8(const COLstring& Original, COLstring* pNormal);


#endif // end of defensive include
