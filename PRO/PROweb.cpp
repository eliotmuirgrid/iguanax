// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PROweb
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 14 July 2023 - 11:57AM
// ---------------------------------------------------------------------------

#include <PRO/PROweb.h>
#include <PRO/PROexecute.h>

#ifdef _WIN32
#include <COL/COLwindows.h>
#include <shellapi.h>
#endif

#include <stdlib.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool PROweb(const COLstring& Url){
   COL_FUNCTION(PROweb);
   COL_VAR(Url);
   COL_VAR(getenv("PATH"));  // Useful to see if a screwed up path with say a space in it breaks things.
   COLstring Command;
#ifdef _WIN32
   Command += "start";
#elif __linux__
   Command += "gio open";
#elif __APPLE__
   Command += "open";
#endif
#ifdef _WIN32
   ShellExecute(NULL, "open", Url.c_str(), NULL, NULL, SW_SHOWNORMAL);
   return true;  // TODO - can we error check
#else
   Command += " " + Url;
   COLstring Out, Err;
   int ExitCode = PROexecuteString(".", Command, &Out, &Err);
   COL_VAR3(ExitCode, Out, Err);
   return ExitCode == 0;
#endif
}
