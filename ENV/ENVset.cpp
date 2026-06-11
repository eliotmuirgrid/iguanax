//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ENVset.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/27/23 1:01 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLutils.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <ENV/ENVfile.h>
#include <ENV/ENVparse.h>
#include <ENV/ENVset.h>
#include <ENVC/ENVCcache.h>
#include <PRM/PRMcallbacks.h>
#include <PRO/PROenv.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

// /environment/set_local
void ENVsetLocal(const COLwebRequest& Request){
   COL_FUNCTION(ENVsetLocal);
   COL_VAR(Request.Data);
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::ENV_VARIABLES, &Error)) {
		return COLrespondError(Request.Address, PRMsetError(ROLid::ENV_VARIABLES));
	}
	const COLstring OriginalKey = COLvarRequiredString(Request.Data, "original_key");
	const COLstring NewKey		 = COLvarRequiredString(Request.Data, "new_key");
	COLstring		 Value		 = COLvarRequiredString(Request.Data, "value");
   Value = Value.strip(COLstring::Both, ' ');
   Value = Value.strip(COLstring::Both, '\t');
   ENVmap LocalEnvs;
   ENVfileLoad(&LocalEnvs);
   LocalEnvs.Map.erase(OriginalKey); // if we're renaming the variable
   LocalEnvs.Map[NewKey] = Value;
   ENVfileSave(LocalEnvs);
   COLstring ExpandedValue = COLexpandEnvironmentVariables(Value);
   PROunsetenv(OriginalKey);
   PROsetenv(NewKey, ExpandedValue);
   ENVCcacheUnset(OriginalKey);
   ENVCcacheSet(NewKey, ExpandedValue);
   COLrespondSuccess(Request.Address);
}