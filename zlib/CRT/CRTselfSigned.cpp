// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CRTselfSigned
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Sun 20 Aug 2023 12:08:03 PDT
// ---------------------------------------------------------------------------

#define OPENSSL_NO_DEPRECATED
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/x509v3.h>
#include <openssl/rand.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <DIR/DIRpath.h>

#include <CRT/CRTselfSigned.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


// returns a pointer to an RSA key on success, NULL otherwise
// the returned key must be freed using CRTfreePrivateKey()
static EVP_PKEY* CRTcreateRsaPrivateKey(COLstring* pError, int KeySize = 4096) {
   COL_FUNCTION(CRTcreateRsaPrivateKey);
   EVP_PKEY_CTX* pCtx = NULL;
   EVP_PKEY* pKey = NULL;
   do {
      if (!(pCtx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL))) {
         *pError = "Failed to allocate a public key algorithm context.";   break;
      }
      if (EVP_PKEY_keygen_init(pCtx) <= 0) {
         *pError = "Failed to initialize the context for key generation."; break;
      }
      if (EVP_PKEY_CTX_set_rsa_keygen_bits(pCtx, KeySize) <= 0) {
         *pError = "Failed to set RSA key size.";                          break;
      }
      if (EVP_PKEY_keygen(pCtx, &pKey) <= 0) {
         *pError = "Failed to generate an RSA key.";                       break;
      }
   } while (false);
   EVP_PKEY_CTX_free(pCtx);
   return pKey;
}

static void CRTfreePrivateKey(EVP_PKEY** ppKey) {
   COL_FUNCTION(CRTfreePrivateKey);
   EVP_PKEY_free(*ppKey);
   *ppKey = NULL;
}

static bool CRTaddX509NameEntry(X509_NAME* pSubject, const char* Field, const char* Value, COLstring* pError) {
   COL_FUNCTION(CRTaddX509NameEntry);
   if (X509_NAME_add_entry_by_txt(pSubject, Field, MBSTRING_ASC, (const unsigned char*)Value, -1, -1, 0) <= 0) {
      *pError = "Failed to set the X509_NAME structure's '" + COL_T(Field) + "' field.";
      return false;
   }
   return true;
}

// returns a pointer to a certificate subject on success, NULL otherwise
// the returned subject must be freed using CRTfreeSubject()
static X509_NAME* CRTcreateSubject(COLstring* pError) {
   COL_FUNCTION(CRTcreateSubject);
   X509_NAME* pSubject = NULL;
   do {
      if (!(pSubject = X509_NAME_new())) {
         *pError = "Failed to creat a X509_NAME structure.";                                   break;
      }
      if (!CRTaddX509NameEntry(pSubject, "C",  "CA", pError))                                  break;
      if (!CRTaddX509NameEntry(pSubject, "ST", "ON", pError))                                  break;
      if (!CRTaddX509NameEntry(pSubject, "L",  "Toronto", pError))                             break;
      if (!CRTaddX509NameEntry(pSubject, "O",  "iNTERFACEWARE Inc.", pError))                  break;
      if (!CRTaddX509NameEntry(pSubject, "OU", "IguanaX", pError))                             break;
      if (!CRTaddX509NameEntry(pSubject, "CN", "localhost", pError))                           break;
      if (!CRTaddX509NameEntry(pSubject, "emailAddress", "contact@interfaceware.com", pError)) break;
      return pSubject;
   } while (false);
   X509_NAME_free(pSubject);
   return NULL;
}

static void CRTfreeSubject(X509_NAME** ppSubject) {
   COL_FUNCTION(CRTfreePrivateKey);
   X509_NAME_free(*ppSubject);
   *ppSubject = NULL;
}

static bool CRTaddExtension(X509* pCert, COLstring* pError) {
   COL_FUNCTION(CRTaddExtension);
   // Add basic constraints extension (CA: true)
   X509_EXTENSION *ext = X509V3_EXT_conf_nid(NULL, NULL, NID_basic_constraints, "critical,CA:TRUE");
   X509_add_ext(pCert, ext, -1);
   X509_EXTENSION_free(ext);
   return true;
}

// returns a pointer to a self-signed certificate on success, NULL otherwise
// the returned certificate must be freed using CRTfreeCertificate()
static X509* CRTcreateCertificate(EVP_PKEY* pKey, COLstring* pError, int Validity = 825) {
   COL_FUNCTION(CRTcreateCertificate);
   // Create a new X509 certificate
   X509* pCert = NULL;
   X509_NAME* pSubject = NULL;
   do {
      if (!(pCert = X509_new())) {
         *pError = "Failed to initialize a X509 structure.";               break;
      }
      if (X509_set_version(pCert, 2) <= 0) {
         *pError = "Failed to set the X509 structure's version.";          break;
      }
      if (ASN1_INTEGER_set(X509_get_serialNumber(pCert), 1) <= 0) {
         *pError = "Failed to set the X509 structure's serial number.";    break;
      }
      if (!X509_gmtime_adj(X509_getm_notBefore(pCert), 0)) {
         *pError = "Failed to set the certificate's inception date.";      break;
      }
      if (!X509_gmtime_adj(X509_getm_notAfter(pCert), Validity * 86400)) {
         *pError = "Failed to set the certificate's expiration date.";     break;
      }
      if (!(pSubject = CRTcreateSubject(pError)))                          break;
      if (X509_set_subject_name(pCert, pSubject) <= 0) {
         *pError = "Failed to set certificate's Common Name.";             break;
      }
      if (X509_set_issuer_name(pCert, pSubject) <= 0) {
         *pError = "Failed to set certificate's Issuer.";                  break;
      }
      CRTfreeSubject(&pSubject);
      if (X509_set_pubkey(pCert, pKey) <= 0) {
         *pError = "Failed to set certificate's Public Key.";              break;
      }
      if (!CRTaddExtension(pCert, pError))                                 break;
      if (X509_sign(pCert, pKey, EVP_sha256()) <= 0) {
         *pError = "Failed to sign the certificate.";                      break;
      }
      return pCert;
   } while (false);
   CRTfreeSubject(&pSubject);
   X509_free(pCert);
   return NULL;
}

static void CRTfreeCertificate(X509** ppCert) {
   COL_FUNCTION(CRTfreeCertificate);
   X509_free(*ppCert);
   *ppCert = NULL;
}

static bool CRTsaveCertificateAndKey(X509* pCert, EVP_PKEY* pKey, COLstring* pError) {
   COL_FUNCTION(CRTsaveCertificateAndKey);
   FILmakeFullDir(DIRotherDir(), FIL_USER_RWX);

   COLstring CertPath = DIRfileConfigWebCert();
   FILE* pCertFile = fopen(CertPath.c_str(), "wb");
   if (!pCertFile) {
      *pError = "Failed to open file to save the certificate: " + COLintToString(errno);
      return false;
   }
   if (PEM_write_X509(pCertFile, pCert) <= 0) {
      *pError = "Failed to write to the certificate file.";
      fclose(pCertFile);
      return false;
   }
   fclose(pCertFile);

   COLstring KeyPath = DIRfileConfigWebKey();
   FILE* pKeyFile = fopen(KeyPath.c_str(), "wb");
   if (!pKeyFile) {
      *pError = "Failed to open file to save the private key: " + COLintToString(errno);
      return false;
   }
   if (PEM_write_PKCS8PrivateKey(pKeyFile, pKey, NULL, NULL, 0, NULL, NULL) <= 0) {
      *pError = "Failed to write to the key file.";
      fclose(pKeyFile);
      return false;
   }
   fclose(pKeyFile);

   return true;
}

bool CRTgenerateSelfSignedCertKey(COLstring* pError){
   COL_FUNCTION(CRTgenerateSelfSignedCertKey);
   EVP_PKEY* pKey = NULL;
   X509* pCert = NULL;
   bool Success = false;
   do {
      if (!(pKey  = CRTcreateRsaPrivateKey(pError))) { break; }
      if (!(pCert = CRTcreateCertificate(pKey, pError))) { break; }
      if (!CRTsaveCertificateAndKey(pCert, pKey, pError)) { break; }
      Success = true;
   } while (false);
   CRTfreeCertificate(&pCert);
   CRTfreePrivateKey(&pKey);
   return Success;
}
