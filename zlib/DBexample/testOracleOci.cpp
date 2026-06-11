//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testMariaDb
//
// Description:
//
// Implementation
//
// Author: Julian Muir
// Date:   03 May 2021
//---------------------------------------------------------------------------

#include "testOracleOci.h"
#include "testCredentials.h"

#include <COL/COLstring.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <FIL/FILutils.h>
#include <DB/DBdriverFind.h>
#include <DB/DBdriverValidate.h>
#include <DB/DBdriverOracleOci.h>
#include <DB/DBoracleOci.h> 
#include <DB/OCIoracleInclude/oci.h>

#include <UNIT/UNITapp.h>

static COLstring FindOracleOciLibPath() {
    COL_FUNCTION(FindOracleOciLibPath);
    COLstring Log, Path;
    COLostream LogStream(Log);
    bool Success = DBfindOracleOci(&Path, LogStream);

    COL_VAR3(Log, Path, Success);
    if (Success) {
        return Path;
    }
    return "OCI driver not found";
}

static void testLoadDll() {
    COL_FUNCTION(testLoadDll);

    COLstring LibraryPath;
    LibraryPath = FindOracleOciLibPath();   
    COL_VAR(LibraryPath);

    // JULIAN hard coding library paths to help debug 
    // LibraryPath = "/Users/julianmuir/instantclient_19_8/libclntsh.dylib.19.1";

    DBdriverOracleOci Driver;
    Driver.loadLibrary(LibraryPath);
    COL_VAR(Driver.isLoaded());
    if (!Driver.isLoaded()){
        COL_TRC("Could not load Oracle OCI drivers");
        return;
    }else{COL_TRC("Loaded Oracle OCI drivers");}

    static const int DB_OCI_CHARSET_UTF8 = 871;
    OCIEnv*          pEnvironmentHandle;
    sword Result = Driver.oci_env_nls_create(
        &pEnvironmentHandle,                   // NOTE using NULL does not work :)
        OCI_DEFAULT, // Mode
        0, // pContext
        0, // pMalocFunc
        0, // pRalocFunc
        0, // pMfreeFunc
        0, // ExtraMemSize
        0, // pUsermem
        DB_OCI_CHARSET_UTF8, // CharSet
        DB_OCI_CHARSET_UTF8  // NCharSet
    );
    if (Result != OCI_SUCCESS) {
        COL_TRC("Could not initialize Oracle OCI object");
    } else { COL_TRC("Initialized Oracle OCI object"); }
}

// JULIAN this test still needs work as the connection code is work in progress
// JULIAN the connection still works with the timeout and stopped Oracle

// This tests that OCI can connect to the Oracle database
// but it DOES NOT check that the connection is working
// another test is needed for that using hasConnectionByGuid
// OR PERHAPS ADD TO THIS TEST
static void testOciConnect() {
    COL_FUNCTION(testConnect);
   
    COLstring LibraryPath = FindOracleOciLibPath();  
    DBoracleOci OracleOci;
    OracleOci.setDriver(LibraryPath);

    COLvar ConnectionInfo;
    GetCredentials("OracleOci", &ConnectionInfo, true);
    COL_VAR(ConnectionInfo);

    bool Result = OracleOci.connect(ConnectionInfo);

    if (!Result) {
        COL_TRC("Could not connect to Oracle OCI object");
    } else {
        COL_TRC("Connected to Oracle OCI object");
    }
}

static void testexecuteSqlString() {
    COL_FUNCTION(testConnect);
   
    COLstring LibraryPath = FindOracleOciLibPath();  
    DBoracleOci OracleOci;
    OracleOci.setDriver(LibraryPath);

    COLvar ConnectionInfo;
    GetCredentials("OracleOci", &ConnectionInfo, true);
    COL_VAR(ConnectionInfo);

    bool Result = OracleOci.connect(ConnectionInfo);

    if (!Result) {
        COL_TRC("Could not connect to Oracle OCI object");
    } else {
        COL_TRC("Connected to Oracle OCI object");
    }

    COLstring SqlString;
    // SqlString = "SELECT 0 FROM DUAL";
    // SqlString = "SELECT * FROM HR.COUNTRIES";
    // SqlString = "SELECT * FROM HR.COUNTRIES_2";
    SqlString = "SELECT * FROM HR.TEST";
    COLvar pResultSet; 
    COLvar pResultInfo;
    bool SqlResult = OracleOci.executeSqlString(SqlString, &pResultSet, &pResultInfo);
    
    COL_TRC("=============================testexecuteSqlString==============================");
    COL_VAR(pResultInfo);

 
}

void testOracleOci(UNITapp* pApp) {
    // pApp->add("oracle_oci/loadDll", &testLoadDll);
    // pApp->add("oracle_oci/oci_connect", &testOciConnect);
    pApp->add("oracle_oci/execute", &testexecuteSqlString);
    // pApp->add("oracle_oci/lib_test", &testJulianLibStuff);
    // pApp->add("oracle_oci/pointers", &testJulianPointers);
}





// -------------------------------------------------
// JULIAN strange old stuff below here
// -------------------------------------------------


static void testJulianLibStuff() {
    COL_FUNCTION(testJulianLibStuff);

    COLstring test = "Julian was here testJulianLibStuff";
    COL_VAR(test);

    // JULIAN hard coding library paths to help debug 
    COLstring LibraryPath;
    LibraryPath = "/Users/julianmuir/instantclient_19_8/libclntsh.dylib.19.1"; // OCI
    LibraryPath = "/Users/julianmuir/instantclient_19_8/libocci.dylib.19.1";   // OCCI

    if (!FILfileExists(LibraryPath)){
        COL_TRC("File does not exist.");
    }

    COLdll Dll;
    int ErrorCode = COL_DLL_LOAD_OK;
    COL_TRC("======================================================================");
    ErrorCode = Dll.tryLoadLibrary(LibraryPath);
    // DBlibValidateAndLoad(&Dll, LibraryPath); // alternate call...
    COL_TRC("======================================================================");

    if( !Dll.loaded() ){
        COL_TRC("Could not load driver file.");
        if(ErrorCode == COL_DLL_LOAD_NO_PERMISSIONS){
            COL_TRC("Not enough permissions to load the library.");
        }
        else if(ErrorCode == COL_DLL_LOAD_INVALID_LIBRARY){
            COL_TRC("Library load failed - the file specified is not a valid dynamic library.");
        }
    }
    COL_TRC("=================================SUCCESS=====================================");
}

static void testJulianPointers() {
    COL_FUNCTION(testJulianPointers);

    COLstring test = "Julian was here";
    COL_VAR(test);
    // COL_TRC("=================================");


   
//     char* pNULL    = NULL;
//     char* p0       = 0;
//     char* pNullptr = nullptr;

//     COL_VAR(NULL);
//     COL_VAR((long)nullptr);
//     COL_TRC("=================================");


//     if (!pNULL) {
//         COL_TRC("!pNULL    works");
//     }
//     if (!p0) {
//         COL_TRC("!p0       works");
//     }
//     if (!pNullptr) {
//         COL_TRC("!pNullptr works");
//     }
//     if (pNullptr == NULL) {
//         COL_TRC("pNullptr == NULL works");
//     }
}


