#ifndef __WEB_SESSION_H__
#define __WEB_SESSION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: WEBsession
//
// Author: John Qi
//
// Description:
//
// Session handling logic.
//---------------------------------------------------------------------------
#include <ctime>
#include <COL/COLmap.h>
class COLvar;

class WEBsession {
public:
   WEBsession() {};
   WEBsession(const COLstring& Username, const COLstring& Id) : m_Username(Username), m_SessionId(Id), m_LastActivity(::time(NULL)) {}

   COLstring m_SessionId;  // TODO - questionable if we need m_SessionId here
   COLstring m_Username;
   time_t    m_LastActivity;

   // SSO specific session info
   time_t    m_LastValidation = 0;
   COLstring m_SsoProvider    = "None";
   COLstring m_SsoMode        = "Disabled";
};

COLostream& operator<<(COLostream& Stream, const WEBsession& Session);

// Creates a new session and returns the session ID
COLstring WEBcreateSession(COLmap<COLstring, WEBsession>* SessionMap, const COLstring& Username);

// Removes a session from map
void WEBdeleteSession(COLmap<COLstring, WEBsession>* pSessionMap, const COLstring& Id);
void WEBdeleteUserSession(COLmap<COLstring, WEBsession>* pSessionMap, const COLstring& Username);

void WEBsessionRemoveOld(COLmap<COLstring, WEBsession>* pSessionMap, int Timeout);

#endif // end of defensive include
