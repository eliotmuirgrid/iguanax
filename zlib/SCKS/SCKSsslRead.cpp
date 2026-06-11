// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKSsslRead
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 21 June 2023 - 12:56PM
// ---------------------------------------------------------------------------

#include <SCKS/SCKSsslRead.h>
#include <SCKS/SCKSsslUtils.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int SCKSsslRead(BIO* m_pInBio, SSL* m_pSSL, COLstring* pOut, COLstring* pIn){
   COL_FUNCTION(SCKSsslRead);
   COL_DUMP("In", pIn->c_str(), pIn->size());
   int AmountWritten = BIO_write(m_pInBio, pIn->c_str(), pIn->size());
   COL_TRC("Wrote " << AmountWritten << " to the SSL read sink bio from incoming data.");
   if ((AmountWritten <= 0) && (BIO_should_retry(m_pInBio) == false)){
      COL_ERR("Unexpected error.  Couldn't write to the SSL read sink bio.  This is very weird.");
      // TODO - what should we do with this?
   }
   pIn->clear();
   int AmountRead;
   while (1){
      int SizeRequired = pOut->size() + 17000;  // 17000 - approximate size of an SSL record
      if (pOut->capacity() <= SizeRequired){
         COL_TRC("Increasing size of input buffer to " << SizeRequired);
         pOut->setCapacity(SizeRequired);
      }
      AmountRead = SSL_read(m_pSSL, pOut->get_buffer()+pOut->size(), pOut->capacity()-pOut->size());
      COL_TRC("Decrypted " << AmountRead << " bytes from the SSL input stream.");
      if (AmountRead <= 0) { COL_TRC("Breaking since amount read is less than zero"); break; }
      pOut->setSize(pOut->size() + AmountRead);
   }
   COL_DUMP("Out", pOut->c_str(), pOut->size());
   return AmountRead;
}

const char* SCKSerrorAsString(int Code){
   switch(Code) {
   case SSL_ERROR_NONE      :  return "SSL_ERROR_NONE";
   case SSL_ERROR_WANT_WRITE:  return "SSL_ERROR_WANT_WRITE";
   case SSL_ERROR_WANT_READ :  return "SSL_ERROR_WANT_READ";
   case SSL_ERROR_ZERO_RETURN: return "SSL_ERROR_ZERO_RETURN";
   case SSL_ERROR_SYSCALL:     return "SSL_ERROR_SYSCALL";
   case SSL_ERROR_SSL:         return "SSL_ERROR_SSL";
   default:                    return "SSL_UNKNOWN";
   }
}

static void SCKSflushErrors(COLstring* pError){
   *pError = SCKSsslError();
   if (*pError != "") {
      COL_TRC(*pError);
   }
}

int SCKSsslCheckError(SSL* m_pSSL, int ReturnCode, COLstring* pError){
   COL_FUNCTION(SCKSsslCheckError);
   SCKSflushErrors(pError); // To ensure accurate error messages from SSL_get_error
   int SSLerror = SSL_get_error(m_pSSL, ReturnCode);
   COL_VAR2(SCKSerrorAsString(SSLerror), SSLerror);
   return SSLerror;
}