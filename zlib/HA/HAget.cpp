//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HAget.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <HA/HAget.h>
COL_LOG_MODULE;

static COLvar createDefault() {
   COL_FUNCTION(createDefault);
   COLvar Default;
   Default["enabled"]            = false;
   Default["shared_working_dir"] = "";
   Default["type"]               = "";
   return Default;
}

void HAget(const COLwebRequest& Request) {
   COL_FUNCTION(HAget);
   if(!FILfileExists(DIRfileConfigHa())) {
      COL_TRC("Config file does not exist -- use defaults.");
      return COLrespondSuccess(Request.Address, createDefault());
   }
   try {
      COLstring Data;
      FILreadFile(DIRfileConfigHa(), &Data);
      COLvar Config;
      if(!Config.parse(Data)) { return COLrespondError(Request.Address, "Config file malformed."); }
      COLrespondSuccess(Request.Address, Config);
   } catch(const COLerror& E) {
      COL_TRC(E.description());
      COLrespondError(Request.Address, E.description());
   }
}