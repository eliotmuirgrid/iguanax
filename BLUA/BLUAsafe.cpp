//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAsafe
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wed 24 Feb 2021 14:59:03 EST
//---------------------------------------------------------------------------
#include "BLUAsafe.h"

#include <COL/COLlog.h>

static bool g_BLUAisSafe = false;

bool BLUAsafeMode(){
   return g_BLUAisSafe;
}

void BLUAsetSafeMode(bool Value){
   g_BLUAisSafe = Value;
}
