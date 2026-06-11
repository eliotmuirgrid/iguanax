#ifndef __NOX_UTILS_H__
#define __NOX_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOXutils
//
// Description:
//
// NOXutils class.
//
// Author: Kevin Senn
// Date:   Tuesday, July 26th, 2011 @ 12:12:23 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

struct XMLexpatParserError;

void NOXrethrowExceptionWithSource(const XMLexpatParserError& Error, const COLstring& XmlSource);

#endif // end of defensive include
