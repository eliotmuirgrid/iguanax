#ifndef __IWEB_API_H__
#define __IWEB_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBapi
//
// Description:
//
// Web API for web server config.
//
// Author: Eliot Muir
// Date:   Sunday 18 September 2022 - 06:33PM
// ---------------------------------------------------------------------------

#include <COL/COLweb.h>
#include <COL/COLhashmap.h>
class IWEBconfig;
class SCKloop;
class WEBserver;

void IWEBapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, IWEBconfig* pWebConfig, WEBserver* pServer);

#endif // end of defensive include
