//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNETnet
//
// Description:
//
// Implementation
//
// Author: Eric Mulvaney
// Date:   Monday, June 13th, 2011 @ 05:53:28 PM
//
//---------------------------------------------------------------------------
#include "LNETnet.h"

#include <COL/COLlog.h>
#include <LNET/LNETcurlFtp.h>
#include <LNET/LNEThttp.h>
#include <LNET/LNETsmtp.h>
#include <LNET/LNETtcp.h>
#include <LNET/LNETtcpAsync.h>
#include <LUA/LUAutils.h>
COL_LOG_MODULE;


void LNETaddNetNamespace(lua_State* L){
   COL_FUNCTION(LNETaddNetNamespace);
   lua_newtable(L); //net
      LNETaddHttpModule(L);
      LNETaddSmtpModule(L);
      LNETaddFtpModule(L);
      LNETaddTcpModule(L);
      LNETaddTcpAsyncModule(L);
   lua_setglobal(L, LNET_NAMESPACE);
}