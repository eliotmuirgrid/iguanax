// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUknownHost
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 16 January 2023 - 11:04AM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <FIL/FILpathUtils.h>
#include <GITU/GITUknownHost.h>
#include <PRO/PROexecute.h>
COL_LOG_MODULE;

// not used currently - but nice to have the recipe for this.
bool GITUgetKnownHostKey(const COLstring& Host, COLstring* pKey, COLstring* pError){
   COL_FUNCTION(GITUgetKnownHostKey);
   COLstring Output, Err;
   int ExitCode = PROexecuteString("", "ssh-keyscan " + Host, &Output, &Err);
   COL_VAR2(ExitCode,Output);
   COL_DUMP("Key scan output:", Output.c_str(), Output.size());
   COL_VAR(Output.substr(0, Host.size()));
   if (ExitCode != 0 || Output.substr(0, Host.size()) != Host) { 
      *pError = "FAILED:\n" + Output;
      return false;
   }
   ExitCode = PROexecuteString("", "ssh-keyscan Host", &Output, &Err);

   return true;
}

COLstring GITUknownHostFile(){
   COL_FUNCTION(GITUknownHostFile);
   COLstring FileName = FILexpand("~/.ssh");
   FileName = FILpathAppend(FileName, "known_hosts");
   COL_VAR(FileName);
   return FileName;
}