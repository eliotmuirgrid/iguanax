//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testApi
//
// Description:
//
// Implementation
//
// Author: Julian Muir
// Date:   April 9th Friday 2021
//---------------------------------------------------------------------------
#include "testApi.h"

#include <IDB/IDBdatabaseConfig.h>

#include <UNIT/UNITapp.h>

// #include <DB/DBdatabase#Factory.h>

#include <FIL/FILutils.h>

#include <DIR/DIRpath.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// static in this context means the function is only visible in this file
// and not exposed outside by the linker - which makes sense for unit tests.
static void testSimple(){
   COL_FUNCTION(testSimple);
   COLstring Output = "<--Fred-->";
   UNIT_ASSERT_EQUALS("<--Fred-->", Output);
}

static void testComplex(){
   COL_FUNCTION(testComplex);
}

// JULIAN commenting out old tests to remove DBdatabase#Factory (fix test later)

// static void testQuery(){
//    COL_FUNCTION(testQuery);
//    DBdatabase#Factory Factory;
//    Factory.initialize(COLcout);
//    COLvar Input, Output;
//    IDBquery(Input, &Output);
//    COL_VAR(Output.json(1));
// }

static void testSaveConfig(){
   COL_FUNCTION(testSaveConfig);
   FILsetWorkingDir(DIRconfigDirDefault());         // set the default config dir for the test
 
   IDBdatabaseConfig Config;
   // test with "sensible" fake data values
   Config.mySql().setPreferredMySqlDll("/usr/local/mysql/lib/libmysqlclient.dylib");
   Config.oracle().setPreferredOracleDll("/Users/julianmuir/lib/instantclient_12_2/libclntsh.dylib");
   Config.odbc().setPreferredOdbcDll("/usr/local/Cellar/unixodbc/2.3.9/lib/libodbc.dylib");
   // uncomment and make to test with SILLY fake data
   // Config.mySql().setPreferredMySqlDll("This is just some nonsense value ");
   // Config.oracle().setPreferredOracleDll("This is just some nonsense value ");
   // Config.odbc().setPreferredOdbcDll("This is just some nonsense value ");
   
   // check the settings before saving
   COLcout << "MySQL "  << Config.mySql().preferredMySqlDll()  << newline;
   COLcout << "Oracle " << Config.oracle().preferredOracleDll() << newline;
   COLcout << "odbc "   << Config.odbc().preferredOdbcDll()   << newline;
   IDBsaveConfigFile(&Config);
   // UNIT_ASSERT(1==0);
}

static void testReadConfig(){
   COL_FUNCTION(testSaveConfig);
   FILsetWorkingDir(DIRconfigDirDefault());          // set the default config dir for the test
 
   IDBdatabaseConfig Config;
   IDBreadConfigFile(&Config);
   // check the settings after reading file
   COLcout << "MySQL "  << Config.mySql().preferredMySqlDll()  << newline;
   COLcout << "Oracle " << Config.oracle().preferredOracleDll() << newline;
   COLcout << "odbc "   << Config.odbc().preferredOdbcDll()   << newline;
   // UNIT_ASSERT(1==0);
}


void testApi(UNITapp* pApp){
   // pApp->add("/database/settings/simple", &testSimple);
   // pApp->add("/database/settings/complex", &testComplex);
   // pApp->add("/database/settings/testQuery", &testQuery); // JULIAN uses DBdatabase#Factory - ooopppsss...
   // pApp->add("/database/settings/testSaveConfig", &testSaveConfig); // JULIAN windows Permission denied cannot write file
   pApp->add("/database/settings/testReadConfig", &testReadConfig);
}           
