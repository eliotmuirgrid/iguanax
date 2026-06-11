//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisenseAlterCode
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, December 20th, 2010 @ 02:27:26 PM
//---------------------------------------------------------------------------
#include "TRNintellisenseContext.h"
#include "TRNconstant.h"

#include <SFI/SFIhtmlEscape.h>

#include <LUACX/lauxlib.h>
#include <LUACX/LUAXlexCode.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// TODO - it would be nice if we could close any unfinished loops, etc, to make the code valid.
// For example, if the user types the first line of a for loop, then starts entering the
// body of the loop, intellisense will fail because there is no 'end' keyword at the end of the body.
void TRNintellisenseAlterCode(int LineBegin, const COLstring& iLuaExpression, char IndexChar, COLstring* pCode){
   COL_FUNCTION(TRNintellisenseAlterCode);
   COLstring& LuaCode = *pCode;
   COLstring LuaExpression(iLuaExpression);
   // If we're in an "open function call", like "foo:bar(", replace the last ':' with '.',
   // so we can index properly.
   if (LuaExpression.size() && (IndexChar == '(' || IndexChar == '{')) {
      luax_State* L = lua_open();
      COLstring DummyString;
      bool DummyBool;
      COLauto<LUAXtoken> pTail = LUAXlexCode(L, iLuaExpression, "TRNintellisenseAlterCode", iLuaExpression.size()-1, true, DummyString, DummyBool);
      lua_close(L);
      for(LUAXtoken* pTok = pTail; pTok; pTok = pTok->pNext){
         if(pTok->Token == ':'){
            LuaExpression[ pTok->TokenEnd-1 ] = '.';
            break;
         }
      }
   }

   COL_VAR2(LineBegin, LuaExpression);
   COL_DBG("Code:" << newline << LuaCode);
   COLstring CodeToInsert;
   COLostream CodeToInsertStream(CodeToInsert);
   // By design we put in two instances of the IntellisenseObject = <blah>, one does the assignment, we
   // break on the other one.
   CodeToInsertStream << TRN_GLOBAL_INTELLISENSE_VARIABLE << COL_T(" = ") << LuaExpression << '\n'
                      << TRN_GLOBAL_INTELLISENSE_VARIABLE << COL_T(" = ") << LuaExpression;
   int LineEnd = LuaCode.find('\n', LineBegin);
   
   LuaCode.replace(LineBegin, LineEnd-LineBegin, CodeToInsert);
   COL_TRC("Modified Code:" << newline << LuaCode);
}