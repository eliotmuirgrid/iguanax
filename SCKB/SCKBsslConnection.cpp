//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKBsslConnection
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Tue  1 Nov 2022 22:08:55 EDT
//---------------------------------------------------------------------------
#include <SCKB/SCKBsslConnection.h>
#include <SCKB/SCKBsslContext.h>

#include <openssl/err.h>

#include <SCK/SCKerror.h>
#include <SCK/SCKsystem.h>
#include <SCK/SCKutils.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

SCKBsslConnection::SCKBsslConnection(SCKBsslContext* pContext)
 : SCKBconnection(), m_pContext(pContext) {
   COL_METHOD(SCKBsslConnection::SCKBsslConnection);
}

SCKBsslConnection::~SCKBsslConnection() {
   COL_METHOD(SCKBsslConnection::~SCKBsslConnection);
   close();
}

bool SCKBsslConnection::connect(const COLstring& Host, int Port) {
   COL_METHOD(SCKBsslConnection::connect);
   if (!SCKBconnection::connect(Host, Port)) {
      COL_TRC("Failed to connect to remote host");
      return false;
   }
   m_pSsl = SSL_new(m_pContext->handle());
   m_pBio = BIO_new(BIO_s_socket());
   BIO_set_fd(m_pBio, m_Handle, BIO_NOCLOSE);
   SSL_set_bio(m_pSsl, m_pBio, m_pBio);
   SSL_set_tlsext_host_name(m_pSsl, Host.c_str());  // IX-3038 This is important if the host we connect to has multiple host names
   m_pContext->setUpHostVerify(m_pSsl, Host);
   int Result = SSL_connect(m_pSsl);
   if (Result == 1) {
      COL_TRC("SSL connection established");
      return true;
   }
   int Code = SSL_get_error(m_pSsl, Result);
   // TODO: how to handle error properly?
   if (Code == SSL_ERROR_WANT_READ || Code == SSL_ERROR_WANT_WRITE) {
      COL_TRC("SSL timeout");
      m_LastError = SCK_ETIMEDOUT;
   }
   else if (Code == SSL_ERROR_SSL && !m_pContext->verifyPeerHost(m_pSsl)) {
      COL_TRC("Peer verify failed");
      m_LastError = SCK_ESSLVERIFY;
      SCKBsslError();
   }
   else {
      COL_WRN("Other SSL error " << Code);
      if (SSL_ERROR_SYSCALL == Code) {
         m_LastError = errno;
      }
      SCKBsslError();
   }
   close();
   return false;
}

int SCKBsslConnection::read(COLstring* pBuffer) {
   COL_METHOD(SCKBsslConnection::read);
   if (-1 == m_Handle || NULL == m_pSsl) {
      m_LastError = SCK_EBADF;
      return -1;
   }
   SCKBtimeoutSetter Setter(m_Handle, m_Timeout, SO_RCVTIMEO);
   int Amount = SSL_read(m_pSsl, pBuffer->get_buffer(), pBuffer->capacity()-1);
   COL_VAR(Amount);
   if (Amount >= 0) {
      pBuffer->setSize(Amount);
      return Amount;
   } else {
      // If SSL_read fails due to socket recv timeout, errno =
      // ETIMEDOUT (Windows); EAGAIN (macOS);
      m_LastError = SCKlastSocketError();
   }
   int Code = SSL_get_error(m_pSsl, Amount);
   // If SSL_read fails due to socket recv timeout, Code =
   // SSL_ERROR_SYSCALL (Windows); SSL_ERROR_WANT_READ (macOS)
   if (Code == SSL_ERROR_WANT_READ || Code == SSL_ERROR_WANT_WRITE) {
      COL_TRC("SSL timeout");
      m_LastError = SCK_ETIMEDOUT;
   } else {
#ifdef _WIN32
      // If SSL_read fails due to socket recv timeout, Code = SSL_ERROR_SYSCALL
      // on Windows and ERR_get_error() = 0;
      if (Code == SSL_ERROR_SYSCALL && ERR_get_error() == 0) {
         COL_TRC("SSL timeout");
         // m_LastError already set after SSL_read above
         return -1;
      }
#endif
      COL_WRN("Other SSL error " << Code);
      SCKBsslError();
      close();
   }
   return -1;
}

int SCKBsslConnection::write(const COLstring& Buffer) {
   COL_METHOD(SCKBsslConnection::write);
   if (-1 == m_Handle || NULL == m_pSsl) {
      m_LastError = SCK_EBADF;
      return -1;
   }
   SCKBtimeoutSetter Setter(m_Handle, m_Timeout, SO_SNDTIMEO);
   int Amount = SSL_write(m_pSsl, Buffer.get_buffer(), Buffer.size());
   if (Amount > 0) { return Amount; }
   int Code = SSL_get_error(m_pSsl, Amount);
   if (Code == SSL_ERROR_WANT_READ || Code == SSL_ERROR_WANT_WRITE) {
      COL_TRC("SSL timeout");
      m_LastError = SCK_ETIMEDOUT;
   } else {
      COL_WRN("Other SSL error " << Code);
      SCKBsslError();
      close();
   }
   return -1;
}

void SCKBsslConnection::close() {
   COL_METHOD(SCKBsslConnection::close);
   if (m_pSsl != NULL) {
      SSL_shutdown(m_pSsl);
      SSL_free(m_pSsl);
      m_pSsl = NULL;
      m_pBio = NULL;
   }
   SCKBconnection::close();
}