#ifndef __IWEB_SERVER_SETTINGS_PORT_H__
#define __IWEB_SERVER_SETTINGS_PORT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBserverSettingsPort
//
// Description:
//
// Web server settings page APIs: save settings, input validations
//
// Author: Shuo Hou
// Date:   Fri 19 Feb 2021 4:01:05 EDT
//---------------------------------------------------------------------------
class COLwebRequest;
class COLvar;
class IWEBconfig;

bool IWEBcheckPortRangeAndUse(int Port, const IWEBconfig& WebConfig, COLvar* pResult);
void IWEBserverPort(const COLwebRequest& Request, IWEBconfig* pWebConfig);

#endif // end of defensive include
