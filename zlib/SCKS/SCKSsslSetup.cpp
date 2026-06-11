// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKSsslSetup
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 21 June 2023 - 10:29AM
// ---------------------------------------------------------------------------

#include <SCKS/SCKSsslSetup.h>
#include <SCKS/SCKSsslContext.h>
#include <SCKS/SCKSsslUtils.h>

#include <openssl/ssl.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void SCKSsslSetup(SCKSsslContext* m_pContext, SSL** m_ppSSL, BIO** m_ppInBio, BIO** m_ppOutBio){
   COL_FUNCTION(SCKSsslSetup);
   if (m_pContext == NULL){
      COL_ERR("No SSL_CTX is registered with SCKSsslConnection");
      throw COLerror("No SSL_CTX is registered with SCKSsslConnection", 0);
   }
   *m_ppSSL = SSL_new(m_pContext->handle());
   if (*m_ppSSL == NULL){
      COL_ERR("Error creating SSL object. Refer to SCKSsslError()");
      COL_ERR(SCKSsslError());
      throw COLerror(SCKSsslError(), 0);
   }
   *m_ppInBio  = BIO_new(BIO_s_mem());
   *m_ppOutBio = BIO_new(BIO_s_mem());
   if (*m_ppInBio == NULL || *m_ppOutBio == NULL){
      COL_ERR("Error creating BIO buffers for SSL/TLS connection");
      SSL_free(*m_ppSSL);
      *m_ppSSL = 0;
      throw COLerror(SCKSsslError(), 0);
   }
   // OpenSSL recommends using these functions for assigning these BIOs to the SSL object.
   SSL_set0_rbio(*m_ppSSL, *m_ppInBio);
   SSL_set0_wbio(*m_ppSSL, *m_ppOutBio);
   COL_TRC("Created and assigned the input and output BIOs for the SSL context");
   // Since we are not using ssl connect / accept, we must manually dictate the mode.
   if (m_pContext->isServerMode()){
      COL_TRC("Setting as accept");
      SSL_set_accept_state(*m_ppSSL);
   } else {
      COL_TRC("Setting as connect");
      SSL_set_connect_state(*m_ppSSL);
   }
}