#ifndef __IWEB_SERVER_SETTINGS_TIMEOUT_H__
#define __IWEB_SERVER_SETTINGS_TIMEOUT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBserverSettingsTimeout
//
// Description:
//
// Web server settings page APIs: save settings, input validations
//
// Author: Shuo Hou
// Date:   Fri 19 Feb 2021 4:01:05 EDT
//---------------------------------------------------------------------------
class COLwebRequest;

void IWEBcheckSessionTimeout(const COLwebRequest& Request);

#endif // end of defensive include
