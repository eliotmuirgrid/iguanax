//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBidentity
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thu 20 Aug 2015 08:25:49 EDT
//---------------------------------------------------------------------------
 
#include "WEBidentity.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

void WEBidentity::setAuthenticated(bool Value){
   COL_METHOD(WEBidentity::setAuthenticated);
   m_IsAuthenticated = Value;
   COL_VAR(Value);
}

void WEBidentity::setIsAdmin(bool Value){
   COL_METHOD(WEBidentity::setIsAdmin);
   m_IsAdmin = Value;
   COL_VAR(Value);
}

void WEBidentity::setUserName(const COLstring& Name){
   COL_METHOD(WEBidentity::setUserName);
   m_UserName = Name;
   COL_VAR(m_UserName);
}

void WEBidentity::setSessionId(const COLstring& SessionId){
   COL_METHOD(WEBidentity::setSessionId);
   m_SessionId = SessionId;
   COL_VAR(m_SessionId);
}

void WEBidentity::setPermissions(const COLvar& Permissions){
   COL_METHOD(WEBidentity::setPermissions);
   m_Permissions = Permissions;
   COL_VAR(Permissions);
}

COLostream& operator<<(COLostream& Stream, const WEBidentity& Info){
   Stream << "Authenticated: " << Info.isAuthenticated() << " User=" << Info.userName() << " Session=" << Info.sessionId();
   return Stream;
}

