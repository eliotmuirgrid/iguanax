#ifndef __IWEB_SERVER_H__
#define __IWEB_SERVER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBserver
//
// Description:
//
// Init (start) web server and restart web server
//
// Author: Shuo Hou
// Date:   Wed 03 March 2021 11:12:04 EST
//---------------------------------------------------------------------------
#include <COL/COLweb.h>

class IWEBconfig;
class WEBserver;
class SCKloop;

void IWEBportChangeInfo(COLostream& Out);

bool IWEBserverInit(WEBserver& WEBserver,IWEBconfig& WebConfig, bool Quiet, COLstring* pError);

#endif // end of defensive include
