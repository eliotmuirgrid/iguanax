#ifndef __USR_USER_H__
#define __USR_USER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRuser
//
// Description:
//
// POD for defining a user.
//
// Author: Matthew Sobkowski
// Date:   Mon 29 Mar 2021 11:13:50 EDT
//---------------------------------------------------------------------------
#include <COL/COLarray.h>

class COLvar;

class USRuser {
public:
   USRuser();
   USRuser(const COLstring& Username, const COLstring& Password, const COLarray<COLstring>& Roles);
   USRuser(const COLstring& Username, const COLstring& Password, const COLarray<COLstring>& Roles, bool IsExternal);
   ~USRuser();

   void             setUsername(const COLstring& Value) { m_Username = Value; }
   const COLstring& username() const { return m_Username; }

   void             setPassword(const COLstring& Value);
   void             setPlainPassword(const COLstring& Value) { m_Password = Value; }
   const COLstring& password() const { return m_Password; }

   void                       setRoles(const COLvar& TagVar);
   void                       setRoles(const COLarray<COLstring>& Value) { m_Roles = Value; }
   const COLarray<COLstring>& roles() const { return m_Roles; }

   COLvar toVar(bool withPassword) const;

   bool authenticate(const COLstring& Password) const;

   void setExternal() { m_IsExternalUser = true; }
   bool isExternalUser() const { return m_IsExternalUser; }

   void      setDisplayName(const COLstring& DisplayName) { m_DisplayName = DisplayName; }
   COLstring displayName() const { return m_DisplayName; }

   void setProvider(const COLstring& Provider) { m_Provider = Provider; }
   COLstring provider() const { return m_Provider; }

  private:
   bool                m_IsExternalUser = false;
   COLstring           m_Username;
   COLstring           m_Password;
   COLstring           m_DisplayName;
   COLstring           m_Provider = "Local";
   COLarray<COLstring> m_Roles;
};

void USRmodifyVarForSave(COLvar& Data);

COLostream& operator<<(COLostream& Stream, const USRuser& User);

#endif // end of defensive include
