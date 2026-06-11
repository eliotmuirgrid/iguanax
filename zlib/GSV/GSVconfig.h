#ifndef __GSVCONFIG_H__
#define __GSVCONFIG_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GSVconfig.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   04/07/24 1:44 PM
//  ---------------------------------------------------------------------------
#include <COL/COLstring.h>
class COLvar;

class GSVconfig {
  public:
   GSVconfig() = default;
   GSVconfig(const COLstring& Username, const COLstring& Token, const COLstring& ServerUrl)
       : m_Token(Token), m_Username(Username), m_ServerUrl(ServerUrl) {};
   COLstring m_Token;
   COLstring m_Username;
   COLstring m_Email;
   COLstring m_ServerUrl;
   bool      m_Local = false;
};

COLostream& operator<<(COLostream& Stream, const GSVconfig& Config);

bool GSVisDefaultHost(const COLstring& Host);
void GSVsetEmailFromConfig(const COLstring& Username, GSVconfig* pConfig);
bool GSVconfigLoadUser(const COLstring& Username, GSVconfig* pConfig);
bool GSVconfigSaveUser(const COLstring& Username, const GSVconfig& Data, COLstring* pError);
COLvar GSVconfigToVar(const GSVconfig& Config, bool EncryptToken);

#endif  //__GSVCONFIG_H__
