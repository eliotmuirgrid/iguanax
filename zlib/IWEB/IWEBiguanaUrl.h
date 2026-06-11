#ifndef __IWEB_URL_H__
#define __IWEB_URL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBiguanaUrl
//
// Description:
//
// This is to get the URL for web server (running or not)
//
// Author: Shuo Hou
// Date:   Thu 11 March 2021 2:01:05 EDT
//---------------------------------------------------------------------------

class IWEBconfig;

#include <COL/COLstring.h>

// Use the loopback IP address instead of localhost because MacOS has issues resolving it from time to time. IX-2931
COLstring IWEBiguanaUrl(const IWEBconfig& WebConfig, const COLstring& Host="127.0.0.1");
COLstring IWEBiguanaUrl(const COLstring& Host="127.0.0.1");

void IWEBoutputListeningAndHelp();

#endif // end of defensive include
