#ifndef __SCKS_SSL_WRITE_H__
#define __SCKS_SSL_WRITE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKSsslWrite
//
// Description:
//
// Routine to read from output BIO from SSL to the real socket.
//
// Author: Eliot Muir 
// Date:   Wednesday 21 June 2023 - 02:52PM
// ---------------------------------------------------------------------------

struct bio_st;
typedef struct bio_st BIO;

class SCKconnection;
class COLstring;

void SCKSsslWrite(BIO* pOutBio, SCKconnection* pConnection);

#endif // end of defensive include
