#ifndef __DBD_SHUTDOWN_H__
#define __DBD_SHUTDOWN_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDshutdown
//
// Author: Eliot Muir
//
// Description:
//
// Simple functional code to handle shutting down IguanaX cleanly.
//---------------------------------------------------------------------------
class ACTactionSet;
class DBDworld;
class SCKloop;
class SDMserver;
class SDMserver;

void DBDshutdown(DBDworld* pWorld);

void DBDbroadcastClusterShutdown(DBDworld* pWorld, ACTactionSet* pShutDownSeries);

void DBDterminateApp(DBDworld* pWorld, ACTactionSet* pShutDownSeries);


#endif // end of defensive include