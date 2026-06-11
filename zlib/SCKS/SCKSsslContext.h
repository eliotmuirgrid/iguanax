#ifndef __SCKS_SSL_CONTEXT_H__
#define __SCKS_SSL_CONTEXT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKSsslContext
//
// Author: Kevin Cai
//
// Description: Context class for generating SSL connections.
//   For any SSL connection, it's mandatory to call:
//      init
//   For server side ctx:
//      setPrivateKey, setCertificate
//   For client side ctx:
//      setServerMode
//   Note: Ctx can be reused for multiple connection classes.
//
//   For validation types, look at:
//   https://www.openssl.org/docs/ssl/SSL_CTX_set_verify.html
//---------------------------------------------------------------------------

#include <openssl/ssl.h>
#include <COL/COLstring.h>

struct ssl_ctx_st;
typedef struct ssl_ctx_st SSL_CTX;
typedef int (*SCKSsslAPLNselectCallback)(SSL *ssl, const unsigned char **out,
                                unsigned char *outlen, const unsigned char *in,
                                unsigned int inlen, void *arg);

class SCKSsslContext {
public:
   SCKSsslContext(bool Server = true);
   ~SCKSsslContext();

   // Better create SSL_CTX outside of constructor.
   int init();

   // Resets the entire object -- is this necessary?
   void freeCtx();

   // Defaults to true
   // Only works if called before init. call freeCtx before making changes.
   void setServerMode(bool Server);
   bool isServerMode() const;

   // Should be called before setPrivateKey
   int setCertificate(const COLstring& Filepath);
   const COLstring& getCertificatePath() const;

   int setPrivateKey(const COLstring& Filepath);
   const COLstring& getPrivateKeyPath() const;

   int setCAs(const COLstring& Filepath, const COLstring& Folderpath = "");
   const COLstring& getCAsPath() const; // Could be problematic to return filepath && folderpath

   int setAlpnSelectCallback(SCKSsslAPLNselectCallback pCallback);

   // Handle to object
   SSL_CTX* handle() { return m_pCTX; }

   int setVerify(bool Verify);
private:
   SSL_CTX* m_pCTX;
   bool m_ServerMode;

   // File paths to the files currently used
   COLstring m_CertificatePath;
   COLstring m_PrivateKeyPath;
   COLstring m_CAFilePath;

   SCKSsslContext(const SCKSsslContext& Orig); // not allowed
   SCKSsslContext& operator=(const SCKSsslContext& Orig); // not allowed
};

#endif // end of defensive include