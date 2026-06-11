//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEButils
//
// Description:
//
// Implementation
//
// Author: Shuo Hou
// Date:   Monday, March 15th, 2021 @ 03:03:15 PM
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <IWEB/IWEBconfig.h>
#include <IWEB/IWEButils.h>
COL_LOG_MODULE;

COLvar IWEBsetupConfigForSave(const IWEBconfig& Config) {
   COL_FUNCTION(IWEBsetupConfigForSave);
   COLvar WebConfigVar;
   WebConfigVar["description"] = Config.serverDescription();
   WebConfigVar["color"]       = Config.serverColour();
   WebConfigVar["guid"]        = Config.uniqueId();
   WebConfigVar["port"]        = Config.webPort();
   // WebConfigVar["session_timeout"] = (int) Config.webSessionTimeout();
   WebConfigVar["secure"]      = Config.useHttps();
   WebConfigVar["cert_key"]    = Config.httpsCertificateKeyFile();
   WebConfigVar["private_key"] = Config.httpsPrivateKeyFile();
   return WebConfigVar;
}
