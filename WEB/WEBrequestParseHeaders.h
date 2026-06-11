#ifndef __WEB_REQUEST_PARSE_HEADERS_H__
#define __WEB_REQUEST_PARSE_HEADERS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBrequestParseHeaders
//
// Description:
//
// Broken out for tracing
//
// Author: Eliot Muir 
// Date:   Thursday 26 October 2023 - 04:34PM
// ---------------------------------------------------------------------------

#include <COL/COLmap.h>

class COLstring;

bool WEBrequestParseHeaders(const char* pStart, const char* pEnd, COLmap<COLstring, COLstring>* pHeaders);

#endif // end of defensive include
