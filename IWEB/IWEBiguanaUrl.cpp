//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBiguanaUrl
//
// Description:
//
// Implementation
//
// Author: Shuo Hou
// Date:   Thu 11 March 2021 2:01:05 EDT
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLstring.h>
#include <FIL/FILdirEnumerator.h>
#include <IWEB/IWEBconfig.h>
#include <IWEB/IWEBiguanaUrl.h>
#include <IWEB/IWEBload.h>
#include <POP/POPcolor.h>
COL_LOG_MODULE;

COLstring IWEBiguanaUrl(const IWEBconfig& WebConfig, const COLstring& Host){
   COL_FUNCTION(IWEBiguanaUrl);
   COLstring Url;
   WebConfig.useHttps() ? Url += "https" : Url += "http";
   Url += "://" + Host + ":";  // IX-2073 We tried to use SCKlocalHostName but it is a mess on many platforms like Mac
   Url += WebConfig.webPort();
   Url += "/";
   COL_VAR(Url);
   return Url;
}

COLstring IWEBiguanaUrl(const COLstring& Host) {
   COL_FUNCTION(IWEBiguanaUrl);
   COLstring Url;
   IWEBconfig Config;
   COLstring Error;
   if (IWEBload(&Config, &Error)){
      COLstring Result = IWEBiguanaUrl(Config, Host);
      COL_VAR(Result);
      return Result;
   }
   COL_TRC("Could not find the URL.");
   return COL_T("http://") + Host + COL_T(":7654/");
}

void IWEBoutputListeningAndHelp(){
   COL_FUNCTION(IWEBoutputListeningAndHelp);
   COLcout << "Verify it is running by using this command:" << newline;
   COLcout << "   " << POPiguanaCommandLine() << " --service status" << newline;
}
