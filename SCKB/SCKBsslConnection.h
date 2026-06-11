#ifndef __SCKB_SSL_CONNECTION_H__
#define __SCKB_SSL_CONNECTION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKBsslConnection
//
// Description:
//
// Simple secure blocking socket connection with timeout.
//
// Author: John Q
// Date:   Tue  1 Nov 2022 22:08:55 EDT
//---------------------------------------------------------------------------
#include <SCKB/SCKBconnection.h>

class   SCKBsslContext;
struct  ssl_st;
struct  bio_st;
typedef struct ssl_st SSL;
typedef struct bio_st BIO;


class SCKBsslConnection : public SCKBconnection {
public:
   SCKBsslConnection(SCKBsslContext* pContext);
   ~SCKBsslConnection();
   SCKBsslConnection(const SCKBsslConnection& Orig) = delete;
   SCKBsslConnection& operator=(const SCKBsslConnection& Orig) = delete;

   bool connect(const COLstring& Host, int Port);
   int  read (COLstring* pBuffer);
   int  write(const COLstring& Buffer);
   void close();

private:
   SCKBsslContext* m_pContext { nullptr };   // Not owned
   SSL*     m_pSsl            { nullptr };
   BIO*     m_pBio            { nullptr };
};

#endif // end of defensive include