#ifndef __INST_STOP_H__
#define __INST_STOP_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTstop
//
// Description:
//
// Experimental routine to stop the installer
//
// Author: Eliot Muir 
// Date:   Wednesday 12 July 2023 - 08:13AM
// ---------------------------------------------------------------------------

class COLwebRequest;
class SDMserver;
class SCKloop;

void INSTstop(const COLwebRequest& Request, SDMserver* pServer, SCKloop* pLoop);

void INSTstartServiceAfterShutdown();

#endif // end of defensive include
