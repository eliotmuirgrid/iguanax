//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLload.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/01/24 12:57 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <ROL/ROLload.h>
#include <ROL/ROLrole.h>
#include <ROL/ROLserialize.h>
COL_LOG_MODULE;

void ROLload(ROLmap* pRoleMap) {
   COL_FUNCTION(ROLload);
   const COLstring RoleFile = DIRfileConfigRoles();
   if(!FILfileExists(RoleFile)) { return; }
   try {
      COLstring Config;
      FILreadFile(RoleFile, &Config);
      COLvar RoleMapVar;
   	if(RoleMapVar.parse(Config)) {
   		if(ROLvarToMap(RoleMapVar, pRoleMap)) { return; }
   		COL_ERR("Failed to load role var into map");
   	} else {
   		COL_ERR("Invalid format detected -- parse failed");
   	}

   }catch (const COLerror& E) {
      COL_ERR(E.description());
   }
}