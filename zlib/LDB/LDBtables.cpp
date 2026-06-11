// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LDBtables
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 09 November 2022 - 08:24AM
// ---------------------------------------------------------------------------
#include <LDB/LDBtables.h>
#include <LDB/LDBcache.h>

#include <NTAB/NTABschema.h>
#include <NTAB/NTABcollection.h>

#include <FIL/FILutils.h>

#include <LND/LNDutils.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAutils.h>
#include <LUA/LUAopen.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int LDBloadSchemaTables(lua_State* L){
   COL_FUNCTION(LDBloadSchemaTables);
   COLstring RootDir, Vdb;
   RootDir = LUAprojectRoot(L);
   Vdb = LUAoptionalStringParam(L, 1, "vmd", ""); // for compatibility, accept vmd as the vdb argument
   if (Vdb == "") { Vdb = LUAoptionalStringParam(L, 1, "vdb", ""); } // also checks for vdb argument if vmd wasn't present
   if (Vdb == "") { LUA_ERROR_STREAM ("Parameter 'vdb' or 'vmd' is required."); }
   Vdb = RootDir + Vdb;
   COL_TRC("Using vdb file: ");
   COL_VAR(Vdb);
   if (!FILfileExists(Vdb)){ 
      LUAraiseLuaError(L, "Unable to find " + Vdb);
      return 0;
   }
   // We get pSchema from the LND_GRAMMAR_CACHE. It is created on heap and are destroyed when the table
   // (and the lua state) are destroyed.
   bool LegacyWarning = false;
   NTABschema* pSchema = LDBgetDbSchema(L, Vdb, &LegacyWarning); 
   COLstring Error;
   if(pSchema->countOfCollection() == 0){
      LUAraiseLuaError(L, "No tables are defined.");
      return 0;
   }
   const NTABcollectionG* pTableCollection = NTABcollectionByName(*pSchema, "Tables");
   COL_VAR(*pTableCollection);
   COLref<NODplace> pNode = new NTABcollection(pTableCollection);
   LNDpushNode(L, pNode.get());
   if (LegacyWarning) {
      COLstring Warning = "⚠️Warning: Legacy vmd file format detected. It is recommended to use the importer and convert the file to the new format for future use.";
      COL_TRC("Returned the table node and the legacy vmd file warning.");
      LUApushString(L, Warning);
      return 2; // node + warning
   } else {
      COL_TRC("Returned the table node.");
      return 1; // number of results
   }

}
