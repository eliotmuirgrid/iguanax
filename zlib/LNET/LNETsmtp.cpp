//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNETsmtp
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   Wednesday, March 9th, 2011 @ 02:21:40 PM
//
//---------------------------------------------------------------------------
#include "LNETsmtp.h"

// #include "NODLenvironment.h"
#include <NODL/NODLutils.h>
#include <LNET/LNETcurl.h>
#include <LUA/LUAtimeout.h>
#include <LUAL/LUALutils.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAutils.h>
#include <LUA/LUAcheck.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAtable.h>

#include <CURL/CURLutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#define LNET_CURL_SMTP_DEFAULT_TIMEOUT  15

static const char *const LNET_SMTP_OPTIONS[] = {
   "server", "username", "password", "timeout", "to", "from", "header", "body",
   "use_ssl", "certificate_file", "certificate_type", "private_key_file", "private_key_pass", "private_key_type",
   "ssl_engine", "verify_peer", "verify_host", "ca_file", "issuer_cert", "crl_file", "ssl_version", "cipher_list",
   "cipher_suite_list", "live", "debug", NULL
};

bool LNETsmtpValidateServer(const COLstring& Server, COLstring& OutputServer ) {
   //only allow servers of form
   //smtp://abc
   //abc - assume this is smtp:// prefixed
   const char* Smtp = "smtp://";
   if (Server.find(Smtp) == 0){
      OutputServer = Server;
   }
   else if (Server.find("://") != npos){
      return false;
   }
   else{
      OutputServer = Smtp + Server;
   }
   return true;
}

static COLstring LNETfixEmail(const COLstring& From) {
   // libCurl 7.21.4 automatically wraps MAIL FROM values with <> when they
   // don't begin with "<", but we ship an older version.  The version we do
   // ship requires <> around RCPT TO values, and will insert them likewise.
   // If the user specifies "Name <email@address.com>" we strip the name; if
   // they omit the <> around what we assume is the address, we insert it.
   //
   const char* pFrom = From.c_str();
   const char* pLt = strchr(pFrom, '<');
   if(!pLt) {
      return COL_T("<") + From + COL_T(">");
   } else if(pLt != pFrom) {
      return From.substr(pLt - pFrom);
   }
   return From;
}

//assumes table for params is at top of stack
static curl_slist* LNETmakeToList(lua_State* L){
   curl_slist* pList = 0;
   int TableIndex = lua_gettop(L);
   int ItemCount = 0;
   lua_pushnil(L);  /* first key */
   while (lua_next(L, TableIndex) != 0) {
      //we use tostring to truncate at embedded nulls
      const char* pValue = lua_tostring(L,-1);
      if (pValue){
         const COLstring& Email = LNETfixEmail(pValue);
         pList = curl_slist_append(pList, Email.c_str());
      }
      lua_pop(L,1);//removes 'value'; keeps 'key' for next iteration
   }
   lua_pop(L,1);//pop table, if lua_next returns false, the key is already popped
   return pList;
}

static void LNETmakeHeaderData(lua_State* L, COLostream& Stream){
   int TableIndex = lua_gettop(L);
   lua_pushnil(L);  /* first key */
   while (lua_next(L, TableIndex) != 0) {
      //we use tostring to truncate at embedded nulls
      const char* pKey = lua_tostring(L,-2);
      const char* pValue = lua_tostring(L,-1);
      if (pKey){ //allow empty values, but not empty keys
         Stream << pKey << ':' << (pValue ? pValue : "") << "\r\n";
      }
      lua_pop(L,1);//removes 'value'; keeps 'key' for next iteration
   }
   lua_pop(L,1);//pop table, if lua_next returns false, the key is already popped
}

class LNETsmtpPayloadContext {
public:
   LNETsmtpPayloadContext() : BytesRead(0){}
   COLstring Data;
   COLint32 BytesRead;
};

static size_t onReadPayload(void *ptr, size_t size, size_t nmemb, void *userp) {
   LNETsmtpPayloadContext* pCtx = (LNETsmtpPayloadContext*)userp;
   if ((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
      return 0;
   }
   else if (pCtx->BytesRead >= pCtx->Data.size()){
      //done reading
      return 0;
   }
   size_t Length = COL_MIN((size*nmemb), (pCtx->Data.size() - pCtx->BytesRead) );
   if (Length){
      memcpy(ptr, pCtx->Data.c_str() + pCtx->BytesRead , Length);
      pCtx->BytesRead += Length;
   }
   return Length;
}

#define LNET_SMTP_STR_OPT_PARAM(Key,Default) LUAoptionalStringParam(L, 1, (Key), (Default))
#define LNET_SMTP_INT_OPT_PARAM(Key,Default) LUAoptionalIntParam   (L, 1, (Key), (Default))
#define LNET_SMTP_STR_PARAM(Key)             LUAexpectedStringParam(L, 1, (Key))
#define LNET_SMTP_TABLE_PARAM(Key)           LUAexpectedTableParam (L, 1, (Key))

static CURLcode perform(lua_State* L, CURL* pCurl) {
   CURLcode Result = curl_easy_perform(pCurl);
   return Result;
}

static void setBoolOpt(lua_State* L, LNETcurlCacheItem* pCurlCachedItem,
                       CURLoption Opt, const char* pName, bool Default,
                       long TrueValue = 1L, long FalseValue = 0L) {
   COL_FUNCTION(setBoolOpt);
   bool Value = LUAoptionalBoolParam(L, 1, pName, Default);
   LNETcurlCacheItemCheckError(pCurlCachedItem, curl_easy_setopt(pCurlCachedItem->m_pHandle, Opt, Value ? TrueValue : FalseValue) );
}

static void setStringOpt(lua_State* L, LNETcurlCacheItem* pCurlCachedItem,
                         CURLoption Opt, const char* pName) {
   COL_FUNCTION(setStringOpt);
   if ( LUAoptionalStringParamPushed(L, 1, pName) ) {
      LNETcurlCacheItemCheckError(pCurlCachedItem, curl_easy_setopt(pCurlCachedItem->m_pHandle, Opt, lua_tostring(L,-1)) );
      lua_pop(L,1);
   }
}

static void setSslOptions(lua_State* L, LNETcurlCacheItem* pCurlCachedItem) {
   COL_FUNCTION(setCurlSslOptions);
   COLstring SslOpt = LNET_SMTP_STR_OPT_PARAM("use_ssl", "");
   CURL* pCurlHandle = pCurlCachedItem->m_pHandle;

   if (SslOpt == "yes") {
      curl_easy_setopt(pCurlHandle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
   }
   else if (SslOpt == "try") {
      curl_easy_setopt(pCurlHandle, CURLOPT_USE_SSL, CURLUSESSL_TRY);
   }
   else {
      COL_TRC("Not using SSL...return...");
      return;
   }

   setBoolOpt(L, pCurlCachedItem, CURLOPT_SSL_VERIFYPEER, "verify_peer", true);
   setBoolOpt(L, pCurlCachedItem, CURLOPT_SSL_VERIFYHOST, "verify_host", true, 2);

   setStringOpt(L, pCurlCachedItem, CURLOPT_SSLCERT,     "certificate_file");
   setStringOpt(L, pCurlCachedItem, CURLOPT_SSLCERTTYPE, "certificate_type");
   setStringOpt(L, pCurlCachedItem, CURLOPT_SSLKEY,      "private_key_file");
   setStringOpt(L, pCurlCachedItem, CURLOPT_SSLKEYTYPE,  "private_key_type");
   setStringOpt(L, pCurlCachedItem, CURLOPT_SSLENGINE,   "ssl_engine");
   setStringOpt(L, pCurlCachedItem, CURLOPT_ISSUERCERT,  "issuer_cert");
   setStringOpt(L, pCurlCachedItem, CURLOPT_CRLFILE,     "crl_file");

   LNETcurlSetSslCipherSuiteList(L, pCurlHandle);
   LNETcurlSetSslCipherList(L, pCurlHandle);
   LNETcurlSetSslCaBundle(L, pCurlHandle);
   LNETcurlSetSslVersion(L, pCurlHandle);
}

static int LNETsmtpSendMail(lua_State* L) {
   COL_FUNCTION(LNETsmtpSendMail);
   LNETcurlStringList Recipients;
   try{
      LUAcheckParams(L, 1, LNET_SMTP_OPTIONS);
      COLstring SmtpServer;
      COLstring UserName = LNET_SMTP_STR_OPT_PARAM("username", "");
      COLstring Password = LNET_SMTP_STR_OPT_PARAM("password", "");
      COLstring FromParam = LNETfixEmail( LNET_SMTP_STR_PARAM("from") );

      int Timeout = LNET_SMTP_INT_OPT_PARAM("timeout",LNET_CURL_SMTP_DEFAULT_TIMEOUT);

      CURLdebugLog DebugLog;
      bool Debug = LUAoptionalBoolParam(L, 1, "debug", false);

      bool NoAct = LUALisEnvironmentTest(L)
         ? !LUAoptionalBoolParam(L, 1, "live", false)
         : false;
      if (LUALisEnvironmentStartup(L)) NoAct = true;
      LNETsmtpPayloadContext Payload;
      COLostream PayloadStream(Payload.Data);
      LNET_SMTP_TABLE_PARAM("header");
      LNETmakeHeaderData(L,PayloadStream);
      Payload.Data += "\r\n";
      Payload.Data += LNET_SMTP_STR_PARAM("body");

      LNET_SMTP_TABLE_PARAM("to");
      Recipients.pList = LNETmakeToList(L);

      {
         COLstring SmtpParam = LNET_SMTP_STR_PARAM("server");
         if ( ! LNETsmtpValidateServer(SmtpParam, SmtpServer) ) {
            COL_ERROR_STREAM("Invalid SMTP server " << SmtpParam << ". Servers should begin with smtp://.\nIf not provided, smtp:// is assumed.",COLerror::SystemError);
         }
      }

      if (!NoAct) {
         LNETcurlCacheItem* pCurl = LNETcurlGetCachedHandle(L, SmtpServer);
         CURL* pCurlHandle = pCurl->m_pHandle;

         if (Debug) {
            DebugLog.trace(pCurlHandle);
         }
         LNETcurlCacheItemCheckError(pCurl, curl_easy_setopt(pCurlHandle, CURLOPT_URL, SmtpServer.c_str()));
         // LNETcurlCacheItemCheckError(pCurl, curl_easy_setopt(pCurlHandle, CURLOPT_URL, SmtpServer.c_str())); // This was called twice in the original implementation - Vismay

         setSslOptions(L, pCurl);

         if ( UserName.size() || Password.size() ) {
            LNETcurlCacheItemCheckError(pCurl, curl_easy_setopt(pCurlHandle, CURLOPT_USERNAME, UserName.c_str()) );
            LNETcurlCacheItemCheckError(pCurl, curl_easy_setopt(pCurlHandle, CURLOPT_PASSWORD, Password.c_str()) );
         }

         LNETcurlCacheItemCheckError(pCurl, curl_easy_setopt(pCurlHandle, CURLOPT_TIMEOUT, Timeout)             );
         LNETcurlCacheItemCheckError(pCurl, curl_easy_setopt(pCurlHandle, CURLOPT_READDATA, &Payload)           );
         LNETcurlCacheItemCheckError(pCurl, curl_easy_setopt(pCurlHandle, CURLOPT_READFUNCTION, onReadPayload)  );
         LNETcurlCacheItemCheckError(pCurl, curl_easy_setopt(pCurlHandle, CURLOPT_MAIL_FROM, FromParam.c_str()) );
         LNETcurlCacheItemCheckError(pCurl, curl_easy_setopt(pCurlHandle, CURLOPT_MAIL_RCPT, Recipients.pList)  );

         // Set this to disable VRFY command being sent.
         // https://sourceforge.net/p/curl/bugs/1389/
         LNETcurlCacheItemCheckError(pCurl, curl_easy_setopt(pCurlHandle, CURLOPT_UPLOAD, 1L) );

         try {
            //TODO - allow user to reset this to 0 in the case of bad ssl implementations?
            //curl_easy_setopt(curl, CURLOPT_SSL_SESSIONID_CACHE, 0);
            LNETcurlCacheItemCheckError(pCurl, perform(L, pCurlHandle));
         }
         catch (const COLerror& Error) {
            LNETcurlAppendDebugLog(DebugLog,Error);
         }
         //in case the op took a while, refresh time
         LNETcurlCacheItemTouch(pCurl);
      }

      LUAcheckTimeout(L);

      if (Debug) {
         LUApushString(L, DebugLog.Output);
         return 1;
      }

      return 0;
   }

   LUA_CATCH();
   return 0;
}

void LNETaddSmtpModule(lua_State* L) {
   COL_FUNCTION(LNETaddSmtpModule);
   lua_newtable(L); //smtp
   LUAaddMethod(L, "send",      &LNETsmtpSendMail);
   LUAaddMethod(L, "sendEmail", &LNETsmtpSendMail);
   lua_setfield(L, -2, "smtp");
}
