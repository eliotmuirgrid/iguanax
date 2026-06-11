//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: IWEBserverSettingsPort.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <IWEB/IWEBconfig.h>
#include <IWEBA/IWEBserverSettingsPort.h>
#include <PRT/PRTutils.h>
COL_LOG_MODULE;

bool IWEBcheckPortRangeAndUse(int Port, const IWEBconfig& WebConfig, COLvar* pResult) {
   COL_FUNCTION(IWEBcheckPortRangeAndUse);
   COLvar& Result = *pResult;
   if(Port > 65535 || Port <= 0) {
      Result["port_error"] = "Port should be in the range: 1 - 65535";
      return false;
   }
   COLstring  PortError;
   COLostream PortErrorStream(PortError);
   if(!WebConfig.webPort().equals(COLintToString(Port)) && PRTmainCheckPort(PortErrorStream, Port)) {
      Result["port_error"] = PortError;
      return false;
   }
   return true;
}

// /web/server/port
void IWEBserverPort(const COLwebRequest& Request, IWEBconfig* pWebConfig) {
   COL_FUNCTION(IWEBserverPort);
   const COLvar& Input = Request.Data;
   COL_VAR(Input.json(1));
   COLvar Result;
   int    Port = COLvarRequiredInt(Input, "port");
   if(IWEBcheckPortRangeAndUse(Port, *pWebConfig, &Result)) {
      return COLrespondSuccess(Request.Address, Result);
   } else {
      COL_VAR(Result.json(1));
      return COLrespondError(Request.Address, Result["port_error"]);
   }
}