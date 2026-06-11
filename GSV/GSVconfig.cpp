//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GSVconfig.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   04/07/24 2:09 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GSV/GSVconfig.h>
#include <PASS/PASSword.h>
#include <INI/INIdata.h>
COL_LOG_MODULE;

COLostream& operator<<(COLostream& Stream, const GSVconfig& Config) {
   Stream << "Git Server: " << Config.m_ServerUrl << newline << "Git Username: " << Config.m_Username << newline
          << "Token: " << Config.m_Token;
   return Stream;
}

bool GSVisDefaultHost(const COLstring& Host) {
   return Host == "dev.azure.com" || Host == "ssh.dev.azure.com" || Host == "bitbucket.org" || Host == "github.com" ||
          Host == "gitlab.com";
}

static bool GSVparseConfig(const COLstring& Username, const COLstring& Content, GSVconfig* pConfig) {
   COL_FUNCTION(GSVparseConfig);
   COLvar Config;
   if(!Config.parse(Content)) { return false; }
   if(Config.exists("username")) { pConfig->m_Username = Config["username"].asString(); }
   if(Config.exists("gitHost"))  { pConfig->m_ServerUrl = Config["gitHost"].asString(); }
   if(Config.exists("email"))    { pConfig->m_Email = Config["email"].asString(); }
   else                          { GSVsetEmailFromConfig(Username, pConfig); }
   if(Config.exists("token")) {
      COLstring RawToken;
      PASSdecrypt(Config["token"], &RawToken);
      pConfig->m_Token = RawToken;
   }
   pConfig->m_Local = !GSVisDefaultHost(pConfig->m_ServerUrl);
   COL_VAR(*pConfig);
   return Config.exists("service") && (!Config.exists("gitHost") || Config["gitHost"].asString().is_null());
}

void GSVsetEmailFromConfig(const COLstring& Username, GSVconfig* pConfig) {
   COL_FUNCTION(GSVsetEmailFromConfig);
   const COLstring GitConfig = DIRconfigUserFile(Username);
   try {
      COLstring Content;
      FILreadFile(GitConfig, &Content);
      INIdata Data;
      INIparse(Content, &Data);
      COL_VAR(Data);
      pConfig->m_Email = INIdataKeyGet(Data, "user", "", "email");
      COL_VAR(pConfig->m_Email);
   } catch(const COLerror& E) { COL_TRC(E.description()); }
}

bool GSVconfigLoadUser(const COLstring& Username, GSVconfig* pConfig) {
   COL_FUNCTION(GSVconfigLoadUser);
   const COLstring FileName = DIRconfigUserGitFile(Username);
   if(!FILfileExists(FileName)) {
      COL_TRC("We have a fresh install of GIT with no " << FileName << " file.");
      return false;
   }
   COLstring Content, Error;
   try {
      FILreadFile(FileName, &Content);
   } catch(const COLerror& E) {
      COL_TRC(E.description());
      return false;
   }
   const bool SaveFile = GSVparseConfig(Username, Content, pConfig);
   if(SaveFile && !GSVconfigSaveUser(Username, *pConfig, &Error)) { COL_TRC(Error); }
   return true;
}

bool GSVconfigSaveUser(const COLstring& Username, const GSVconfig& Data, COLstring* pError) {
   COL_FUNCTION(GSVconfigSaveUser);
   const COLstring FileName = DIRconfigUserGitFile(Username);
   try {
      const COLvar Content = GSVconfigToVar(Data, true);
      FILwriteFile(FileName, Content.json(1));
      return true;
   } catch(const COLerror& Error) {
      *pError = Error.description();
      return false;
   }
}

COLvar GSVconfigToVar(const GSVconfig& Config, bool EncryptToken) {
   COL_FUNCTION(GSVconfigToVar);
   COLvar Out;
   Out["username"] = Config.m_Username;
   Out["email"]    = Config.m_Email;
   if(!Config.m_ServerUrl.is_null()) Out["gitHost"] = Config.m_ServerUrl;
   if(EncryptToken && !Config.m_Token.is_null()) {
      COLstring EncryptedToken;
      PASSencrypt(Config.m_Token, &EncryptedToken);
      Out["token"] = EncryptedToken;
   } else {
      Out["token"] = Config.m_Token;
   }
   return Out;
}