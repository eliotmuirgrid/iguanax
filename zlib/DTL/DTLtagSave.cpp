//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DTLtagSave.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/03/24 12:27 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <DIR/DIRpath.h>
#include <DTL/DTLtagSave.h>
#include <FIL/FILutils.h>
#include <HASH/HASHutils.h>
COL_LOG_MODULE;

void DTLtagSave(const COLwebRequest& Request) {
   COL_FUNCTION(DTLtagSave);
   COL_VAR(Request.Data);
   const COLstring Username = Request.User;
	const COLstring UserInterfaces = DIRconfigUserDir(Request.User) + "filter_tags.json";
	const COLvar& Interfaces		 = COLvarRequiredObject(Request.Data, "filters");
	COLarray<COLstring> Vec;
   if(!HASHvarToArr(Interfaces, &Vec)) { return COLrespondError(Request.Address, HASHparseError()); }
   COLvar SaveData;
   SaveData.setArrayType();
   for(const auto& it : Vec) {
      COL_VAR(it);
      SaveData.push_back(it);
   }
	COLstring Error;
	const bool Success = FILsave(UserInterfaces, SaveData.json(1), &Error);
	Success ? COLrespondSuccess(Request.Address, SaveData) : COLrespondError(Request.Address, Error);
}