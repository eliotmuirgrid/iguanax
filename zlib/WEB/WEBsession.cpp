//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: WEBsession
//
// Author: John Qi
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------
#include <COL/COLlist.h>
#include <COL/COLlog.h>
#include <GUID/GUIDid.h>
#include <SDB/SDBcapi.h>
#include <WEB/WEBsession.h>
COL_LOG_MODULE;

COLostream& operator<<(COLostream& Stream, const WEBsession& Session){
   Stream << "User=" << Session.m_Username << " SessionId=" << Session.m_SessionId << " time=" << Session.m_LastActivity;
   return Stream;
}


COLstring WEBcreateSession(COLmap<COLstring, WEBsession>* pSessionMap, const COLstring& Username) {
   COL_FUNCTION(WEBcreateSession);
   COLmap<COLstring, WEBsession>& SessionMap = *pSessionMap;
   COLstring Id = GUIDnewId();
   SessionMap.add(Id, WEBsession(Username, Id));
   COL_VAR(Id);
   return Id;
}

void WEBdeleteSession(COLmap<COLstring, WEBsession>* pSessionMap, const COLstring& Id) {
   COL_FUNCTION(WEBdeleteSession);
   COLmap<COLstring, WEBsession>& SessionMap = *pSessionMap;
   if ( SessionMap.count(Id) == 0) {
      COL_WRN("Session '" << Id << "' not found");
   } else {
      COL_TRC("Found session " << Id << " and removing it.");
      SessionMap.erase(Id);
      COLASSERT(SessionMap.count(Id) == 0);
   }
}

void WEBdeleteUserSession(COLmap<COLstring, WEBsession>* pSessionMap, const COLstring& Username) {
   COL_FUNCTION(WEBdeleteUserSession);
   COLmap<COLstring, WEBsession>& SessionMap = *pSessionMap;
   auto i = SessionMap.begin();
   while (i != SessionMap.end()) {
      if (Username == i.value().m_Username) {
         COL_TRC("Removing session for user " << Username);
         i = pSessionMap->erase(i);
      } else {
         i++;
      }
   }
}

void WEBsessionRemoveOld(COLmap<COLstring, WEBsession>* pSessionMap, int Timeout){
   COL_FUNCTION(WEBsessionRemoveOld);
   COL_VAR(Timeout);
   COLmap<COLstring, WEBsession>& SessionMap = *pSessionMap;
   time_t Expiry = ::time(NULL) - Timeout;
   COL_TRC("Remove those less than " << Expiry);
   COLstring Log;
   COLostream LogStream(Log);
   COLlist<COLstring> RemoveList;
   for (auto i = SessionMap.begin(); i != SessionMap.end(); i++){
      COL_VAR2(i->first, i->second.m_LastActivity);
      if (i->second.m_LastActivity < Expiry){
         RemoveList.add(i->first);
         LogStream << i->second.m_Username<< newline;
      }
   }
   if (Log.size()) {
      SDBIlogService("Removed expired sessions for the following users:\n" + Log, "#login");
   }
   for (auto i = RemoveList.begin(); i != RemoveList.end(); i++){
      COL_TRC("Removing session: " << *i);
      SessionMap.erase(*i);
   }
}
