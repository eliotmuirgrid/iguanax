#ifndef __DBD_STARTUP_APPLE_H__
#define __DBD_STARTUP_APPLE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDstartupApple
//
// Description:
//
// Start up for Apple - each OS is quite different so this is easier to maintain.
//
// Author: Eliot Muir 
// Date:   Thursday 17 August 2023 - 10:47AM
// ---------------------------------------------------------------------------

class CMDlineParser;
class COLstring;
class DBDworld;

int DBDstartupApple(const CMDlineParser& LineParser, const COLstring& WorkingDir, DBDworld* pWorld);

#endif // end of defensive include
