#ifndef __DBDUP_IS_SERVICE_H__
#define __DBDUP_IS_SERVICE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDUPisService
//
// Description:
//
// Api to see if we are running as a service so that upgrades will work on Windows.
// Always returns true on posix.
//
// Author: Vismay Shah 
// Date:   Friday 18 August 2023 - 01:42PM
// ---------------------------------------------------------------------------
class COLwebRequest;

void DBDUPisService(const COLwebRequest& Request);

#endif // end of defensive include
