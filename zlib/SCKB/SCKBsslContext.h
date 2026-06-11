#ifndef __SCKB_SSL_CONTEXT_H__
#define __SCKB_SSL_CONTEXT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKBsslContext
//
// Description:
//
// Context class for generating SSL connections.
//
// Author: John Q
// Date:   Tue  1 Nov 2022 22:08:55 EDT
//---------------------------------------------------------------------------
#include <COL/COLstring.h> // for COLstring default
#include <openssl/ssl.h>   // for Cert Key file type default

class SCKBsslContext {
public:
   SCKBsslContext();
   ~SCKBsslContext();
   SCKBsslContext(const SCKBsslContext& Orig) = delete;
   SCKBsslContext& operator=(const SCKBsslContext& Orig) = delete;

   // Set the min and max TLS version. Available values are:
   // SSL3_VERSION, TLS1_VERSION, TLS1_1_VERSION, TLS1_2_VERSION, TLS1_3_VERSION
   bool setMinVersion (int Version);
   bool setMaxVersion (int Version);
   bool setCipherList (const COLstring& Ciphers);
   bool setCaLocations(const COLstring& FilePath, const COLstring& DirPath = "");
   // Only PEM or ASN1 format are supported for now
   bool setCertificate(const COLstring& FilePath, int FileType = SSL_FILETYPE_PEM);
   bool setPrivateKey (const COLstring& FilePath, int FileType = SSL_FILETYPE_PEM, const COLstring& Password = "");
   void setVerifyPeer (bool Verify);
   void setVerifyHost (bool Verify);

   // must called before connect
   bool setUpHostVerify(SSL* pSsl, const COLstring& Hostname);
   // call after connect
   bool verifyPeerHost (SSL* pSsl);

   SSL_CTX*  handle() { return m_pCTX; }
   COLstring lastError();
private:
   SSL_CTX* m_pCTX;
   bool m_VerifyHost { false };
   int  m_LastError  { X509_V_OK };
};

void SCKBsslError();

#endif //End of defensive guard
