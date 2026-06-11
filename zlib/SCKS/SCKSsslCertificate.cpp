//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKSsslCertificate
//
// Author: John Qi
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>

#include <openssl/conf.h>
#include <openssl/pem.h>
#include <openssl/x509v3.h>
#include <openssl/crypto.h>
#ifndef OPENSSL_NO_ENGINE
#  include <openssl/engine.h>
#endif

#include <COL/COLstring.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <FIL/FILutils.h>

#ifdef _WIN32
// Need to undefine X509_NAME in _WIN32. Otherwise we get "error C2065: 'name' : undeclared identifier"
// http://openssl.6102.n7.nabble.com/X509-h-errors-td38910.html
#  undef X509_NAME
#  undef X509_EXTENSIONS
#  undef X509_CERT_PAIR
#  undef PKCS7_ISSUER_AND_SERIAL
#  undef OCSP_REQUEST
#  undef OCSP_RESPONSE
#endif

void SCKSsslSavePrivateKeyToFile(EVP_PKEY* pkey, const COLstring& FileName) {
   COL_FUNCTION(SCKSsslSavePrivateKeyToFile);
   COL_VAR(FileName);
   COL_TRC("Saving to file.");
   FILE* f;
   f = fopen(FileName.c_str(), "wb");
   PEM_write_PrivateKey(f, pkey, NULL, NULL, 0, NULL, NULL); //This doesn't seem to be the right way to use this function

/* // TODO: figure out how to make the passphrase protected private key to work with cow
   PEM_write_PrivateKey(
      f,                               // write the key to the file we've opened
      pkey,                            // our key from earlier
      EVP_des_ede3_cbc(),              // default cipher for encrypting the key on disk
      (unsigned char *)PRIVATE_KEY_PASSPHRASE,          // passphrase required for decrypting the key on disk
      PRIVATE_KEY_PASSPHRASE_LENGTH,   // length of the passphrase string
      NULL,                            // callback for requesting a password
      NULL                             // data to pass to the callback
      );
*/
   fclose(f);
   COL_TRC("Done writing");

#ifndef _WIN32
   // set file permission to rw-------
   FILsetPermission(FileName, 0600);
#endif // !_WIN32
}

bool SCKSaddExt(X509* cert, int nid, const char* value){
   X509_EXTENSION* ex;
   X509V3_CTX ctx;
   /* This sets the 'context' of the extensions. */
   /* No configuration database */
   X509V3_set_ctx_nodb(&ctx);
   /* Issuer and subject certs: both the target since it is self signed,
   * no request and no CRL
   */
   X509V3_set_ctx(&ctx, cert, cert, NULL, NULL, 0);
   ex = X509V3_EXT_conf_nid(NULL, &ctx, nid, value);
   if (!ex)
      return false;

   X509_add_ext(cert, ex, -1);
   X509_EXTENSION_free(ex);
   return true;
}

bool SCKSsslCreateCertificate(X509** x509p, EVP_PKEY* pkeyp, int serial, int days) {
   COL_FUNCTION(SCKSsslCreateCertificate);
   COL_VAR2(serial, days);

   X509* x;
   X509_NAME* name = NULL;
   if ((x509p == NULL) || (*x509p == NULL)) {
      if ((x = X509_new()) == NULL) {
         return false;
      }
   } else {
      x = *x509p;
   }

   X509_set_version(x, 2);
   ASN1_INTEGER_set(X509_get_serialNumber(x), serial);
   X509_gmtime_adj(X509_get_notBefore(x), 0);
   X509_gmtime_adj(X509_get_notAfter(x), (long)60 * 60 * 24 * days);
   X509_set_pubkey(x, pkeyp);

   name = X509_get_subject_name(x);

   X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char *)"CA", -1, -1, 0);
   X509_NAME_add_entry_by_txt(name, "ST", MBSTRING_ASC, (unsigned char *)"Ontario", -1, -1, 0);
   X509_NAME_add_entry_by_txt(name, "L", MBSTRING_ASC, (unsigned char *)"Toronto", -1, -1, 0);
   X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char *)"iNTERFACEWARE Inc.", -1, -1, 0);
//   X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *)"iNTERFACEWARE Inc.", -1, -1, 0);

   //Its self signed so set the issuer name to be the same as the subject.
   X509_set_issuer_name(x, name);

   /* Add various extensions: standard extensions */
   SCKSaddExt(x, NID_basic_constraints, "critical,CA:TRUE");
   SCKSaddExt(x, NID_key_usage, "critical,keyCertSign,cRLSign");
   SCKSaddExt(x, NID_authority_key_identifier, "keyid,issuer");
   SCKSaddExt(x, NID_subject_key_identifier, "hash");

   if (!X509_sign(x, pkeyp, EVP_sha1())) {
      return false;
   }

   *x509p = x;
   return true;
}

void SCKSsslSaveCertificateToFile(X509* x509, const COLstring& FileName) {
   COL_FUNCTION(SCKSsslSaveCertificateToFile);
   COL_VAR(FileName.c_str());
   FILE * f;
   f = fopen(FileName.c_str(), "wb");
   PEM_write_X509(
      f,   /* write the certificate to the file we've opened */
      x509 /* our certificate */
   );
   COL_TRC("Done writing");
   fclose(f);
}

bool SCKSsslMakeCertificate(const COLstring& SslDir, int bits, int serial, int days) {
   COL_FUNCTION(SCKSsslMakeCertificate);
   COL_VAR4(SslDir, bits, serial, days);
   BIO* bio_err; //This isn't used anywhere
   X509* x509 = NULL;

   bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);

   // crate private key
   EVP_PKEY* pkey = EVP_RSA_gen((unsigned int) bits);
   if (pkey == NULL) { return false; }

   // create ssl private key and certificate directory
   COL_VAR(SslDir);
   if (!FILfileExists(SslDir)) {
      COL_TRC("ssl dir does not exist, creating ...");
      COLint32 DirPermission = 493;       // permission = rwxr-xr-x
      FILmakeFullDir(SslDir, DirPermission);
   }

   COLstring KeyPath = SslDir + FIL_DIR_SEPARATOR + "key.pem";
   COL_VAR(KeyPath);
   SCKSsslSavePrivateKeyToFile(pkey, KeyPath);

   // create certificate
   if (!SCKSsslCreateCertificate(&x509, pkey, serial, days)) {
      return false;
   }
   COLstring CertPath = SslDir + FIL_DIR_SEPARATOR + "cert.pem";
   COL_VAR(CertPath);
   SCKSsslSaveCertificateToFile(x509, CertPath);

   X509_free(x509);
   EVP_PKEY_free(pkey);

#ifndef OPENSSL_NO_ENGINE
   ENGINE_cleanup();
#endif
   CRYPTO_cleanup_all_ex_data();
   BIO_free(bio_err);
   return true;
}