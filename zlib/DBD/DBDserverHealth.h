#ifndef __DBD_SERVER_HEALTH_H__
#define __DBD_SERVER_HEALTH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDserverHealth
//
// Description:
//
// Simple api that responds with { "status" : "up"}
//
// Author: Vismay Shah 
// Date:   Friday 14 November 2025 - 09:41AM
// ---------------------------------------------------------------------------
class COLwebRequest;

void DBDserverHealth(const COLwebRequest& Request);

#endif // end of defensive include
