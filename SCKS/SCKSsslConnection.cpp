//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKSsslConnection
//
// Author: Kevin Cai
//
// Description: Implementation
//---------------------------------------------------------------------------

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include <SCK/SCKconnection.h>
#include <SCK/SCKutils.h>
#include <SCK/SCKerror.h>
#include <SCK/SCKonData.h>

#include <SCKS/SCKSsslSetup.h>
#include <SCKS/SCKSsslDebugCallbacks.h>
#include <SCKS/SCKSsslRead.h>
#include <SCKS/SCKSsslWrite.h>
#include <SCKS/SCKSsslContext.h>
#include <SCKS/SCKSsslConnection.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

SCKSsslConnection::SCKSsslConnection(SCKSsslContext* pSslContext, SCKloop& Loop, int SocketHandle, int BufferInCapacity)
: SCKconnection(Loop, SocketHandle, BufferInCapacity), m_pContext(pSslContext){
   COL_METHOD(SCKSsslConnection::SCKSsslConnection);
   m_pSSL = NULL;
   m_pInBio = NULL;
   m_pOutBio = NULL;
   m_FirstData = true;
   SCKSsslSetup(m_pContext, &m_pSSL, &m_pInBio, &m_pOutBio);  // This can throw
   SCKSsslDebugRegister(m_pSSL, m_pContext->isServerMode());
}

SCKSsslConnection::~SCKSsslConnection(){
   COL_METHOD(SCKSsslConnection::~SCKSsslConnection);
   COL_VAR(m_pSSL);
   SSL_free(m_pSSL);
   // BIOs are freed automatically by SSL_free
}

void SCKSsslConnection::onData(){
   COL_METHOD(SCKSsslConnection::onData);
   m_CallbackStack++;
   if (m_FirstData && m_pOnRawData.get()) { m_pOnRawData->run(&m_BufferIn, this); }
   int OriginalSize = m_PlainBufferIn.size();
   int ReturnCode = SCKSsslRead(m_pInBio, m_pSSL, &m_PlainBufferIn, &m_BufferIn);
   // m_TotalRead is incremented when SCKconnection reads the encrypted data
   // and incremented here again for the plaintext version of the same data?
   m_TotalRead += m_PlainBufferIn.size() - OriginalSize;
   int SSLerror = SCKSsslCheckError(m_pSSL, ReturnCode, &m_SslError);
   if      (SSLerror == SSL_ERROR_NONE)        {}
   else if (SSLerror == SSL_ERROR_WANT_WRITE)  {COL_TRC("Still shaking hands. Calling write."); write("",0); }
   else if (SSLerror == SSL_ERROR_WANT_READ)   {COL_TRC("Want read.  So we calling write.");    write("",0); }
   else {
      SCKSreportFatalError(SSLerror, ReturnCode);
      hardClose();
      m_CallbackStack--;
      return;
   }
   m_FirstData = false;
   if (m_PlainBufferIn.size() > OriginalSize){
      SCKonData(this);
   }
   m_CallbackStack--;
}
void SCKSsslConnection::write(const char* pBuffer, int SizeOfBuffer){
   COL_METHOD(SCKSsslConnection::write);
   COL_DUMP("Write", pBuffer, SizeOfBuffer);
   m_CallbackStack++;
   COL_VAR(SizeOfBuffer);
   m_PlainBufferOut.append(pBuffer, SizeOfBuffer);
   // TODO: Possible optimization here. Too much space is allocated.
   if (m_ImmediateOut.is_null()){
      COL_TRC("Immediate out is clear so we can try output the new data.");
      m_ImmediateOut.append(m_PlainBufferOut.c_str(), m_PlainBufferOut.size());
      m_PlainBufferOut.clear();
   }
   COL_VAR2(m_PlainBufferOut.size(), m_ImmediateOut.size());
   int SSLerror = -1;
   if (!m_ImmediateOut.is_null()){
      int SSLrc = SSL_write(m_pSSL, m_ImmediateOut.c_str(), m_ImmediateOut.size()); // Must be called with the same arguments each time.
      COL_TRC("Encrypted " << SSLrc << " bytes.");
      SSLerror = SCKSsslCheckError(m_pSSL, SSLrc, &m_SslError);
      if      (SSLerror == SSL_ERROR_NONE)       { COL_TRC("SSL write successful. Dumping Out Buffer."); m_ImmediateOut.clear(); }
      else if (SSLerror == SSL_ERROR_WANT_READ)  { COL_TRC("Still shaking hands. Leaving data in Out Buffer");}
      else if (SSLerror == SSL_ERROR_WANT_WRITE) { COL_TRC("No idea how to handle this. Treating as SSL_ERROR_WANT_READ."); }
      else {
         SCKSreportFatalError(SSLerror, SSLrc);
         hardClose();  // TODO - is this safe
         m_CallbackStack--;
         return;
      }
   }
   SCKSsslWrite(m_pOutBio, this);
   m_CallbackStack--;
}