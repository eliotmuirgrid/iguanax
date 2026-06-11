// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PASScreateSalt
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 21 September 2022 - 05:18PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <FIL/FILatomic.h>
#include <FIL/FILutils.h>
#include <GUID/GUIDid.h>
#include <PASS/PASScreateSalt.h>
#include <PASS/PASSword.h>
COL_LOG_MODULE;

void PASScreateSalt(){
   COL_FUNCTION(PASScreateSalt);
   COLvar Encrypt;
   if(!FILfileExists(DIRfileConfigEncrypt())) {
      COL_TRC("Creating new encrypt file.");
      Encrypt["key"] = GUIDnewId(32);
      Encrypt["iv"]  = GUIDnewId(16);
      FILserializeSaveConfig(Encrypt, DIRfileConfigEncrypt());
   } else {
      COL_TRC("Loading existing one.");
      FILserializeLoadConfig(&Encrypt, DIRfileConfigEncrypt());
   }
   PASSsetKeyIV(Encrypt["key"], Encrypt["iv"]);
}