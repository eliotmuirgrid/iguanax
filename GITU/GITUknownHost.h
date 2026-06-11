#ifndef __GITU_KNOWN_HOST_H__
#define __GITU_KNOWN_HOST_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUknownHost
//
// Description:
//
// Routines to get known host and add it to the list
//
// Author: Eliot Muir 
// Date:   Monday 16 January 2023 - 11:04AM
// ---------------------------------------------------------------------------

class COLstring;

// This function will obtain the key
bool GITUgetKnownHostKey(const COLstring& Host, COLstring* pKey, COLstring* pError);

COLstring GITUknownHostFile();  // returns the path of the known host file.

#endif // end of defensive include