#ifndef __DBG_DOWNLOAD_PDB_H__
#define __DBG_DOWNLOAD_PDB_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: Gets PDB out of package and gives information on it.  Was originally a download piece.
//
// Description:
//
// Code to download PDP.  Only really needed on windows but the code works
// on all platforms - easier to develop this way.
//
// Author: Eliot Muir 
// Date:   Sunday 13 August 2023 - 10:23AM
// ---------------------------------------------------------------------------

class COLstring;
class COLwebRequest;

COLstring DBGpdbFileName();

void DBGpdbInfo(const COLwebRequest& Request);

bool DBGextractPdbIfNeeded(COLstring* pFeedback);

#endif // end of defensive include
