// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKSsslDebugCallbacks
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 21 June 2023 - 10:44AM
// ---------------------------------------------------------------------------

#include <SCKS/SCKSsslDebugCallbacks.h>
#include <SCKS/SCKSsslUtils.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void SCKSsslDebugServerCallback(const SSL* pSsl, int where, int ret){
   //COL_FUNCTION(SCKSsslDebugServerCallback);
   if(ret == 0) {
      COL_TRC("-- krx_ssl_info_client_callback: error occurred.");
      return;
   }
   COL_TRC("Client : " << SSL_state_string_long(pSsl));
}

void SCKSsslDebugCallbackServer(const SSL* pSsl, int where, int ret){
   //COL_FUNCTION(SCKSsslDebugCallbackServer);
   if(ret == 0) {
      COL_TRC("-- krx_ssl_info_server_callback: error occurred.");
      return;
   }
   COL_TRC("Server : " << SSL_state_string_long(pSsl));
}

void SCKSsslDebugRegister(SSL* pSSL, bool IsServer){
   COL_FUNCTION(SCKSsslDebugRegister);
   if (IsServer){
      SSL_set_info_callback(pSSL, SCKSsslDebugCallbackServer);
   } else {    
      SSL_set_info_callback(pSSL, SCKSsslDebugServerCallback);
   }
}

void SCKSreportFatalError(int ErrorCode, int RC){
   switch (ErrorCode){
   case SSL_ERROR_WANT_CONNECT: COL_TRC("Needed connect?");                          break;
   case SSL_ERROR_WANT_ACCEPT:  COL_TRC("SSL object was not initialized properly."); break;
   case SSL_ERROR_ZERO_RETURN:  COL_TRC("SSL connection has been closed.");          break;
   case SSL_ERROR_SSL:          COL_TRC(SCKSsslError());                             break;
   case SSL_ERROR_SYSCALL:
      if (ERR_peek_error() != 0){ COL_TRC("Error on SSL error queue. " << SCKSsslError());} 
      else if (RC == 0)         { COL_TRC("Unexpected EOF encountered");                  } 
      else if (RC == -1)        { COL_TRC("System error ");                               }
      break;
   }
}