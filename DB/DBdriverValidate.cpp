//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdriverValidate
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thu 15 Apr 2021 14:52:50 EDT
//---------------------------------------------------------------------------
#include "DBdriverValidate.h"

#include <FIL/FILutils.h>

#include <COL/COLostream.h>
#include <COL/COLdll.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool DBloadLibrary(COLdll* Dll, const COLstring& FileName){
   COL_FUNCTION(DBloadLibrary);
   COL_VAR(FileName);
  if (!FILfileExists(FileName)){
      COL_TRC("File does not exist.");
      return false;
   }
   COL_TRC("Driver file exists.");
   int ErrorCode = COL_DLL_LOAD_OK;
   ErrorCode = Dll->tryLoadLibrary(FileName);
   if( !Dll->loaded() ){
      COL_TRC("Could not load driver file.");
      if(ErrorCode == COL_DLL_LOAD_NO_PERMISSIONS){
         COL_TRC("Not enough permissions to load the library.");
      }
      else if(ErrorCode == COL_DLL_LOAD_INVALID_LIBRARY){
         COL_TRC("Library load failed - the file specified is not a valid dynamic library.");
      }
      return false;
   }
   return true;
}

bool DBlibValidateAndLoad (COLdll* Dll, const COLstring& FileName) {return DBloadLibrary(Dll, FileName); }
