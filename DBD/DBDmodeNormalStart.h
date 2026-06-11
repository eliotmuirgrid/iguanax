#ifndef __DBD_MODE_NORMAL_START_H__
#define __DBD_MODE_NORMAL_START_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDmodeNormalStart
//
// Description:
//
// Function to turn on the normal mode after GIT setup and a user have been created.
//
// Author: Eliot Muir
// Date:   Thursday 25 May 2023 - 03:10PM
// ---------------------------------------------------------------------------
class COLwebRequest;
class DBDworld;

void DBDmodeNormalStart(const COLwebRequest& Request, DBDworld* pWorld);

#endif // end of defensive include
