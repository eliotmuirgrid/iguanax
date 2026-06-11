#ifndef __DBD_MODE_API_H__
#define __DBD_MODE_API_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DBDmodeApi.h
//
//  Description: Api to determine the mode we are in
//
//  Author: Matthew Sobkowski
//  Date:   5/2/23 9:55 AM
//  ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

class DBDworld;

void DBDmodeApi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, DBDworld* pWorld);


#endif // end of defensive include
