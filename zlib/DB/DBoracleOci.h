#ifndef __DB_ORACLE_OCI_H__
#define __DB_ORACLE_OCI_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBOracleOci
//
// Description:
//
// This class handles calls to Oracle through the native OCI API. This class 
// links the API with the common interface provided by the DBdatabase parent 
// class.
//
// Author: Julian Muir 
// Date:   May 13, 2021
//
//---------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
// JULIAN the original class was copied from DBdatabaseOracleOci.h for reference
//-----------------------------------------------------------------------------------

#include "DBdatabase.h"
#include "DBdriverOracleOci.h"

#include <COL/COLclosure.h>
#include <COL/COLvar.h>

// JULIAN copied from DBmySql.h
class DBresultSetPtr;
class DBresultSetRow;
class DBoracleOciDll;        // JULIAN is this needed??? equiv in MySQL not used yet??

enum DBOracleOciLoadResult  // JULIAN is this needed??? 
{
    DB_OCI_LOAD_OK = 1,
    DB_OCI_LOAD_INVALID_LIBRARY = -1, // The file was not a valid DLL or shared object - i.e. a text file
    DB_OCI_LOAD_NOT_EXIST = -2,       // The file did not exist
    DB_OCI_LOAD_NO_PERMISSIONS = -3,  //  We did not have permissions to load it
    DB_OCI_LOAD_UNKNOWN_ERROR = -4
};

class DBoracleOci { // : public DBdatabase
public:
    DBoracleOci();
    virtual ~DBoracleOci();

    // Set the dynamic client library this instance of DBoracleOci will use
    bool setDriver(const COLstring& LibraryPath);

    // Initializes any required parameters/settings and connects the 
    // DB object to the DBMS.
    bool connect(const COLvar& ConnectionInfo);

    // void connect( const char* pDataSourceName, // JULIAN OLD FORMAT
    //                       const char* pUserName,
    //                       const char* pPassword );

    // Deallocates all open database handles and disconnects the 
    // DB object from the DBMS.
    void disconnect(); // JULIAN TODO no function in cpp yet
   
    // Returns if the database object is connected. This can be used to test
    // the connection before issuing further statements to the object.
    // Note that databases may go down, which may result in databases disconnecting on
    // exceptions, and not on just calls to disconnect().
    bool isConnected();

    // Submits a SQL command string to the DBMS. All of the above executeSql...
    // methods call this method.
    // NOTE: Input strings are assumed to be unescaped as this function will
    // escape any special SQL characters for you.
  
    // JULIAN MYSQL FORMAT CAN WE CHANGE ====TODO==== looks like nothing in cpp yet
    // DBresultSetPtr executeSqlString( const COLstring& SqlString, 
    //                                         COLuint32 ResultSetStart = 0, 
    //                                         COLuint32 ResultSetSize = 0, 
    //                                         COLarray<DBresultSetPtr> *pAllResultSets = NULL);

    // DBresultSetPtr executeSqlString( const COLstring& SqlString,
    //                                         COLuint32 ResultSetStart = 0,
    //                                         COLuint32 ResultSetSize = 0,
    //                                         COLarray<DBresultSetPtr> *pAllResultSets = NULL) const;

    // JULIAN MYSQL FORMAT CAN WE CHANGE ====TODO==== try to do it this way...
    bool executeSqlString(const COLstring& SqlString, COLvar* pResultSet, COLvar* pResultInfo);


    // JULIAN WE MIGHT NEED THIS ????????????????????? ========TODO?=========
    // see https://docs.oracle.com/cd/B19306_01/appdev.102/b14250/oci05bnd.htm
    // Since OCI uses parameter binding, this function needs to be overriden.
    void createSqlCommandWithoutBinding( const DBsqlCommand& SqlCommand, COLostream& OutputSql ) const;

    COLstring getErrorMessages(OCIError* pErrorHandle);

private:
    DBdriverOracleOci       m_Driver;

    // Oracle handles are critical - this page has handle information 
    // https://docs.oracle.com/en/database/oracle/oracle-database/12.2/lnoci/oci-programming-basics.html#GUID-5F13B2EC-E29C-4710-8AD7-E97E6C28085E

    OCIError*               m_pErrorHandle          = nullptr; // could create each time but this is more convenient - set once in ::connect()
    OCIEnv*                 m_pEnvironmentHandle    = nullptr; // OCI environment handle set in ::setDriver (LoadLibrary)
    OCISvcCtx*              m_pServiceContextHandle = nullptr; // defines attributes for the operational context for OCI calls to a server
    // it contains 3 SUB-HANDLES: (1) Server Handle (OCIServer), (2) User Session Handle (OCISession) and (3) Transaction Handle (OCITrans)
    // these can be retrieved using calls to oci_attr_get() - so they do not need to be stored here (but we can add later for convenience it needed)

    COLvar                  m_CachedConnectionInfo;            
    COLclosure1<COLstring>* m_pLogger;                         //JULIAN TODO copied from DBmySql.h

    // JULIAN not exactly sure what these are yet or what they will be used for
    // Copied from DBdatabaseOracleOciPrivate in DBdatabaseOracleOci.cpp
    // move to object not driver = driver is minimal...
    // OCIServer*              m_pServerHandle         = nullptr; // JULIAN - probably just use calls to oci_attr_get()
    // OCISession*             m_pSessionHandle        = nullptr; // for these 3 sub-handles of m_pServiceContextHandle
    // OCITrans*               m_pTransactionHandle    = nullptr; 



    // JULIAN TODO what is not allowed???????
    DBoracleOci( DBoracleOci& Orig );            // not allowed
    DBoracleOci& operator=( DBoracleOci& Orig ); // not allowed
};

#endif // end of defensive include

