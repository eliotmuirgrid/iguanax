#ifndef __SCKS_SSL_READ_H__
#define __SCKS_SSL_READ_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKSsslRead
//
// Description:
//
// Push encrypted TLS data in, get unencrpyted data out.
//
// Author: Eliot Muir
// Date:   Wednesday 21 June 2023 - 12:56PM
// ---------------------------------------------------------------------------

struct bio_st;
typedef struct bio_st BIO;

struct ssl_st;
typedef struct ssl_st SSL;

class COLstring;

int SCKSsslRead(BIO* m_pInBio, SSL* m_pSSL, COLstring* pOut, COLstring* pIn);

int SCKSsslCheckError(SSL* m_pSSL, int ReturnCode, COLstring* pError);

#endif // end of defensive include
