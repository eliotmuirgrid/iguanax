// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IPmine
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 26 September 2023 - 08:39AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <IP/IPmine.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>
COL_LOG_MODULE;

bool IPmine(COLstring* pAddress) {
   COL_FUNCTION(IPmine);
   WEBrequest  Request;
   WEBresponse Response;
   Request.Url = "https://api.ipify.org";
   WEBrequestBlockingGet(Request, &Response);
   COL_VAR2(Response.Body, Response.ReturnCode);
   if(Response.ReturnCode == 200) {
      *pAddress = Response.Body;
      return true;
   }
   *pAddress = "Unable to query public IP address";
   return false;
}