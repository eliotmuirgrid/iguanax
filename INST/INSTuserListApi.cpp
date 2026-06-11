// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTuserListApi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Sunday 16 July 2023 - 04:38PM
// ---------------------------------------------------------------------------

#include <INST/INSTuserListApi.h>
#include <INST/INSTuserList.h>

#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /install/user/list
void INSTuserListApi(const COLwebRequest& Request){
   COL_FUNCTION(INSTuserListApi);
   COLarray<COLstring> List;
   INSTuserList(&List);
   COLvar Result;
   for (auto i=List.begin(); i != List.end(); i++){
      Result.push_back(*i);
   }
   COL_VAR(Result.json(1));
   COLrespondSuccess(Request.Address, Result);
   
}