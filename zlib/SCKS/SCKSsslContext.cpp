//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: SCKSsslContext
//
// Author: Kevin Cai
//
// Implementation
//---------------------------------------------------------------------------

#include <SCK/SCKsystem.h>

#include <SCKS/SCKSsslContext.h>
#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

SCKSsslContext::SCKSsslContext(bool Server):m_pCTX(NULL), m_ServerMode(Server){
   COL_METHOD(SCKSsslContext::SCKSsslContext);
};

SCKSsslContext::~SCKSsslContext(){
   COL_METHOD(SCKSsslContext::~SCKSsslContext);
   freeCtx();
}

void SCKSsslContext::freeCtx(){
   COL_METHOD(SCKSsslContext::freeCtx);
   if (m_pCTX){
      SSL_CTX_free(m_pCTX);
   }
   m_pCTX = NULL;
   m_ServerMode = true;
   m_CertificatePath = m_PrivateKeyPath = m_CAFilePath = "";
}

int SCKSsslContext::init(){
   COL_METHOD(SCKSsslContext::init);
   m_pCTX = SSL_CTX_new(m_ServerMode ? SSLv23_server_method() : SSLv23_client_method());
   COL_VAR(m_pCTX);
   if (m_pCTX == NULL){
      COL_TRC("Returning -1 - failed!");
      return -1;
   }
   //SSL_CTX_set_mode(m_pCTX, SSL_MODE_AUTO_RETRY); // Read/write only returns after handshake compeleted.
   setVerify(false); // Unexpected behaviour if it's never set. Better to set here in case it's forgotten.
   COL_TRC("Succeeded!");
   return 0;
}

void SCKSsslContext::setServerMode(bool Server){
   COL_METHOD(SCKSsslContext::setServerMode);
   m_ServerMode = Server;
}

bool SCKSsslContext::isServerMode() const{
   COL_METHOD(SCKSsslContext::isServerMode);
   return m_ServerMode;
}

int SCKSsslContext::setCertificate(const COLstring& Filepath){
   COL_METHOD(SCKSsslContext::setCertificate);
   COL_VAR(Filepath);
   if (m_pCTX == NULL){ // Too much hand holding?
      return -1;
   }
   COLstring CorrectedPath = Filepath;
   FILcorrectPathSeparators(CorrectedPath);
   m_CertificatePath.zero();
   COL_VAR(CorrectedPath);
   int rc = SSL_CTX_use_certificate_chain_file(m_pCTX, CorrectedPath.c_str());
   if (rc == 0){
      COL_ERR("Cannot load certificate at path " << Filepath);
      return -1;
   }
   m_CertificatePath = CorrectedPath.c_str();
   return 0;
}

int SCKSsslContext::setPrivateKey(const COLstring& Filepath){
   COL_METHOD(SCKSsslContext::setPrivateKey);
   if (m_pCTX == NULL){ // Too much hand holding?
      return -1;
   }
   COLstring CorrectedPath = Filepath;
   FILcorrectPathSeparators(CorrectedPath);
   m_PrivateKeyPath.zero();
   COL_VAR2(CorrectedPath, Filepath);

   int rc = SSL_CTX_use_PrivateKey_file(m_pCTX, CorrectedPath.c_str(), SSL_FILETYPE_PEM);
   if (rc == 0){
      COL_ERR("Cannot load private key at path " << Filepath);
      return -1;
   }
   m_PrivateKeyPath = CorrectedPath.c_str();
   return 0;
}

int SCKSsslContext::setCAs(const COLstring& Filepath, const COLstring& Folderpath){
   COL_METHOD(SCKSsslContext::setCAs);
   if (m_pCTX == NULL){ // Too much hand holding?
      return -1;
   }
   COLstring CorrectedFile = Filepath;
   COLstring CorrectedFolder = Folderpath;
   FILcorrectPathSeparators(CorrectedFile);
   FILcorrectPathSeparators(CorrectedFolder);
   m_CAFilePath.zero();

   int rc = SSL_CTX_load_verify_locations(m_pCTX, CorrectedFile.is_null() ? NULL : CorrectedFile.c_str(),
         CorrectedFolder.is_null() ? NULL : CorrectedFolder.c_str()); // What a mess!
   if (rc == 0){
      return -1;
   }
   m_CAFilePath = CorrectedFile + " " + CorrectedFolder;
   return 0;
}

int SCKSsslContext::setAlpnSelectCallback(SCKSsslAPLNselectCallback pCallback) {
   COL_METHOD(SCKSsslContext::setAlpnSelectCallback);
   if (m_pCTX == NULL){ // Too much hand holding?
      return -1;
   }
   SSL_CTX_set_alpn_select_cb(m_pCTX, pCallback, NULL);
   return 0;
}

int SCKSsslContext::setVerify(bool Verify){
   COL_METHOD(SCKSsslContext::setVerify);
   if (m_pCTX == NULL){ // Too much hand holding?
      return -1;
   }
   SSL_CTX_set_verify(m_pCTX, (Verify ? (SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT) : SSL_VERIFY_NONE), NULL);
   return 0;
}

const COLstring& SCKSsslContext::getCertificatePath() const{
   return m_CertificatePath;
}

const COLstring& SCKSsslContext::getPrivateKeyPath() const{
   return m_PrivateKeyPath;
}

const COLstring& SCKSsslContext::getCAsPath() const{
   return m_CAFilePath;
}