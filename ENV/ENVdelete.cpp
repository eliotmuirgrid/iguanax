//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ENVdelete.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/28/23 9:31 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <ENV/ENVdelete.h>
#include <ENV/ENVfile.h>
#include <ENV/ENVparse.h>
#include <ENVC/ENVCcache.h>
#include <PRM/PRMcallbacks.h>
#include <PRO/PROenv.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

// /environment/delete
void ENVdelete(const COLwebRequest& Request){
   COL_FUNCTION(ENVdelete);
   const COLvar& Input = Request.Data;
   COL_VAR(Input);
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::ENV_VARIABLES, &Error)) {
		return COLrespondError(Request.Address, PRMsetError(ROLid::ENV_VARIABLES));
	}
	const COLstring Key = COLvarRequiredString(Input, "key");
   ENVmap LocalEnvs;
   ENVfileLoad(&LocalEnvs);
   LocalEnvs.Map.erase(Key);
   ENVfileSave(LocalEnvs);
   PROunsetenv(Key);
   ENVCcacheUnset(Key);
   COLvar Result;
   ENVfileToVar(LocalEnvs, &Result["local"]);
   COLrespondSuccess(Request.Address, Result);
}