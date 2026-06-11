#ifndef __GITC_SERVER_H__
#define __GITC_SERVER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCserver
//
// Description:
//
// Api to retrieve the server for connecting to the git host
//
// Author: Vismay Shah 
// Date:   Monday 10 June 2024 - 3:21PM
// ---------------------------------------------------------------------------
class COLwebRequest;

void GITCserver(const COLwebRequest& Request);

#endif // end of defensive include
