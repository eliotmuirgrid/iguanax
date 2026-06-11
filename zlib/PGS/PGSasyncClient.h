#ifndef __PGS_ASYNC_CLIENT_H__
#define __PGS_ASYNC_CLIENT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: PGSasyncClient
//
// Author: Eliot Muir
//
// Description:
//
// This is a asynchronous client designed to work with SCKloop.  
//---------------------------------------------------------------------------

class SCKloop;
#include "PGSconnectionPool.h"
#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLvar.h>

class PGSasyncClient{
public:
   PGSasyncClient(SCKloop* pLoop, PGSconnectionPool* pConnectionPool);
   ~PGSasyncClient();
   
   // Establish a TCP/IP database connection to a database
   // This call just gives data for the connection which is done 
   // on another thread - so if the credentials are wrong the error
   // will be delayed.  Parameter's expected are:
   //    database - Name of the database to connect to
   //    user     - Name of the user ID to connect as
   //    password - Password of the user to connect as
   void connect(const COLvar& ConnectInfo); 
   
   // Executes a SQL statement - could be a INSERT, CREATE USER etc.
   // The callback returns a COLvar with the expected fields:
   //     success - boolean - true or false
   //     error   - The text of an error if and only if success == false
   void execute(const COLstring& Sql, COLauto<COLclosure1< COLauto<COLvar> > > pCallback);
 
   // Perform a SQL query - e.g. SELECT
   // The callback returns a COLvar with the expected fields
   //     success - boolean - true or false
   //     error   - The text of an error if and only if success == false
   //     data    - The data as returned by PSGclient::query
   void query(const COLstring& Sql , COLauto<COLclosure1< COLauto<COLvar> > > pResult);

   // This is a potential interface for inserting rows of data into Postgres.
   // COLvar Data;
   // Data["table"] = "testlog";
   // Data["columns"][0]["name"] = "id";
   // Data["columns"][0]["key"] = true
   // Data["columns"][1]["name"] = "body";
   // Data["data"][0][0] = 1
   // Data["data"][0][1] = "Hello world";
   // Data["data"][1][0] = 2
   // Data["data"][1][1] = "This is data";
   // Connection.insert(Data, COLnewClosure1(&Callback));
   // The callback returns a COLvar with the expected fields
   //     success - boolean - true or false
   //     error   - The text of an error if and only if success == false
   void insert(COLauto<COLvar> pData, COLauto<COLclosure1< COLauto<COLvar> > > pCallback);

   void copy(const COLstring& Sql , COLauto<COLclosure1< COLauto<COLvar> > > pResult);
private:
   PGSasyncClient(const PGSasyncClient& Orig); 
   PGSasyncClient& operator=(const PGSasyncClient& Orig);

   COLvar m_ConnectInfo;
   SCKloop* m_pLoop;
   PGSconnectionPool* m_pConnectionPool;
};

#endif // end of defensive include
