#ifndef __PGS_CLIENT_H__
#define __PGS_CLIENT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: PGSclient
//
// Author: Eliot Muir
//
// Description:
//
// Native postgres database library oriented to be COLvar friendly.
//---------------------------------------------------------------------------

#include <COL/COLdll.h>
#include <COL/COLmap.h>

class COLstring;
class COLvar;
class PGSconnectionPool;


bool PGSload(const COLstring& LibraryPath);

typedef void* PQconn;
typedef void* PQresult;

// TODO rename to PGSclient
class PGSclient{
public:
   PGSclient(PGSconnectionPool* pPool);
   ~PGSclient();
   
   // Establish a TCP/IP database connection to a database
   // This call is blocking.  Parameter's expected are:
   //    database - Name of the database to connect to
   //    user     - Name of the user ID to connect as
   //    password - Password of the user to connect as
   // Function will throw a COLerror if there is an error.
   void connect(const COLvar& ConnectInfo); 
   
   // Disconnect from the database
   // Releases the connection back to the connection pool UNLESS Force is set to ture
   void disconnect(bool Force = false);

   // Are we connected?
   bool isConnected() const;

   // Executes a SQL statement - could be a INSERT, CREATE USER etc.
   // will throw if the statement fails.
   void execute(const COLstring& Sql);

   // Perform a SQL query - e.g. SELECT
   // will throw if the statement fails
   void query(const COLstring& Sql , COLvar* pResult);

   // This is new  interface for inserting rows of data into Postgres which
   // should be more convenient.
   // COLvar Data;
   // Data["table"] = "testlog";
   // Data["data"]["id"][0] =1;
   // Data["data"]["id"][1] =2;
   // Data["data"]["id"][2] =3;
   // Data["data"]["body"][0] = "Hello world";
   // Data["data"]["body"][1] = "This is the data";
   // Data["data"]["body"][2] = "This is more data";
   // Connection.insert(Data);
   void insert(const COLvar& Data);

   void copy(const COLstring& CopyCommand, COLvar* pResult);
   bool getCopyData(COLstring* pResult);

   void putCopyData(const COLstring& Data);
   void endCopyData();
private:
   // Our connection handle
   PQconn* m_Connection;
   PQresult* m_CopyResult;

   PGSconnectionPool* m_pPool;
   PGSclient(const PGSclient& Orig); 
   PGSclient& operator=(const PGSclient& Orig);

   void parseColumns (const COLvar& Data, COLstring* pSql);
   void parseRows (const COLvar& Data, COLstring* pSql);
};

// PGSescapeLiteral()
// Quote a value for use in a SQL statement
// this will help prevent SQL injection and perform character 
// escaping where needed. Returns COLstring for ease of use,
// and in-line usage when composing SQL statements.  This function
// puts quotes around the the input.
COLstring PGSescapeLiteral(const COLstring& Input);

typedef PQconn*      (*PQconnect)(const char** pKeywords, const char** pValues, int expand_dbname);
typedef void         (*PQfinish) (PQconn* conn);
typedef char*        (*PQerrorMessage)(const PQconn* Conn);
typedef unsigned int (*PQstatus)      (const PQconn* Conn);


extern PQconnect      pPQConnect;
extern PQfinish       pPQFinish;
extern PQerrorMessage pPQErrorMessage;
extern PQstatus       pPQStatus;

#endif // end of defensive include
