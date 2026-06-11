// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDlaunchDashboard
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 22 September 2022 - 03:30PM
// ---------------------------------------------------------------------------

#include <DBD/DBDlaunchDashboard.h>

#include <IWEB/IWEBiguanaUrl.h>

#include <PRO/PROweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool DBDlaunchDashboard() {
   COL_FUNCTION(DBDlaunchDashboard);
   COLcout << "Opening " << IWEBiguanaUrl() << newline;
   if (!PROweb(IWEBiguanaUrl())) {
      COLcout << "Failed to launch Iguana Dashboard." << newline;
      return false;
   } 
   return true;
}
