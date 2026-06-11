//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINRrestart.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   17/12/24 10:45 AM
//  ---------------------------------------------------------------------------
#include <APP/APPmain.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
#include <GINR/GINRrestart.h>
#include <GINR/GINRrestart.h>
#include <PRO/PROlaunch.h>
#include <SDM/SDMshutdown.h>
#include <PRM/PRMcallbacks.h>
COL_LOG_MODULE;

void GINRrestart(const COLwebRequest& Request) {
   COL_FUNCTION(GINRrestart);
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", MANAGE_INSTANCE, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(MANAGE_INSTANCE));
   }
   COLrespondSuccess(Request.Address);
#ifdef _WIN32
   const COLstring CurrentBinary = FILexecutableDir() + FILexecutableFileName();
   const COLstring Cmd           = CurrentBinary + " --service restart";
   COL_VAR2(FILexecutableDir(), Cmd);
   int ProcessId = PROlaunch(FILexecutableDir(), Cmd);
#elif __APPLE__
   APPkillMacController();
   SDMshutdown();
#else
   SDMshutdown();
#endif
}
