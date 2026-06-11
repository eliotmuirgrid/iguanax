#ifndef __DBD_MODE_INITIALIZATION_H__
#define __DBD_MODE_INITIALIZATION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDmodeInitialization
//
// Description:
//
// The INITIALIZATION mode for Iguana. Once done, calls the NORMAL mode and 
// sets all the apis. 
//
// Author: Vismay Shah 
// Date:   Monday 17 July 2023 - 08:50AM
// ---------------------------------------------------------------------------
class DBDworld;

void DBDmodeInitialization(DBDworld* pWorld);

#endif // end of defensive include
