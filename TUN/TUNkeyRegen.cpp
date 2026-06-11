
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: TUNkeyRegen.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   27/01/25 1:42 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <TUN/TUNkeyConvert.h>
#include <TUN/TUNkeyEllipticalGen.h>
#include <TUN/TUNkeyRegen.h>
#include <TUN/TUNstateClient.h>
COL_LOG_MODULE;

// /tunnel/key/regen
void TUNkeyRegen(const COLwebRequest& Request, TUNstateClient* pClient) {
   try {
      const COLstring TunKeyFile = DIRfileConfigTunnelKey();
      TUNkey Key;
      TUNgenerateEllipticalKey(&Key);
      FILmakeFullDir(FILpathDir(TunKeyFile), FIL_USER_RWX);
      FILwriteFile(TunKeyFile, TUNkeyPrivateAsPemString(Key));
      COLrespondSuccess(Request.Address);
   } catch(const COLerror& E) {
      COL_TRC(E.description());
      COLrespondError(Request.Address, E.description());
   }
}