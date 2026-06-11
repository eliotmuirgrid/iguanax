
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITverifyInstanceModules.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   27/05/25 2:27 PM
//  ---------------------------------------------------------------------------
#include <GIT/GITverifyInstanceModules.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <GIT/GITverifyModulesFile.h>
COL_LOG_MODULE;

// /git/verify_instance_modules
void GITverifyInstanceModules(const COLwebRequest& Request) {
   COL_FUNCTION(GITverifyInstanceModules);
   COLstring  Err;
   const bool Valid = GITverifyModulesFile(DIRconfigDir(), &Err);
   COL_VAR2(Valid, Err);
   COLvar     Result;
   if(!Valid) { Result["error"] = Err; }
   COLrespondSuccess(Request.Address, Result);
}

