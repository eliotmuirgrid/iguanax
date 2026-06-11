#ifndef IFWARE_MAIN2_INST_INSTAPI_H_
#define IFWARE_MAIN2_INST_INSTAPI_H_
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: INSTapi.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   6/15/23 11:46 AM
//  ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

class DBDworld;
class SDMserver;
class SCKloop;

void INSTapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);
void INSTapiNormal(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);
void INSTapiReplace(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, DBDworld* pWorld);
void INSTapiAsync(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SDMserver* pServer, SCKloop* pLoop);

#endif // end of defensive include
