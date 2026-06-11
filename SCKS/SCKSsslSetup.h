#ifndef __SCKS_PREPARE_H__
#define __SCKS_PREPARE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKSsslSetup
//
// Description:
//
// Initialization routine for Open SSL.
//
// Author: Eliot Muir 
// Date:   Wednesday 21 June 2023 - 10:29AM
// ---------------------------------------------------------------------------

struct bio_st;
typedef struct bio_st BIO;

struct ssl_st;
typedef struct ssl_st SSL;

class SCKSsslContext;

void SCKSsslSetup(SCKSsslContext* m_pContext, SSL** m_ppSSL, BIO** m_ppInBio, BIO** m_ppOutBio);

#endif // end of defensive include