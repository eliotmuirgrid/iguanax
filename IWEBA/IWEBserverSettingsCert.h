#ifndef __IWEB_SERVER_SETTINGS_CERT_H__
#define __IWEB_SERVER_SETTINGS_CERT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBserverSettingsCert
//
// Description:
//
// Web server settings page APIs: save settings, input validations
//
// Author: Shuo Hou
// Date:   Fri 19 Feb 2021 4:01:05 EDT
//---------------------------------------------------------------------------
class COLwebRequest;
class COLstring;
class COLvar;

void IWEBserverCertificate(const COLwebRequest& Request);

#endif // end of defensive include
