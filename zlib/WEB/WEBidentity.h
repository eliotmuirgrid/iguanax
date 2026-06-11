#ifndef __WEB_IDENTITY_H__
#define __WEB_IDENTITY_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBidentity
//
// Description:
//
// Decided that authentication is a core concept for a web server so making
// this class to handle it - it was TUIauthInfo.
//
// Author: Eliot Muir
// Date:   Thu 20 Aug 2015 08:24:49 EDT
//---------------------------------------------------------------------------

#include <COL/COLstring.h>
#include <COL/COLvar.h>

class WEBidentity{
public:
   WEBidentity() : m_IsAuthenticated(false){ }

   const COLstring& userName()    const { return m_UserName;   }
   const COLstring& sessionId()   const { return m_SessionId; }
   const COLvar& permissions()    const { return m_Permissions; }
   bool isAuthenticated()         const { return m_IsAuthenticated; }
   bool isAdmin()                 const { return m_IsAdmin; }

   void setAuthenticated(bool Value);
   void setIsAdmin(bool Value);
   void setUserName(const COLstring& Name);
   void setSessionId(const COLstring& SessionId);
   void setPermissions(const COLvar& Permissions);

private:
   COLstring m_UserName;
   COLstring m_SessionId;
   COLvar    m_Permissions;
   bool      m_IsAuthenticated;
   bool      m_IsAdmin;
};

COLostream& operator<<(COLostream& Stream, const WEBidentity& Info);

#endif // end of defensive include
