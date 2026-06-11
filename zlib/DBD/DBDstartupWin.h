#ifndef __DBD_STARTUP_WIN_H__
#define __DBD_STARTUP_WIN_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDstartupWin
//
// Description:
//
// Start up for windows
//
// Author: Eliot Muir 
// Date:   Thursday 17 August 2023 - 10:46AM
// ---------------------------------------------------------------------------

class CMDlineParser;
class DBDworld;
class COLstring;

int DBDstartupWin(const CMDlineParser& LineParser, const COLstring& WorkingDir, DBDworld* pWorld,int argc, const char** argv);

#endif // end of defensive include
