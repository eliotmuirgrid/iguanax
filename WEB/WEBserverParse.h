#ifndef __WEB_SERVER_PARSE_H__
#define __WEB_SERVER_PARSE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBserverParse
//
// Description:
//
// Parsing routines for incoming HTTP requests.
//
// Author: Eliot Muir 
// Date:   Monday 22 May 2023 - 04:06PM
// ---------------------------------------------------------------------------

class COLstring;
class WEBrequestData;

bool WEBserverParse(COLstring* pBuffer, WEBrequestData* pRequestData, int* pTotalSize=nullptr);

#endif // end of defensive include
