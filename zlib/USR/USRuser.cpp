//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRuser
//
// Description:
//
// Implementation
//
// Author: Matthew Sobkowski
// Date:   Mon 29 Mar 2021 11:13:50 EDT
//---------------------------------------------------------------------------

#include "USRuser.h"

#include <COL/COLlog.h>
#include "COL/COLvar.h"
#include <GUID/GUIDid.h>
#include <SFI/SFIhexEncodeFilter.h>
#include <openssl/evp.h>

COL_LOG_MODULE;

// Explicit constructors so we can track lifetime
USRuser::USRuser(){
   COL_METHOD(USRuser::USRuser);
}

USRuser::USRuser(const COLstring& Username, const COLstring& Password, const COLarray<COLstring>& Roles)
: m_Username(Username), m_Roles(Roles) {
   COL_METHOD(USRuser::USRuser);
   setPassword(Password); 
}

USRuser::USRuser(const COLstring& Username, const COLstring& Password,  const COLarray<COLstring>& Roles, bool IsExternal)
: m_Username(Username), m_Roles(Roles), m_IsExternalUser(IsExternal) {
   COL_METHOD(USRuser::USRuser-External);
   setPassword(Password); 
}

USRuser::~USRuser(){
   COL_METHOD(USRuser::~USRuser);
}


static const int MON_PASSWORD_SALT_LENGTH = 64;

static COLstring USRhashPassword(const COLstring& Password, COLstring& Salt) {
   COL_FUNCTION(USRhashPassword);
   COLstring Hash;
   Hash.setCapacity(EVP_MAX_KEY_LENGTH);
   Hash.setSize(EVP_MAX_KEY_LENGTH);
   auto* pHashBuffer = (unsigned char*) Hash.get_buffer();
   PKCS5_PBKDF2_HMAC(Password.c_str(), Password.size(), (unsigned char*) Salt.get_buffer(), Salt.size(), 1000, EVP_sha256(), EVP_MAX_KEY_LENGTH, pHashBuffer);
   return Hash;
}

void USRuser::setRoles(const COLvar& TagVar) {
   COL_METHOD(USRuser::setRoles);
   if(!TagVar.isArray()) {
      COL_TRC("Tag var must be array type");
      return;
   }
   m_Roles.clear();
   for(const auto& it : TagVar.array()) { m_Roles.push_back(it); }
}

void USRuser::setPassword(const COLstring& Value) {
   COL_METHOD(USRuser::setPassword);
   COLstring Salt = GUIDnewId(MON_PASSWORD_SALT_LENGTH);
   COLstring Hash = USRhashPassword(Value, Salt);
   Salt.append(SFIhexEncode(Hash.get_buffer(), Hash.size()));
   m_Password = Salt;
}

bool USRuser::authenticate(const COLstring& Password) const {
   COL_METHOD(USRuser::authenticate);
   COLstring Salt;
   COLstring Hash;
   COLstring CheckHash;
   m_Password.splitAtIndex(Salt, Hash, MON_PASSWORD_SALT_LENGTH);
   CheckHash = USRhashPassword(Password, Salt);
   COLstring CheckHashHex = SFIhexEncode(CheckHash.get_buffer(), CheckHash.size());
   COL_VAR3(m_Username, Hash, CheckHash);
   return Hash == CheckHashHex;
}
COLvar USRuser::toVar(const bool withPassword) const {
   COL_METHOD(USRuser::toVar);
   COLvar UserVar;
   UserVar["roles"].setArrayType();
   UserVar["username"]     = m_Username;
   UserVar["display_name"] = m_DisplayName;
   UserVar["provider"]     = m_Provider;
   UserVar["external"]     = isExternalUser();
   for(const auto& it : m_Roles) { UserVar["roles"].push_back(it); }
   if(withPassword) { UserVar["password"] = m_Password; }
   return UserVar;
}

void USRmodifyVarForSave(COLvar& Data){
   COL_FUNCTION(USRmodifyVarForSave);
   if(Data.exists("external")) {
      Data.erase("external"); // don't want to serialize this one
   }
}

COLostream& operator<<(COLostream& Stream, const USRuser& User){
   COLstring Roles;
   for(const auto& it : User.roles()) { Roles.append(it + " "); }
   Stream << "USER: " << "username=" << User.username() <<" description=" << Roles;
   return Stream;
}