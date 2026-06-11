//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNcountOfLocals
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday, November 18th, 2010 @ 02:43:52 PM
//---------------------------------------------------------------------------
#include "TRNcountOfLocals.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

int TRNcountOfLocals(lua_State* L, lua_Debug* pD){
   COL_FUNCTION(TRNcountOfLocals);
   int LocalIndex = 0, CountOfLocal = 0;
   const char* pLocalVarName = NULL;
   while ((pLocalVarName = lua_getlocal(L, pD, ++LocalIndex))){
      if (strcmp(pLocalVarName, "(*temporary)") != 0){
         CountOfLocal++;
         COL_TRC(pLocalVarName << " => local");
      }
      else {
         COL_TRC(pLocalVarName << " => NOT a local");
      }
      lua_pop(L, 1);
   }
   COL_VAR(CountOfLocal);
   return CountOfLocal;
}
