// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LHL7mapping
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Friday 05 September 2025 - 04:18PM
// ---------------------------------------------------------------------------

#include <LHL7/LHL7mapping.h>

#include <EDI/EDIgrammar.h>
#include <EDI/EDIload.h>

#include <LEDI/LEDIutils.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAtable.h>

#include <FIL/FILutils.h>

#include <COL/COLsplit.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring LHL7loadFile(const COLstring& Vmd){
   COL_FUNCTION(LHL7loadFile);
   COLstring Grammar;
   FILreadFile(Vmd, &Grammar);
   return Grammar;
}

static bool LHL7getReplacements(const COLstring& Grammar, bool LongNames, COLvar* pReplacements, COLstring* pLoadError){
   COL_FUNCTION(LHL7getReplacements);
   COLstring Warning;
   bool LegacyWarning;
   EDIcollection Collection;
   // return EDIsmartLoad(Grammar, &Collection, pLoadError, &Warning, &LegacyWarning, pReplacements, LongNames); // commenting out for 10.1.113 release
   return false;
}

int LHL7mapping(lua_State* L){
   COL_FUNCTION(LHL7mapping);
   COLstring Path = LUAoptionalStringParam(L, 1, "path");
   COLstring Vmd  = LEDIresolveVmdFile(L) ;
   bool LongNames = LUAoptionalBoolParam(L, 1, "longname", false);
   if(!FILfileExists(Vmd)) { LUAraiseLuaError(L, "Unable to find " + Vmd); }
   COL_VAR3(Path, Vmd, LongNames);
   COLstring Grammar = LHL7loadFile(Vmd);
   COLstring LoadError;
   COLvar Replacements;
   if (!LHL7getReplacements(Grammar, LongNames, &Replacements, &LoadError)){ return LUAraiseLuaError(L, LoadError); }
   COL_VAR(Replacements.json(1));
   if (Path.is_null()){ // Do we care? maybe just return the whole table?
      COL_TRC("No path provided, returning the whole mapping table");
      Replacements.toLua(L);
      return 1;
   }
   COL_TRC("Descending using path " + Path);
   COLarray<COLstring> Descend;
   COLsplit(&Descend, Path, "/");
   COLvar Now = Replacements;
   COLvar Out;
   for (int i = 0; i != Descend.size(); i++){
      COLstring Key = Descend[i];
      if (Now.isMap() && !Now.map().count(Key)) { return LUAraiseLuaError(L, "The path specified has no mapping"); }
      Now = Now[Key];
   }
   COL_VAR(Now);
   Now.toLua(L);
   return 1;
}
