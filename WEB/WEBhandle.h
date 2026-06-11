#ifndef __WEB_HANDLE_H__
#define __WEB_HANDLE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBhandle
//
// Description:
//
// New path to handle web requests
//
// Author: Eliot Muir 
// Date:   Friday 19 May 2023 - 02:21PM
// ---------------------------------------------------------------------------

class WEBrequestData;
class COLstring;

void WEBrequestParse(const COLstring& Input, WEBrequestData* pOut);

#endif // end of defensive include
