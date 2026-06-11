//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBdatabaseConfig
//
// Description:
//
// A configuration object for the various databases.
//
// Author: Julian Muir
// Date:   Mon 20 Apr 2021
//---------------------------------------------------------------------------
#include "IDBdatabaseConfig.h"
#include "IDBmySql.h"
#include "IDBoracle.h"
#include "IDBodbc.h"
#include "IDBsqlite.h"
#include "IDBmariaDb.h"

#include <DIR/DIRpath.h>

#include <FIL/FILatomic.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLstring.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool IDBreadConfigFile(IDBdatabaseConfig* pConfig){
   COL_FUNCTION(IDBreadConfigFile);
   COLvar ConfigVar;
   bool Success  = FILserializeLoadConfig(&ConfigVar, DIRfileConfigDatabase());
   if (Success){
      pConfig->mySql().setPreferredMySqlDll   (ConfigVar["mysql"]);
      pConfig->oracle().setPreferredOracleDll (ConfigVar["oracle"]);
      pConfig->odbc().setPreferredOdbcDll     (ConfigVar["odbc"]);
   }
   return Success;
}

void IDBsaveConfigFile(IDBdatabaseConfig* pConfig){
   COL_FUNCTION(IDBsaveConfigFile);
   COLvar ConfigVar;
   ConfigVar["mysql"]  = pConfig->mySql().preferredMySqlDll();
   ConfigVar["oracle"] = pConfig->oracle().preferredOracleDll();
   ConfigVar["odbc"]   = pConfig->odbc().preferredOdbcDll();
   FILserializeSaveConfig(ConfigVar, DIRfileConfigDatabase());
   COL_VAR(ConfigVar["mysql"]);
}