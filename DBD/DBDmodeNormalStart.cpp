// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDmodeNormalStart
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday 25 May 2023 - 03:10PM
// ---------------------------------------------------------------------------
#include <DBD/DBDmodeNormalStart.h>
#include <DBD/DBDworld.h>

#include <SDM/SDMrestart.h>

#include <SCK/SCKloop.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBDmodeNormalStart(const COLwebRequest& Request, DBDworld* pWorld){
   COL_FUNCTION(DBDmodeNormalStart);
   COLcout << "Going to stop installer and start service." << newline;
   pWorld->pLoop->addTimer(COLnewClosure0(&SDMrequestRestart), 0);
   COLvar Data;
   Data["message"] = "Starting the initialization mode.";
   COLrespondSuccess(Request.Address, Data);
}