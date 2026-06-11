// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNkeyLoadOrGenerate
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 29 June 2023 - 02:05PM
// ---------------------------------------------------------------------------

#include <TUN/TUNkeyLoadOrGenerate.h>
#include <TUN/TUNkeyLoad.h>
#include <TUN/TUNkeyEllipticalGen.h>
#include <TUN/TUNkeyConvert.h>

#include <DIR/DIRpath.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNkeyLoadOrGenerate(TUNkey* pKey) {
   COL_FUNCTION(TUNkeyLoadOrGenerate);
	const COLstring TunKeyFile = DIRfileConfigTunnelKey();
   if (FILfileExists(TunKeyFile)){
      TUNkeyLoad(TunKeyFile, pKey);
   } else {
      TUNgenerateEllipticalKey(pKey);
      FILmakeFullDir(FILpathDir(TunKeyFile), FIL_USER_RWX);
      FILwriteFile(TunKeyFile, TUNkeyPrivateAsPemString(*pKey));
   }
}