//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNETcurlFtp
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   Monday, March 28th, 2011 @ 03:12:38 PM
//
//---------------------------------------------------------------------------
#include "LNETcurlFtp.h"
// #include "NODLenvironment.h"
// #include "NODLglobals.h"
#include <LUA/LUAtimeout.h>
#include <LUAL/LUALutils.h>
// #include "NODLtracing.h"

#include <new> // For placement new

#include <FIL/FILutils.h>
#include <FIL/FILfile.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAutils.h>
#include <LUA/LUAcheck.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAtable.h>

#include <NODL/NODLutils.h>
#include <LNET/LNETcurl.h>

// #include <COL/COLsimpleBuffer.h>
#include <COL/COLauto.h>
#include <COL/COLdateTime.h>
#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
#include <LUAL/LUALprint.h>
#include <SEC/SECaccess.h>
COL_LOG_MODULE;

#include <CURL/CURLftp.h>
#include <CURL/CURLutils.h>

//indicate the module names, also used for documentation
//and protocol verification
#define LNET_FTP_NAMESPACE "ftp"
#define LNET_SSH_FTP_NAMESPACE "sftp"
#define LNET_FTP_SSL_NAMESPACE "ftps"

//uppercase versions, for documentation
#define LNET_FTP_NAMESPACE_CAPS "FTP"
#define LNET_SSH_FTP_NAMESPACE_CAPS "SFTP"
#define LNET_FTP_SSL_NAMESPACE_CAPS "FTPS"

//these map to function names that are general to ftp/ftps/sftp
#define LNET_FTP_INIT "init"
#define LNET_FTP_LIST "list"
#define LNET_FTP_GET "get"
#define LNET_FTP_DELETE "delete"
#define LNET_FTP_RENAME "rename"
#define LNET_FTP_PUT "put"

#define LNET_THROW_BAD_FTP_URL(UrlParam,Protocol)\
   COL_ERROR_STREAM("Invalid URL " << (UrlParam) << ". URLs should begin with " <<Protocol<< ":// .\nIf not provided, "<< Protocol << ":// is assumed.",COLerror::SystemError);

const char* LNET_CURL_FTP_CACHE ="CurlFtpCache";
const char* LNET_CURL_FTP_CACHE_META_TABLE ="CurlFtpCacheMetaTable";

#define LNET_CURL_FTP_META_BASE "CurlFtpMeta_"
const char* LNET_FTP_META_TABLE = LNET_CURL_FTP_META_BASE LNET_FTP_NAMESPACE;
const char* LNET_SSH_FTP_META_TABLE = LNET_CURL_FTP_META_BASE LNET_SSH_FTP_NAMESPACE;
const char* LNET_FTP_SSL_META_TABLE = LNET_CURL_FTP_META_BASE LNET_FTP_SSL_NAMESPACE;

#define LNET_CURL_FTP_DEFAULT_TIMEOUT 15
#define LNET_CURL_FTP_DEFAULT_TIMEOUT_STR "15"
#define LNET_CURL_FTP_DEFAULT_FTP_PORT "21"
#define LNET_CURL_FTP_DEFAULT_SSH_FTP_PORT "22"
#define LNET_CURL_FTP_DEFAULT_FTP_SSL_PORT "990"

struct LNETenumMap_t { const char* pKey; int Value; };

#define LNET_CHECK_TIMEOUT_AND_RETURN(_Expression)\
   int Result = _Expression;\
   LUAcheckTimeout(L);\
   return Result;

class LNETcurlFtpItem{
public:
   LNETcurlFtpItem(const COLstring& iKey,
                   const COLstring& iProtocol,
                   bool iNoAct)
      : Ftp(LNET_CURL_FTP_DEFAULT_TIMEOUT, LNETcurlVerboseMode() != 0)
      , Key(iKey), Protocol(iProtocol), NoAct(iNoAct), m_Prequote(NULL)
   {
         touch();
   }

   ~LNETcurlFtpItem()
   {
      COL_METHOD(~LNETcurlFtpItem);
      if (m_Prequote){
         COL_TRC("Calling curl_slist_free_all() on m_Prequote");
         curl_slist_free_all(m_Prequote);
      }
   }

   const COLstring Key;
   const COLstring Protocol;
   const bool NoAct;  // If true, no destructive actions should be done.
   CURLftp Ftp;
   void touch(){
      COLcurrentTimeStamp (&m_LastAccessTimeStamp);
   }
   COLtimeStamp m_LastAccessTimeStamp;
   curl_slist*  m_Prequote;
};

typedef COLhashmap<COLstring, COLauto<LNETcurlFtpItem> > LNETcurlFtpCache;

void LNETcreateCurlFtpCache(lua_State* L){
   COL_FUNCTION(LNETcreateCurlFtpCache);
   const int StackTop = lua_gettop (L);
   LUApushString(L, LNET_CURL_FTP_CACHE);
   lua_gettable(L, LUA_REGISTRYINDEX);
   COLASSERT(lua_isnil(L, -1)); // curl ftp cache should not be in the registry
   lua_pop (L, 1); // Swallow the nil table pointer
   LUApushString(L, LNET_CURL_FTP_CACHE);
   LUApushObject<LNETcurlFtpCache>(L, LNET_CURL_FTP_CACHE_META_TABLE);
   lua_settable(L, LUA_REGISTRYINDEX);
   COLASSERT(StackTop == lua_gettop (L))
}


static LNETcurlFtpCache& LNETgetCurlFtpCache(lua_State* L){
   COL_FUNCTION(LNETgetCurlFtpCache);
   const int StackTop = lua_gettop (L);
   LUApushString(L, LNET_CURL_FTP_CACHE);
   lua_gettable(L, LUA_REGISTRYINDEX);
   COLASSERT(!lua_isnil(L, -1));
   void* UserData = LUAcheckUdata(L, -1, LNET_CURL_FTP_CACHE_META_TABLE,
      "`curl ftp cache' expected");
   lua_pop(L, 1); // Remove the table from the stack
   COLASSERT(StackTop == lua_gettop (L));
   return *(LNETcurlFtpCache*)UserData;
}

bool LNETcurlFtpValidateServer(const COLstring& Protocol, const COLstring& Url, COLstring& OutputServer )
{
   //only allow urls of form
   //<protocol>://abc
   //abc - assume this is just the server
   const COLstring Slashed = Protocol + "://";
   if (Url.find(Slashed) == 0){
      OutputServer = Url.substr( Slashed.size() );
   }
   else if (Url.find("://") != npos){
      return false;
   }
   else{
      OutputServer = Url;
   }
   return true;
}

static int LNETpushFtpKey(lua_State* L){
   lua_getfield(L, -1, "key");
   return 1;
}

//pushes id onto stack
static void LNETcurlFtpCreateFtpId(lua_State* L, const COLstring& Key, const char* pMetaTableName){
   COLint32 StackSize = lua_gettop(L);
   lua_newtable(L);
   luaL_getmetatable(L, pMetaTableName);
   lua_setmetatable(L, -2);

   LUApushString(L,Key);
   lua_setfield(L,-2,"key");

   COLPRECONDITION(StackSize+1 == lua_gettop(L));
}

class LNETftpParams {
   CURLftpContext m_FtpParams;
   COLstring m_Protocol;
   bool m_NoAct;
public:
   LNETftpParams(lua_State* L, COLuint16 DefaultPort, const char* pProtocol);
   const CURLftpContext& ftpContext() const { return m_FtpParams; }
   bool noAct() const { return m_NoAct; }
   COLstring makeKey() const;
};

LNETftpParams::LNETftpParams(lua_State* L, COLuint16 DefaultPort, const char* pProtocol)
{
   COLstring UrlParam = LUAexpectedStringParam(L, 1, "server");
   COLstring Server;
   if (!LNETcurlFtpValidateServer(pProtocol,UrlParam,Server)){
      LNET_THROW_BAD_FTP_URL(UrlParam,pProtocol);
   }
   COLstring UserName = LUAexpectedStringParam(L, 1, "username");
   COLstring Password = LUAoptionalStringParam(L, 1, "password", "");
   int Port = LUAoptionalIntParam(L, 1, "port", DefaultPort);

   m_Protocol = pProtocol;
   m_NoAct = LUALisEnvironmentTest(L)
      ? !LUAoptionalBoolParam(L, 1, "live", false)
      : false;
   if (LUALisEnvironmentStartup(L)) m_NoAct = true;
   m_FtpParams.setHost(Server);
   m_FtpParams.setPassword(Password);
   m_FtpParams.setUsername(UserName);
   m_FtpParams.setPort(Port);
}

//note that this isn't the complete key from ftps/sftp protocols
COLstring LNETftpParams::makeKey() const
{
   COLstring Out;
   COLostream OutStream(Out);
   OutStream << (m_NoAct ? "" : "live ") << m_Protocol << "://" << m_FtpParams.username()
      << '@' << m_FtpParams.host() << ':' << m_FtpParams.port();
   return Out;
}

//
// Utilities
//

static int
addString(lua_State* L, int, void* v)
{
   if(lua_type(L,-1) != LUA_TSTRING)
      return -1;
   curl_slist **p = (curl_slist**)v;
   *p = curl_slist_append(*p, lua_tostring(L,-1));
   return 0;
}

static curl_slist*
getStringList(lua_State* L, int i, const char* pOption)
{
   curl_slist *slist = NULL;
   try {
      if(!LUAisArray(L,i) || LUAwithIPairs(L, i, addString, &slist)) {
         LUA_ERROR_STREAM('\'' << pOption << "' must be an array of "
            "strings.");
      }
   } catch(...) {
      curl_slist_free_all(slist);
      throw;
   }
   return slist;
}

static void setIntOpt(lua_State* L, CURL* pCurl, CURLoption Opt,
                         const char* pName, int Default)
{
   int Val = LUAoptionalIntParam(L, 1, pName, Default);
   curl_easy_setopt(pCurl, Opt, Val);
}

static void setStringOpt(lua_State* L, CURL* pCurl, CURLoption Opt,
                         const char* pName)
{
   if(LUAoptionalStringParamPushed(L, 1, pName)) {
      curl_easy_setopt(pCurl, Opt, lua_tostring(L,-1));
      lua_pop(L,1);
   }
}

static void setBoolOpt(lua_State* L, CURL* pCurl, CURLoption Opt,
                       const char* pName, bool Default,
                       long TrueValue = 1L, long FalseValue = 0L)
{
   bool Value = LUAoptionalBoolParam(L, 1, pName, Default);
   curl_easy_setopt(pCurl, Opt, Value ? TrueValue : FalseValue);
}

static void setEnumOpt(lua_State* L, CURL* pCurl, CURLoption Opt,
                       const char* pName, const LNETenumMap_t aMap[])
{
   if(LUAoptionalStringParamPushed(L, 1, pName)) {
      const char* s = lua_tostring(L,-1);
      for(int i=0; aMap[i].pKey; i++) {
         if(!strcmp(s, aMap[i].pKey)) {
            curl_easy_setopt(pCurl, Opt, aMap[i].Value);
            lua_pop(L,1);
            return;
         }
      }
      LUA_ERROR_STREAM('\'' << s << "' is not a valid value for '"
         << pName << "'.");
   }
}


//
// Common and Shared FTP Options
//

// These apply to FTP(S) as well as Secure FTP.
#define BASIC_INIT_OPTIONS \
   "server", "username", "password", "port", "timeout", "live"

static int initGeneral(lua_State* L,
                       const char* pProtocol,  COLuint16   DefaultPort,
                       const char* pMetatable,
                       void (*initMore)(lua_State*, CURL*, LNETcurlFtpItem*))
{
   COL_FUNCTION(initGeneral);
   try {
      LNETcurlFtpCache& Cache = LNETgetCurlFtpCache(L);
      LNETftpParams FtpParams(L, DefaultPort, pProtocol);
      const COLstring& FtpKey = FtpParams.makeKey();
      LNETcurlFtpCache::TPlace FtpPlace = Cache.find(FtpKey);
      if(!FtpPlace) {
         FtpPlace = Cache.add(FtpKey,
            new LNETcurlFtpItem(FtpKey, pProtocol, FtpParams.noAct()));
      }
      LNETcurlFtpItem* pFtp = Cache[FtpPlace];
      const CURLftpContext& Options = FtpParams.ftpContext();
      CURL* pCurl =
         (CURL*)pFtp->Ftp.initBasic(pProtocol, Options.host(), Options.port());
      pFtp->Ftp.setUsername(Options.username());
      pFtp->Ftp.setPassword(Options.password());
      setIntOpt(L,pCurl,CURLOPT_TIMEOUT,"timeout",LNET_CURL_FTP_DEFAULT_TIMEOUT);
      initMore(L, pCurl, pFtp);
      pFtp->touch();
      LNETcurlFtpCreateFtpId(L, pFtp->Key, pMetatable);
      LUAcheckTimeout(L);
      return 1;
   }
   LUA_CATCH();
   return 0;
}

static const LNETenumMap_t LNETsslForce[] = {
   "yes",     CURLUSESSL_ALL,
   "no",      CURLUSESSL_TRY,
   "control", CURLUSESSL_CONTROL,
   NULL
};

static const LNETenumMap_t s_SslFtpAuth[] = {
   "ssl", CURLFTPAUTH_SSL,
   "tls", CURLFTPAUTH_TLS,
   NULL
};

static const LNETenumMap_t s_FtpPlainUseSsl[] = {
   "all",     CURLUSESSL_ALL,
   "try",     CURLUSESSL_TRY,
   "control", CURLUSESSL_CONTROL,
   NULL
};

// These options do not apply to Secure FTP.
#define ADVANCED_INIT_OPTIONS \
   "active", "no_eprt", "no_epsv", "skip_pasv_ip", \
   "use_pret", "alt_auth", "acct", "on_ready", \
   "use_ssl", \
   "certificate_file", "certificate_type", "private_key_file", "private_key_pass", "private_key_type", \
   "verify_peer", "verify_host", "ca_file", "issuer_cert", "crl_file", \
   "ssl_engine", "ssl_version", "cipher_list", "cipher_suite_list"

static void initAdvanced(lua_State* L, CURL* pCurl, LNETcurlFtpItem* pFtp)
{
   COL_FUNCTION(initAdvanced);

   int Top = lua_gettop(L);
   setStringOpt(L, pCurl, CURLOPT_FTPPORT, "active");
   setStringOpt(L, pCurl, CURLOPT_FTP_ACCOUNT, "acct");
   setStringOpt(L, pCurl, CURLOPT_FTP_ALTERNATIVE_TO_USER, "alt_auth");
   setBoolOpt(L, pCurl, CURLOPT_FTP_USE_EPRT, "no_eprt", false, 0, 1);
   setBoolOpt(L, pCurl, CURLOPT_FTP_USE_EPSV, "no_epsv", false, 0, 1);
   setBoolOpt(L, pCurl, CURLOPT_FTP_SKIP_PASV_IP, "skip_pasv_ip", false);
   setBoolOpt(L, pCurl, CURLOPT_FTP_USE_PRET, "use_pret", false);
   setEnumOpt(L, pCurl, CURLOPT_USE_SSL,    "use_ssl",   s_FtpPlainUseSsl);
   setStringOpt(L, pCurl, CURLOPT_SSLCERT,     "certificate_file");
   setStringOpt(L, pCurl, CURLOPT_SSLCERTTYPE, "certificate_type");
   setStringOpt(L, pCurl, CURLOPT_SSLKEY,      "private_key_file");
   setStringOpt(L, pCurl, CURLOPT_SSLKEYTYPE,  "private_key_type");
   setStringOpt(L, pCurl, CURLOPT_SSLENGINE,   "ssl_engine");
   setStringOpt(L, pCurl, CURLOPT_ISSUERCERT,  "issuer_cert");
   setStringOpt(L, pCurl, CURLOPT_CRLFILE,     "crl_file");

   LNETcurlSetSslCaBundle(L, pCurl);
   LNETcurlSetSslCipherSuiteList(L, pCurl);
   LNETcurlSetSslCipherList(L, pCurl);
   LNETcurlSetSslVersion(L, pCurl);

   // The key password must always be set, even when not specified, to
   // avoid prompting the user (at stdin) for a password if it is needed.
   curl_easy_setopt(pCurl, CURLOPT_KEYPASSWD,
      LUAoptionalStringParam(L, 1, "private_key_pass", "").c_str());

   setBoolOpt(L, pCurl, CURLOPT_SSL_VERIFYPEER, "verify_peer", true);
   setBoolOpt(L, pCurl, CURLOPT_SSL_VERIFYHOST, "verify_host", true, 2);
   if(LUAoptionalTableParam(L, 1, "on_ready")) {
      pFtp->m_Prequote = getStringList(L, -1, "on_ready");
      curl_easy_setopt(pCurl, CURLOPT_PREQUOTE, pFtp->m_Prequote);
      lua_pop(L, 1);
   }
   lua_settop(L, Top);
}

//
// Regular FTP
//

static const char* s_FtpInitOptions[] = {
   BASIC_INIT_OPTIONS,
   ADVANCED_INIT_OPTIONS,
   NULL
};

int LNETcurlFtpInitFtp(lua_State* L){
   COL_FUNCTION(LNETcurlFtpInitFtp);
   LUAcheckParams(L, 1, s_FtpInitOptions);
   return initGeneral(L, "ftp", atoi(LNET_CURL_FTP_DEFAULT_FTP_PORT),
      LNET_FTP_META_TABLE, initAdvanced);
}

//
// Secure FTP
//

static const char* s_SshFtpInitOptions[] = {
   BASIC_INIT_OPTIONS,
   "public_key_file", "private_key_file", "private_key_pass",
   "host_fingerprint", "known_hosts",
   NULL
};

void initSecureFtp(lua_State* L, CURL* pCurl, LNETcurlFtpItem* pFtp)
{
   int Top = lua_gettop(L);
   setStringOpt(L, pCurl, CURLOPT_SSH_PUBLIC_KEYFILE,  "public_key_file");
   setStringOpt(L, pCurl, CURLOPT_SSH_PRIVATE_KEYFILE, "private_key_file");
   setStringOpt(L, pCurl, CURLOPT_SSH_KNOWNHOSTS,      "known_hosts");
   // The key password must always be set, even when not specified, to
   // avoid prompting the user (at stdin) for a password if it is needed.
   curl_easy_setopt(pCurl, CURLOPT_KEYPASSWD,
      LUAoptionalStringParam(L, 1, "private_key_pass", "").c_str());
   // We don't "repair" a bad checksum, we just error out since Curl won't.
   if(LUAoptionalStringParamPushed(L, 1, "host_fingerprint")) {
      const char* s = lua_tostring(L,-1);
      if(lua_strlen(L,-1) != 32 || strspn(s, "0123456789abcdefABCDEF") != 32) {
         LUA_ERROR_STREAM("'host_fingerprint' must be a 128-bit MD5 "
            "checksum, exactly 32 hexadecimal characters.");
      }
      curl_easy_setopt(pCurl, CURLOPT_SSH_HOST_PUBLIC_KEY_MD5, s);
      lua_pop(L,1);
   }
   lua_settop(L, Top);
}

int LNETcurlFtpInitSshFtp(lua_State* L){
   COL_FUNCTION(LNETcurlFtpInitSshFtp);
   LUAcheckParams(L, 1, s_SshFtpInitOptions);
   return initGeneral(L, "sftp", atoi(LNET_CURL_FTP_DEFAULT_SSH_FTP_PORT),
      LNET_SSH_FTP_META_TABLE, initSecureFtp);
}

//
// FTP over SSL/TLS
//

static const char* s_FtpSslInitOptions[] = {
   BASIC_INIT_OPTIONS,
   ADVANCED_INIT_OPTIONS,
   "certificate_file", "certificate_type", "private_key_file",
   "private_key_pass", "private_key_type", "ssl_engine", "verify_peer",
   "verify_host", "ca_file", "issuer_cert", "crl_file", "ssl_version",
   "ssl_auth", "force_ssl", "use_ccc", "cipher_list", "cipher_suite_list",
   NULL
};

void initFtpSsl(lua_State* L, CURL* pCurl, LNETcurlFtpItem* pFtp)
{
   int Top = lua_gettop(L);
   initAdvanced(L, pCurl, pFtp);
   setStringOpt(L, pCurl, CURLOPT_SSLCERT,     "certificate_file");
   setStringOpt(L, pCurl, CURLOPT_SSLCERTTYPE, "certificate_type");
   setStringOpt(L, pCurl, CURLOPT_SSLKEY,      "private_key_file");
   setStringOpt(L, pCurl, CURLOPT_SSLKEYTYPE,  "private_key_type");
   setStringOpt(L, pCurl, CURLOPT_SSLENGINE,   "ssl_engine");
   setStringOpt(L, pCurl, CURLOPT_ISSUERCERT,  "issuer_cert");
   setStringOpt(L, pCurl, CURLOPT_CRLFILE,     "crl_file");

   // The key password must always be set, even when not specified, to
   // avoid prompting the user (at stdin) for a password if it is needed.
   curl_easy_setopt(pCurl, CURLOPT_KEYPASSWD,
      LUAoptionalStringParam(L, 1, "private_key_pass", "").c_str());
   curl_easy_setopt(pCurl, CURLOPT_USE_SSL, CURLUSESSL_ALL); // The default.
   setEnumOpt(L, pCurl, CURLOPT_USE_SSL,    "force_ssl",   LNETsslForce);
   setEnumOpt(L, pCurl, CURLOPT_FTPSSLAUTH, "ssl_auth",    s_SslFtpAuth);
   setBoolOpt(L, pCurl, CURLOPT_SSL_VERIFYPEER, "verify_peer", true);
   setBoolOpt(L, pCurl, CURLOPT_SSL_VERIFYHOST, "verify_host", true, 2);
   setBoolOpt(L, pCurl, CURLOPT_FTP_SSL_CCC,    "use_ccc",    false);

   LNETcurlSetSslCaBundle(L, pCurl);
   LNETcurlSetSslCipherSuiteList(L, pCurl);
   LNETcurlSetSslCipherList(L, pCurl);
   LNETcurlSetSslVersion(L, pCurl);

   lua_settop(L,Top);
}

int LNETcurlFtpInitFtpSsl(lua_State* L){
   COL_FUNCTION(LNETcurlFtpInitFtpSsl);
   LUAcheckParams(L, 1, s_FtpSslInitOptions);
   return initGeneral(L, "ftps", atoi(LNET_CURL_FTP_DEFAULT_FTP_SSL_PORT),
      LNET_FTP_SSL_META_TABLE, initFtpSsl);
}

//
// FTP Operations
//

//takes parameter from index, checks to see if its
//valid and returns it. throws if not found, or id isn't valid
LNETcurlFtpItem* LNETcurlFtpGetCacheItem(lua_State* L, int i, const char* pProtocol){
   const int StackTop = lua_gettop (L);
   LNETcurlFtpItem* pOut = 0;
   //ensure param is a table
   if (StackTop && lua_type(L,i) == LUA_TTABLE){
      COLstring Key;
      lua_getfield(L,i,"key");
      if (lua_type(L,-1) == LUA_TSTRING){
         Key = LUAtoString(L,-1);
         LNETcurlFtpCache& Cache = LNETgetCurlFtpCache(L);
         LNETcurlFtpCache::TPlace Found = Cache.find(Key);
         if (Found){
            pOut = Cache[Found];

            //this is to ensure that we didn't call the wrong method. For instance
            //calling ftp.get on what is actually a ftps connection
            if(pOut->Protocol != pProtocol) pOut = 0;
            else pOut->touch();
         }
      }
      lua_pop(L,1);
   }
   COLPRECONDITION(StackTop == lua_gettop (L));
   if (!pOut){
      if (StackTop == 0){
         //no params case
         LUA_ERROR_STREAM ("Missing " << pProtocol << " connection object.");
      }
      else{
         LUA_ERROR_STREAM ( "First parameter is not a valid " << pProtocol << " connection object.");
      }
   }
   return pOut;
}

static void LNETcurlFtpCheckCall(CURLftp::eResult Result){
   switch(Result){
   case CURLftp::CURL_FTP_OK:
   case CURLftp::CURL_FTP_OK_OVERWROTE_FILE:
      return;
      break;
   case CURLftp::CURL_FTP_CANNOT_CONNECT:
      COL_ERROR_STREAM("Unable to connect to server.",COLerror::SystemError);
      break;
   case CURLftp::CURL_FTP_TIMEOUT:
      COL_ERROR_STREAM("Timeout while communicating with server.",COLerror::SystemError);
      break;
   case CURLftp::CURL_FTP_REMOTE_FILE_EXISTS:
      COL_ERROR_STREAM("Remote file exists.",COLerror::SystemError);
      break;
   case CURLftp::CURL_FTP_INTERRUPTED:
      COL_ERROR_STREAM("Operation was interrupted.",COLerror::SystemError);
      break;
   case CURLftp::CURL_FTP_BUSY:
      COL_ERROR_STREAM("Server is busy.",COLerror::SystemError);
      break;
   default:
      COL_ERROR_STREAM("Unknown error with ftp/ftps/sftp operation.",COLerror::SystemError);
   }
}

static const char* s_ListOptions[] = { "remote_path", "raw", "debug", NULL };

//pushes result onto top of stack
void LNETcurlFtpMakeFileList(lua_State* L, const COLarray<CURLftpListDirEntry>& List){
   const int StackTop = lua_gettop (L);
   lua_createtable(L,List.size(),0);
   for (COLint32 ListIndex = 0; ListIndex < List.size(); ListIndex++){
      lua_newtable(L);
      lua_pushstring(L,List[ListIndex].FileName.c_str());
      lua_setfield(L,-2,"filename");
      lua_pushboolean(L, List[ListIndex].IsRetrievable );
      lua_setfield(L,-2,"is_retrievable");

      lua_pushinteger(L,ListIndex+1); //1 based key
      lua_pushvalue(L,-2); //new table value
      lua_settable(L,-4); //insert entry
      lua_pop(L,1);//pop out orig table
   }
   COLASSERT((StackTop+1) == lua_gettop (L))
}

int LNETcurlFtpListBase(lua_State* L, const char* pProtocol){
   COL_FUNCTION(LNETcurlFtpListBase);
   try{
      LUAcheckParams(L, 2, s_ListOptions);
      CURLdebugLog DebugLog;
      LNETcurlFtpItem* pFtp = LNETcurlFtpGetCacheItem(L, 1, pProtocol);
      COLstring RemotePath = LUAexpectedStringParam(L, 2, "remote_path");
      if(LUAoptionalBoolParam(L, 2, "debug", false)) {
         DebugLog.trace((CURL*)pFtp->Ftp.curlHandle());
      }

      // Iguana can't parse the output of every SFTPS server out there (especially Windows ones).
      // The `raw` option gives user the chance to parse it themselves in Translator.
      bool RawOutput= LUAoptionalBoolParam(L, 2, "raw", false);
      try
      {
         COLstring RawResults;
         COLarray<CURLftpListDirEntry> Results;
         COLstring* pRawResults = RawOutput ? &RawResults : NULL;
         LNETcurlFtpCheckCall(pFtp->Ftp.listDirectory(RemotePath,Results,pRawResults));
         if (RawOutput) {
            LUApushString(L, RawResults);
         } else {
            LNETcurlFtpMakeFileList(L,Results);
         }
      }
      catch(const COLerror& Error){
         LNETcurlAppendDebugLog(DebugLog,Error);
      }
      pFtp->touch();

      if(DebugLog.tracing()) {
         LUApushString(L, DebugLog.Output);
         return 2;
      }
      return 1;
   }
   LUA_CATCH();
   return 0;
}

int LNETcurlFtpListDirectoryFtp(lua_State* L){
   LNET_CHECK_TIMEOUT_AND_RETURN(LNETcurlFtpListBase(L, LNET_FTP_NAMESPACE));
}

int LNETcurlFtpListDirectorySshFtp(lua_State* L){
   LNET_CHECK_TIMEOUT_AND_RETURN(LNETcurlFtpListBase(L, LNET_SSH_FTP_NAMESPACE));
}

int LNETcurlFtpListDirectoryFtpSsl(lua_State* L){
   LNET_CHECK_TIMEOUT_AND_RETURN(LNETcurlFtpListBase(L, LNET_FTP_SSL_NAMESPACE));
}

static const char* s_GetOptions[] = {
   "remote_path", "local_path", "overwrite", "debug", NULL
};

int LNETcurlFtpGetBase(lua_State* L, const char* pProtocol){
   COL_FUNCTION(LNETcurlFtpGetBase);
   try{
      LUAcheckParams(L, 2, s_GetOptions);
      CURLdebugLog DebugLog;
      LNETcurlFtpItem* pFtp = LNETcurlFtpGetCacheItem(L, 1, pProtocol);
      COLstring RemotePath = LUAexpectedStringParam(L, 2, "remote_path");
      COLstring LocalPath = LUAoptionalStringParam(L, 2, "local_path", "");
      bool Overwrite = LUAoptionalBoolParam(L, 2, "overwrite", false);
      if(LUAoptionalBoolParam(L, 2, "debug", false)) {
         DebugLog.trace((CURL*)pFtp->Ftp.curlHandle());
      }
      bool LocalFileExists = FILfileExists(LocalPath);
      COLauto<COLsink> pSink;
      FILfile* pFile = NULL;
      COLsinkString* pBufferOut = NULL;
      if (LocalPath.size()){
         if (!Overwrite && LocalFileExists){
            COL_ERROR_STREAM("File '" << LocalPath << "' exists, and overwrite is set to false.",COLerror::SystemError);
         }
      	COLstring Err;
      	if(!SECvalidate(LocalPath, &Err)) {
      		return luaL_error(L, "Local path must be within the working directory");
      	}
         pSink = pFile = new FILfile(LocalPath, FILfile::Rewrite);
      }
      else{
         pBufferOut = new COLsinkString();
         pSink = pBufferOut;
      }

      try{
         LNETcurlFtpCheckCall( pFtp->Ftp.getFile(RemotePath, *pSink) );
      } catch(const COLerror& Error) {
         if (FILfileExists(LocalPath) && !LocalFileExists) { // Delete the file here if we failed the curl operation AND the file did not exist already
            pFile->close();
            FILremove(LocalPath);
         }
         pFile = NULL; // set to null so line 716 stays safe
         LNETcurlAppendDebugLog(DebugLog,Error);
      }
      pFtp->touch();

      COLuint64 AmountRead = 0;
      if (pBufferOut) {
         lua_pushlstring(L, (const char*)pBufferOut->string().data(), pBufferOut->string().size());
         AmountRead = pBufferOut->string().size();
      } else {
         lua_pushboolean(L,1);
         if(pFile) AmountRead = pFile->size();
      }

      if(DebugLog.tracing()) {
         LUApushString(L, DebugLog.Output);
         return 2;
      }
      return 1;
   }
   LUA_CATCH();
   return 0;
}

int LNETcurlFtpDownloadFileFtp(lua_State* L){
   LNET_CHECK_TIMEOUT_AND_RETURN(LNETcurlFtpGetBase(L, LNET_FTP_NAMESPACE));
}

int LNETcurlFtpDownloadFileSshFtp(lua_State* L){
   LNET_CHECK_TIMEOUT_AND_RETURN(LNETcurlFtpGetBase(L, LNET_SSH_FTP_NAMESPACE));
}

int LNETcurlFtpDownlaodFileFtpSsl(lua_State* L){
   LNET_CHECK_TIMEOUT_AND_RETURN(LNETcurlFtpGetBase(L, LNET_FTP_SSL_NAMESPACE));
}

static const char* s_DeleteOptions[] = { "remote_path", "debug", NULL };

int LNETcurlFtpDeleteBase(lua_State* L, const char* pProtocol){
   COL_FUNCTION(LNETcurlFtpDeleteBase);
   try{
      LUAcheckParams(L, 2, s_DeleteOptions);
      CURLdebugLog DebugLog;
      LNETcurlFtpItem* pFtp = LNETcurlFtpGetCacheItem(L, 1, pProtocol);
      COLstring RemotePath = LUAexpectedStringParam(L, 2, "remote_path");
      if(LUAoptionalBoolParam(L, 2, "debug", false)) {
         DebugLog.trace((CURL*)pFtp->Ftp.curlHandle());
      }

      if(!pFtp->NoAct) {
         try{
            LNETcurlFtpCheckCall( pFtp->Ftp.deleteFile(RemotePath) );
         }
         catch(const COLerror& Error){
            LNETcurlAppendDebugLog(DebugLog,Error);
         }
      }
      pFtp->touch();
      lua_pushboolean(L,1);

      if(DebugLog.tracing()) {
         LUApushString(L, DebugLog.Output);
         return 2;
      }
      return 1;
   }
   LUA_CATCH();
   return 0;
}

int LNETcurlFtpDeleteFileFtp(lua_State* L){
   LNET_CHECK_TIMEOUT_AND_RETURN(LNETcurlFtpDeleteBase(L, LNET_FTP_NAMESPACE));
}

int LNETcurlFtpDeleteFileSshFtp(lua_State* L){
   LNET_CHECK_TIMEOUT_AND_RETURN(LNETcurlFtpDeleteBase(L, LNET_SSH_FTP_NAMESPACE));
}

int LNETcurlFtpDeleteFileFtpSsl(lua_State* L){
   LNET_CHECK_TIMEOUT_AND_RETURN(LNETcurlFtpDeleteBase(L, LNET_FTP_SSL_NAMESPACE));
}

static const char* s_RenameOptions[] = {
   "remote_path", "new_remote_path", "debug", NULL
};

int LNETcurlFtpRenameBase(lua_State* L, const char* pProtocol){
   COL_FUNCTION(LNETcurlFtpRenameBase);
   try{
      LUAcheckParams(L, 2, s_RenameOptions);
      CURLdebugLog DebugLog;
      LNETcurlFtpItem* pFtp = LNETcurlFtpGetCacheItem(L, 1, pProtocol);
      COLstring RemotePath = LUAexpectedStringParam(L, 2, "remote_path");
      COLstring NewRemotePath = LUAexpectedStringParam(L, 2, "new_remote_path");
      if(LUAoptionalBoolParam(L, 2, "debug", false)) {
         DebugLog.trace((CURL*)pFtp->Ftp.curlHandle());
      }

      try
      {
         if(!pFtp->NoAct) {
            LNETcurlFtpCheckCall( pFtp->Ftp.renameFile(RemotePath,NewRemotePath) );
         }
      }
      catch(const COLerror& Error){
         LNETcurlAppendDebugLog(DebugLog,Error);
      }
      pFtp->touch();
      lua_pushboolean(L,1);

      if(DebugLog.tracing()) {
         LUApushString(L, DebugLog.Output);
         return 2;
      }
      return 1;
   }
   LUA_CATCH();
   return 0;
}

int LNETcurlFtpRenameFileFtp(lua_State* L){
   LNET_CHECK_TIMEOUT_AND_RETURN(LNETcurlFtpRenameBase(L, LNET_FTP_NAMESPACE));
}

int LNETcurlFtpRenameFileSshFtp(lua_State* L){
   LNET_CHECK_TIMEOUT_AND_RETURN(LNETcurlFtpRenameBase(L, LNET_SSH_FTP_NAMESPACE));
}

int LNETcurlFtpRenameFileFtpSsl(lua_State* L){
   LNET_CHECK_TIMEOUT_AND_RETURN(LNETcurlFtpRenameBase(L, LNET_FTP_SSL_NAMESPACE));
}

static const char* s_PutOptions[] = {
   "remote_path", "data", "local_path", "overwrite", "tmp_postfix", "debug",
   NULL
};

int LNETcurlFtpPutBase(lua_State* L, const char* pProtocol){
   COL_FUNCTION(LNETcurlFtpPutBase);
   try{
      LUAcheckParams(L, 2, s_PutOptions);
      CURLdebugLog DebugLog;
      LNETcurlFtpItem* pFtp = LNETcurlFtpGetCacheItem(L, 1, pProtocol);
      COLstring RemotePath = LUAexpectedStringParam(L, 2, "remote_path");
      if(LUAoptionalBoolParam(L, 2, "debug", false)) {
         DebugLog.trace((CURL*)pFtp->Ftp.curlHandle());
      }

      //fetch data as buffer
      const char* pLocalData = 0;
      size_t LocalDataSize = 0;
      if (LUAoptionalStringParamPushed(L, 2, "data")){
         pLocalData = lua_tolstring(L,-1,&LocalDataSize);
      }

      COLstring LocalPath = LUAoptionalStringParam(L, 2, "local_path", "");
      COLstring TmpExtension = LUAoptionalStringParam(L, 2, "tmp_postfix", "");
      if (!pLocalData && !LocalPath.size()){
         LUA_ERROR_STREAM ( "Either local_path or data must be specified.");
      }
      bool Overwrite = LUAoptionalBoolParam(L, 2, "overwrite", false);

      COLuint64 AmountSent = 0;

      try{
         if(pFtp->NoAct) {
            // Do nothing.
         } else if(LocalPath.size()) {
            LNETcurlFtpCheckCall(pFtp->Ftp.putFile(LocalPath,RemotePath,Overwrite,TmpExtension));
            try { AmountSent = FILfileSize(LocalPath.c_str()); } catch(...) {}
         } else {
            LNETcurlFtpCheckCall(pFtp->Ftp.putData(pLocalData,LocalDataSize, RemotePath, Overwrite,TmpExtension));
            AmountSent = LocalDataSize;
         }
      }
      catch(const COLerror& Error){
         LNETcurlAppendDebugLog(DebugLog,Error);
      }

      pFtp->touch();
      lua_pushboolean(L,1);

      if(DebugLog.tracing()) {
         LUApushString(L, DebugLog.Output);
         return 2;
      }
      return 1;
   }
   LUA_CATCH();
   return 0;
}

int LNETcurlFtpUploadFileFtp(lua_State* L){
   LNET_CHECK_TIMEOUT_AND_RETURN(LNETcurlFtpPutBase(L, LNET_FTP_NAMESPACE));
}

int LNETcurlFtpUploadFileSshFtp(lua_State* L){
   LNET_CHECK_TIMEOUT_AND_RETURN(LNETcurlFtpPutBase(L, LNET_SSH_FTP_NAMESPACE));
}

int LNETcurlFtpUploadFileFtpSsl(lua_State* L){
   LNET_CHECK_TIMEOUT_AND_RETURN(LNETcurlFtpPutBase(L, LNET_FTP_SSL_NAMESPACE));
}

static void LNETaddFtpMethods(lua_State* L){
   LUAaddMethod(L,LNET_FTP_LIST,            &LNETcurlFtpListDirectoryFtp);
   LUAaddMethod(L,LNET_FTP_GET,             &LNETcurlFtpDownloadFileFtp);
   LUAaddMethod(L,LNET_FTP_DELETE,          &LNETcurlFtpDeleteFileFtp);
   LUAaddMethod(L,LNET_FTP_RENAME,          &LNETcurlFtpRenameFileFtp);
   LUAaddMethod(L,LNET_FTP_PUT,             &LNETcurlFtpUploadFileFtp);
}

static void LNETaddSshFtpMethods(lua_State* L){
   LUAaddMethod(L,LNET_FTP_LIST,            &LNETcurlFtpListDirectorySshFtp);
   LUAaddMethod(L,LNET_FTP_GET,             &LNETcurlFtpDownloadFileSshFtp);
   LUAaddMethod(L,LNET_FTP_DELETE,          &LNETcurlFtpDeleteFileSshFtp);
   LUAaddMethod(L,LNET_FTP_RENAME,          &LNETcurlFtpRenameFileSshFtp);
   LUAaddMethod(L,LNET_FTP_PUT,             &LNETcurlFtpUploadFileSshFtp);
}

static void LNETaddFtpSslMethods(lua_State* L){
   LUAaddMethod(L,LNET_FTP_LIST,            &LNETcurlFtpListDirectoryFtpSsl);
   LUAaddMethod(L,LNET_FTP_GET,             &LNETcurlFtpDownlaodFileFtpSsl);
   LUAaddMethod(L,LNET_FTP_DELETE,          &LNETcurlFtpDeleteFileFtpSsl);
   LUAaddMethod(L,LNET_FTP_RENAME,          &LNETcurlFtpRenameFileFtpSsl);
   LUAaddMethod(L,LNET_FTP_PUT,             &LNETcurlFtpUploadFileFtpSsl);
}

//expects a table for __index on top of stack.
//leaves it there when we leave
static void LNETdefineFtpMetatable(lua_State* L, const char* pMetatableName){
   int MetaIndexTable = lua_gettop(L);
   int Result = luaL_newmetatable(L, pMetatableName);
   COLASSERT(Result);

   lua_pushvalue(L,MetaIndexTable); //push a copy
   lua_setfield(L, -2, "__index"); // metatable.__index = table that's passed in

   lua_pushcfunction(L, &LNETpushFtpKey);
   lua_setfield(L, -2, "__tostring"); // metatable.__tostring = LNETpushFtpKey

   lua_pop(L, 1); // pop metatable
}

void LNETaddFtpModule(lua_State* L){
   COL_FUNCTION(LNETaddFtpModule);
   lua_newtable(L); //ftp
   LUAaddMethod(L,LNET_FTP_INIT, &LNETcurlFtpInitFtp);
   LNETaddFtpMethods(L);
   LNETdefineFtpMetatable(L, LNET_FTP_META_TABLE);
   lua_setfield(L, -2, LNET_FTP_NAMESPACE);

   lua_newtable(L); //sftp
   LUAaddMethod(L,LNET_FTP_INIT, &LNETcurlFtpInitSshFtp);
   LNETaddSshFtpMethods(L);
   LNETdefineFtpMetatable(L, LNET_SSH_FTP_META_TABLE);
   lua_setfield(L, -2, LNET_SSH_FTP_NAMESPACE);

   lua_newtable(L); //ftps
   LUAaddMethod(L,LNET_FTP_INIT, &LNETcurlFtpInitFtpSsl);
   LNETaddFtpSslMethods(L);
   LNETdefineFtpMetatable(L, LNET_FTP_SSL_META_TABLE);
   lua_setfield(L, -2, LNET_FTP_SSL_NAMESPACE);
}

void LNETcurlFtpInactiveCleanup(lua_State* L){
   COL_FUNCTION(LNETcurlFtpInactiveCleanup);
   const int MAX_CONNECTION_IDLE_TIME = 60; // In seconds
   COLtimeStamp CurrentTime;
   COLcurrentTimeStamp (&CurrentTime);

   LNETcurlFtpCache& Cache = LNETgetCurlFtpCache(L);
   LNETcurlFtpCache::TPlace Place = Cache.first();
   while(Place){
      LNETcurlFtpCache::TPlace NextPlace = Cache.next(Place);
      if (CurrentTime.tv_sec - Cache[Place]->m_LastAccessTimeStamp.tv_sec > MAX_CONNECTION_IDLE_TIME ){
         Cache.remove(Place);
      }
      Place = NextPlace;
   }
}
