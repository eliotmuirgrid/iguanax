#ifndef __GITC_SET_SERVER_H__
#define __GITC_SET_SERVER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCsetServer
//
// Description:
//
// API for setting a custom server as the git host
//
// Author: Vismay Shah 
// Date:   Monday 10 June 2024 - 3:21PM
// ---------------------------------------------------------------------------
class COLwebRequest;

void GITCsetServer(const COLwebRequest& Request);

#endif // end of defensive include
