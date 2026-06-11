#ifndef __BLUA_SAFE_H__
#define __BLUA_SAFE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAsafe
//
// Description:
//
// This global flag means Iguana won't start up the channels in it's startup up routine
// even if they are marked with auto start = true.
//
// Author: Eliot Muir
// Date:   Wed 24 Feb 2021 14:59:03 EST
//---------------------------------------------------------------------------

bool BLUAsafeMode();
void BLUAsetSafeMode(bool Value);

#endif // end of defensive include
