#ifndef __DBD_SESSION_H__
#define __DBD_SESSION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDsession
//
// Description:
//
// Set up bindings for session handling - we pull stuff from WEB and from USR library for this
//
// Author: Eliot Muir 
// Date:   Monday 19 September 2022 - 07:06PM
// ---------------------------------------------------------------------------

#include <USR/USRmap.h>

class WEBserver;

void DBDcreateSession(COLmap<COLstring, USRuser>*  pUserMap, WEBserver* pWebServer);

#endif // end of defensive include
