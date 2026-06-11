//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBlogout
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thu  1 Sep 2022 12:00:18 EDT
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <SDB/SDBcapi.h>
#include <USR/USRuser.h>
#include <WEB/WEBlogin.h>
#include <WEB/WEBlogout.h>
#include <WEB/WEBresponse.h>
#include <WEB/WEBsession.h>
COL_LOG_MODULE;

static void WEBremoveExternalUserFromMap(COLmap<COLstring, USRuser>* pUserMap, const COLstring& Username, COLstring& Name){
   COL_FUNCTION(WEBremoveExternalUserFromMap);
   if ((pUserMap->count(Username)) && (*pUserMap)[Username].isExternalUser()){
      COL_TRC("The user logging out was an externally authenticated user - remove from user map");
      Name = (*pUserMap)[Username].displayName();
      pUserMap->removeKey(Username);
   }
   else { Name = Username;}
}

// /session/logout
void WEBlogout(const COLwebRequest& RequestData, COLmap<COLstring, WEBsession>* pSessionMap, COLmap<COLstring, USRuser>* pUserMap) {
   COL_FUNCTION(WEBlogout);
   COLstring Username;
   WEBremoveExternalUserFromMap(pUserMap, RequestData.User, Username);
   pSessionMap->removeKey(RequestData.SessionId);
   COLvar Result;
   Result["success"] = true;
   if (RequestData.Address.pInternalResponseCallback) {
      COL_TRC("This is an internal request from SCKcall().");
      RequestData.Address.pInternalResponseCallback->runAndDelete(Result);
      return;
   }
   SDBIlogService("User '" + Username + "' has logged out.", "#login");
   COLstring Out;
   WEBresponseWithIguanaSessionCookie(Result, "", RequestData.IsSecure, &Out);
   WEBresponseSend(RequestData.Address.ConnectionId, Out);
}