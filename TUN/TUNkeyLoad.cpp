// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNkeyLoad
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 03:20PM
// ---------------------------------------------------------------------------

#include <TUN/TUNkeyLoad.h>
#include <TUN/TUNkey.h>
#include <TUN/TUNkeyConvert.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNkeyLoad(const COLstring& FileName, TUNkey* pKey){
   COL_FUNCTION(TUNkeyLoad);
   COLstring Content;
   FILreadFile(FileName, &Content);
   TUNkeyPrivateFromPemString(Content, pKey);
}

void TUNkeyPublicLoad(const COLstring& FileName, TUNkey* pKey){
   COL_FUNCTION(TUNkeyPublicLoad);
   COLstring Content;
   FILreadFile(FileName, &Content);
   TUNkeyPublicFromPemString(Content, pKey);
}