//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: SCKBsslContext
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Tue  1 Nov 2022 22:08:55 EDT
//---------------------------------------------------------------------------
#include "SCKBsslContext.h"

#include <openssl/err.h>
#include <openssl/x509v3.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

SCKBsslContext::SCKBsslContext() {
   COL_METHOD(SCKBsslContext::SCKBsslContext);
   m_pCTX = SSL_CTX_new(TLS_client_method());
   SSL_CTX_set_default_verify_paths(m_pCTX);
};

SCKBsslContext::~SCKBsslContext() {
   COL_METHOD(SCKBsslContext::~SCKBsslContext);
   if (m_pCTX) {
      SSL_CTX_free(m_pCTX);
      m_pCTX = NULL;
   }
}

bool SCKBsslContext::setMinVersion(int Version) {
   COL_METHOD(SCKBsslContext::setMinVersion);
   if (Version < TLS1_VERSION) { COL_WRN("Allowing deprecated protocols."); }
   return SSL_CTX_set_min_proto_version(m_pCTX, Version);
}

bool SCKBsslContext::setMaxVersion(int Version) {
   COL_METHOD(SCKBsslContext::setMaxVersion);
   return SSL_CTX_set_min_proto_version(m_pCTX, Version);
}

bool SCKBsslContext::setCipherList(const COLstring& Ciphers) {
   COL_METHOD(SCKBsslContext::setCipherList);
   return SSL_CTX_set_cipher_list(m_pCTX, Ciphers.c_str());
}

bool SCKBsslContext::setCaLocations(const COLstring& FilePath, const COLstring& DirPath) {
   COL_METHOD(SCKBsslContext::setCaLocations);
   const char* pFilePath = FilePath.c_str();
   const char* pDirPath  = DirPath.c_str();
   if (FilePath.is_null()) { pFilePath = NULL; }
   if (DirPath.is_null())  { pDirPath = NULL; }
   COL_VAR2(pFilePath, pDirPath);
   bool Success = SSL_CTX_load_verify_locations(m_pCTX, pFilePath, pDirPath);
   if (!Success) { SCKBsslError(); }
   return Success;
}

bool SCKBsslContext::setCertificate(const COLstring& FilePath, int FileType) {
   COL_METHOD(SCKBsslContext::setCertificate);
   return SSL_CTX_use_certificate_file(m_pCTX, FilePath.c_str(), FileType);
}

static int SCKBpasswordCallback(char *buf, int size, int rwflag, void *u) {
   strncpy(buf, (char *)u, size);
   buf[size - 1] = '\0';
   return strlen(buf);
 }

bool SCKBsslContext::setPrivateKey(const COLstring& FilePath, int FileType, const COLstring& Password) {
   COL_METHOD(SCKBsslContext::setPrivateKey);
   if (!Password.is_null()) {
      SSL_CTX_set_default_passwd_cb_userdata(m_pCTX, (void*)Password.c_str());
      SSL_CTX_set_default_passwd_cb(m_pCTX, SCKBpasswordCallback);
   }
   bool Success = SSL_CTX_use_PrivateKey_file(m_pCTX, FilePath.c_str(), FileType);
   if (!Success) { SCKBsslError(); }
   return Success;
}

void SCKBsslContext::setVerifyPeer(bool Verify) {
   COL_METHOD(SCKBsslContext::setVerifyPeer);
   SSL_CTX_set_verify(m_pCTX, (Verify ? SSL_VERIFY_PEER : SSL_VERIFY_NONE), NULL);
}

void SCKBsslContext::setVerifyHost(bool Verify) {
   COL_METHOD(SCKBsslContext::setVerifyHost);
   if (Verify) { setVerifyPeer(Verify); }
   m_VerifyHost = Verify;
}

bool SCKBsslContext::setUpHostVerify(SSL* pSsl, const COLstring& Hostname) {
   COL_METHOD(SCKBsslContext::setUpHostVerify);
   if (!m_VerifyHost) { return true; }
   COL_VAR(Hostname);
   SSL_set_hostflags(pSsl, X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS);
   return SSL_set1_host(pSsl, Hostname.c_str());
}

bool SCKBsslContext::verifyPeerHost(SSL* pSsl) {
   COL_METHOD(SCKBsslContext::verifyPeerHost);
   m_LastError = SSL_get_verify_result(pSsl);
   return m_LastError == X509_V_OK;
}

COLstring SCKBsslContext::lastError() {
   return X509_verify_cert_error_string(m_LastError);
}

void SCKBsslError() {
   int Code = 0;
   while ((Code = ERR_get_error())) {
      COL_ERR(ERR_error_string(Code, NULL));
   }
}
