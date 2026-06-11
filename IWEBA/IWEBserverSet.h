#ifndef __IWEB_SERVER_SET_H__
#define __IWEB_SERVER_SET_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBserverSet
//
// Description:
//
// Web server settings page APIs: save settings, input validations
//
// Author: Shuo Hou
// Date:   Fri 19 Feb 2021 4:01:05 EDT
//---------------------------------------------------------------------------
class COLstring;
class COLwebRequest;
class IWEBconfig;
class SCKloop;
class WEBserver;

bool IWEBserverSetSaveConfig(const IWEBconfig& WebConfig, COLstring* pError);
void IWEBserverSet(const COLwebRequest& Request, WEBserver* pServer, IWEBconfig* pWebConfig, SCKloop* pLoop);
void IWEBserverSetImpl(const COLwebRequest& Request, IWEBconfig* pWebConfig, SCKloop* pLoop);

#endif // end of defensive include
