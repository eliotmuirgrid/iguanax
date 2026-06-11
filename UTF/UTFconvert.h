#ifndef __UTF_CONVERT_H__
#define __UTF_CONVERT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UTFconvert
//
// Description:
//
// UTFconvert class.
//
// Author: Dmitri Klimenko
// Date:   Tuesday, February 10th, 2009 @ 03:23:42 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include "ConvertUTF.h"

class COLstring;
class UTFstringWchar;

const char* UTFconversionResultToString(ConversionResult Result);

void UTFconvertUtf8ToWchar(UTFstringWchar& WcharResult, const COLstring& UtfSource);

COLstring UTFconvertWcharToUtf8(const UTFstringWchar& Source);

#endif // end of defensive include
