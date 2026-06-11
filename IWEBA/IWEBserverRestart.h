#ifndef __IWEB_SERVER_RESTART_H__
#define __IWEB_SERVER_RESTART_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBserverRestart
//
// Description:
//
// Restart API for web server
//
// Author: Eliot Muir 
// Date:   Sunday 18 September 2022 - 06:49PM
// ---------------------------------------------------------------------------
class COLwebRequest;
class IWEBconfig;
class WEBserver;
class SCKloop;

void IWEBserverRestart(const COLwebRequest& Request, WEBserver* pWebServer, IWEBconfig* pWebConfiguration, SCKloop* pLoop);

#endif // end of defensive include
