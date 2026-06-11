// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBportChange
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 11 July 2023 - 04:34PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <IWEB/IWEBconfig.h>
#include <IWEB/IWEBload.h>
#include <IWEB/IWEBportChange.h>
#include <IWEB/IWEBsave.h>
#include <LINK/LINKconstants.h>
#include <POP/POPcolor.h>
COL_LOG_MODULE;

void IWEBportChangeInfo(COLostream& Out) {
   COL_FUNCTION(IWEBportChangeInfo);
   Out << newline << "To change the port this instance is listening on use:" << newline << newline
       << "   iguana --set_web_port <port>" << newline << newline;
   COLstring Link = POPpop(LINKiwebPortChange);
   Out << "See: " << Link << newline;
}

void IWEBportChange(int NewPort) {
   COL_FUNCTION(IWEBportChange);
   IWEBconfig Config;
   COLstring  Error;
   if(!IWEBload(&Config, &Error)) {
      COLcout << Error << newline;
      return;
   }
   Config.setWebPort(COLintToString(NewPort));
   IWEBsave(Config);
   COLcout << "Changed port to " << NewPort << newline;
}
