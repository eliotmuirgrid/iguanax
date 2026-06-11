//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CURLutils
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Thursday, November 10th, 2005 @ 04:53:24 PM
//
//---------------------------------------------------------------------------
#include "CURLutils.h"

#include <CURL/CURLclient.h>
#include <FIL/FILpathUtils.h>
#include <DIR/DIRpath.h>

#include <curl/curl.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


static bool s_CurlIsReady;

static COLstring DefaultCaBundlePath;

COLstring CURLdefaultCaBundlePath() {
   COL_FUNCTION(CURLdefaultCaBundlePath);
   COL_VAR(DefaultCaBundlePath);
   return DefaultCaBundlePath;
}

void setDefaultCaBundlePath(const COLstring& Path = "") {
   COL_FUNCTION(setDefaultCaBundlePath);
   if (Path.is_null()) {
      DefaultCaBundlePath = DIRfileResourceCaFile();
   } else {
      DefaultCaBundlePath = Path;
   }
   COL_VAR(DefaultCaBundlePath);
}

void CURLglobalInit(const COLstring& CaFilePath)
{
   s_CurlIsReady = !curl_global_init(CURL_GLOBAL_ALL);
   setDefaultCaBundlePath(CaFilePath);
}

CURL* CURLeasyInit()
{
   // Have to disable this check as NETDLL2 (and possibly others) don't have
   // a global initializer where we could call CURLglobalInit(), and we can't
   // use something like DllMain(), since we statically link on Unix.
   // Failures are not likely to occur outside of Iguana, anyway.
   //
   //COLMSGPRECONDITION(s_CurlIsReady, "CURL cannot be initialized.");

   CURL* pHandle = curl_easy_init();
   COLMSGPOSTCONDITION(pHandle, "Cannot create CURL handle.");
   return pHandle;
}

COLstring CURLurlEscape(const COLstring& String)
{
   char* pEscapedURL = curl_escape(String.c_str(), String.size());
   if (NULL == pEscapedURL)
   {
      COL_ERROR_STREAM("curl_escape failed", COLerror::PreCondition);
   }
   COLstring EscapedString(pEscapedURL);
   curl_free(pEscapedURL);
   return EscapedString;
}

//
// Debug Logging for CURL
//

void CURLdebugLog::trace(CURL* pCurl)
{
   if(m_pCurl) {
      curl_easy_setopt(m_pCurl, CURLOPT_VERBOSE, 0L);
      curl_easy_setopt(m_pCurl, CURLOPT_DEBUGFUNCTION, (void*)NULL);
      curl_easy_setopt(m_pCurl, CURLOPT_DEBUGDATA, (void*)NULL);
   }
   m_pCurl = pCurl;
   if(m_pCurl) {
      curl_easy_setopt(m_pCurl, CURLOPT_DEBUGDATA, (void*)this);
      curl_easy_setopt(m_pCurl, CURLOPT_DEBUGFUNCTION, (void*)debugCallback);
      curl_easy_setopt(m_pCurl, CURLOPT_VERBOSE, 1L);
   }
}

int CURLdebugLog::debugCallback(CURL*, curl_infotype InfoType,
                                char* pInfo, size_t InfoSize,
                                void* pContext)
{
   CURLdebugLog* pLog = (CURLdebugLog*)pContext;
   switch(InfoType) {
   case CURLINFO_TEXT:
      pLog->Output.append("* ", 2);

      pLog->Output.append(pInfo,InfoSize);
      break;
   case CURLINFO_HEADER_IN:
      pLog->Output.append("< ", 2);

      pLog->Output.append(pInfo,InfoSize);
      break;
   case CURLINFO_HEADER_OUT:
      pLog->Output.append("> ", 2);

      pLog->Output.append(pInfo, InfoSize);
      break;
   case CURLINFO_DATA_IN:
   case CURLINFO_DATA_OUT:
   case CURLINFO_SSL_DATA_IN:
   case CURLINFO_SSL_DATA_OUT:
      // For Secure FTP traces, Curl reports LIST data received as sent,
      // so we merge these together to avoid confusion.
      InfoType = CURLINFO_DATA_OUT;
      if(pLog->m_LastInfo != InfoType) {
         pLog->Output += COL_T("- [data transfer]\n");
      }
   default:
      break;
   }
   pLog->m_LastInfo = InfoType;
   return 0;
}
