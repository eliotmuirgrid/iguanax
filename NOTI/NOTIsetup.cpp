// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOTIsetup
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 19 September 2022 - 03:20PM
// ---------------------------------------------------------------------------

#include <NOTI/NOTIsetup.h>
#include <NOTI/NOTImodel.h>

#include <DIR/DIRpath.h>

#include <FIL/FILatomic.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void NOTIinit(NOTImodel* pNotifications){
   COL_FUNCTION(NOTIinit);
   COLvar Config;
   if (FILserializeLoadConfig(&Config, DIRfileConfigNotifications())){  // File IO on main thread
      COL_TRC("Loaded notification config.");
      COL_VAR(Config.json(1));
      NOTImodelFromDiscVar(pNotifications, Config);
   } else {
      COL_TRC("Could not load the notification code so setting up defaults.");
      NOTIcreateDefault(pNotifications);
   }
}