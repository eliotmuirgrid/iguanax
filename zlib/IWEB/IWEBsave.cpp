// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBsave
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 28 September 2022 - 10:02AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <FIL/FILatomic.h>
#include <IWEB/IWEBsave.h>
#include <IWEB/IWEButils.h>
COL_LOG_MODULE;

void IWEBsave(const IWEBconfig& Config) {
   COL_FUNCTION(IWEBsave);
   const COLvar ToSave = IWEBsetupConfigForSave(Config);
   FILserializeSaveConfig(ToSave, DIRfileConfigWeb());
}