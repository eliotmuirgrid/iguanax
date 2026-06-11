#ifndef __TUN_KEY_LOAD_OR_GENERATE_H__
#define __TUN_KEY_LOAD_OR_GENERATE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNkeyLoadOrGenerate
//
// Description:
//
// Will load a new private key.  If it doesn't exist, it will generate it.
//
// Author: Eliot Muir 
// Date:   Thursday 29 June 2023 - 02:05PM
// ---------------------------------------------------------------------------

class COLstring;
class TUNkey;

void TUNkeyLoadOrGenerate(TUNkey* pKey);

#endif // end of defensive include
