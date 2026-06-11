#ifndef __SCKS_SSL_CONNECTION_H__
#define __SCKS_SSL_CONNECTION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKSsslConnection
//
// Author: Eliot Muir
//
// Description:
//
// This class inherits from the plain SCKconnection.  It overrides some
// key methods to implement SSL/TLS using Open SSL.  I rewrote this code after
// starting with some code from Kevin Kai.
//
//---------------------------------------------------------------------------

#include <SCK/SCKconnection.h>

class SCKSsslContext;

// Pre-declare openssl stuff to avoid pulling in the openssl headers.
struct bio_st;
typedef struct bio_st BIO;

struct ssl_st;
typedef struct ssl_st SSL;

class SCKSsslConnection : public SCKconnection{
public:
   // Have at least 17000 for buffer size because SSL records are at least 16384.
   SCKSsslConnection(SCKSsslContext* SslContext, SCKloop& Loop, int SocketHandle = -1, int BufferInCapacity = 17000);
   virtual ~SCKSsslConnection();

   virtual COLstring& bufferIn() { return m_PlainBufferIn; }
   virtual void write(const char* pBuffer, int SizeOfBuffer);
   virtual void onData();
   virtual bool sslEnabled() const { return true; }
   void setOnRawData(COLauto<COLclosure2<const COLstring*, SCKconnection*>> pOnRawData) { m_pOnRawData = pOnRawData; }
   const COLstring& sslError() { return m_SslError; };
private:
   COLstring m_PlainBufferIn;
   COLstring m_PlainBufferOut;
   // SSL_write is finicky in that if a write() cannot be fully completed it must be called with the same
   // arguments. This stores the incompletely written message until the operation can be completed.
   COLstring m_ImmediateOut;
   COLstring m_SslError;

   SCKSsslContext* m_pContext; // Not owned

   SSL* m_pSSL; // Owned
   // The following bios are deleted when SSL is freed, as long as their attached to the SSL object.
   BIO* m_pInBio;  // Owned - SSL reads from this so we write to this.
   BIO* m_pOutBio; // Owned - Opposite of above.
   bool m_FirstData; // used to detect HTTP requests
   // const COLstring* - raw TCP buffer, m_BufferIn is private
   // SCKconnection* - so the caller can do something about it
   COLauto<COLclosure2<const COLstring*, SCKconnection*> > m_pOnRawData;
   SCKSsslConnection(const SCKSsslConnection& Orig); // not allowed
   SCKSsslConnection& operator=(const SCKSsslConnection& Orig); // not allowed
};

#endif // End of defensive include