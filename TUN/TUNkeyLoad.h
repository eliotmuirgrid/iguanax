#ifndef __TUN_KEY_LOAD_H__
#define __TUN_KEY_LOAD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNkeyLoad
//
// Description:
//
// Load a PEM private key off disc
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 03:20PM
// ---------------------------------------------------------------------------

class COLstring;
class TUNkey;

void TUNkeyLoad(const COLstring& FileName, TUNkey* pKey);
void TUNkeyPublicLoad(const COLstring& FileName, TUNkey* pKey);

#endif // end of defensive include
