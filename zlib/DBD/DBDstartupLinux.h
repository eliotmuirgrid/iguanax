#ifndef __DBD_STARTUP_LINUX_H__
#define __DBD_STARTUP_LINUX_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDstartupLinux
//
// Description:
//
// Start up for linux.
//
// Author: Eliot Muir 
// Date:   Thursday 17 August 2023 - 10:46AM
// ---------------------------------------------------------------------------

class CMDlineParser;
class DBDworld;
class COLstring;

int DBDstartupLinux(const CMDlineParser& LineParser, const COLstring& WorkingDir, DBDworld* pWorld);

#endif // end of defensive include
