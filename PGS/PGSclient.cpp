//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: PGSclient
//
// Author: Eliot Muir
//
// Description:
//
// Native postgres driver - for information on the Postgres client library API
// See:
//    http://zetcode.com/db/postgresqlc/
//    https://www.postgresql.org/docs/9.1/libpq-connect.html
//---------------------------------------------------------------------------

#include "PGSclient.h"
#include "PGSconnectionPool.h"

#include <COL/COLdll.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
#include <COL/COLostream.h>
#include <COL/COLmap.h>
#include <COL/COLthreadPool.h>
COL_LOG_MODULE;

#include <FIL/FILutils.h>

#ifdef __linux__
   #define PQLIB_NAME "libpq.so"     // LINUX
#elif _WIN32 
   #define PQLIB_NAME "libpq.dll"    // WINDOWS
#else
   #define PQLIB_NAME "libpq.dylib"  // MAC OS X
#endif

typedef PQresult*    (*PQexec)        (PQconn* Conn, const char* Command);
typedef char*        (*PQdb)          (const PQconn* conn);
typedef int          (*PQlibVersion)  ();
typedef int          (*PQresultStatus)(const PQconn* Conn);
typedef void         (*PQclear)       (PQresult* res);
typedef void         (*PQfreemem)     (void* ptr);

// Retrieving Query Result Information
typedef int                (*PQnTuples) (const PQresult* pResult); // Returns the number of rows in query result
typedef int                (*PQnFields) (const PQresult* pResult); // Returns the number of columns in query result
typedef const char*        (*PQgetValue)(const PQresult* pResult, int Row, int Column); // Get value at row/column
typedef const char*        (*PQfName)   (const PQresult* pResult, int Column); // Returns the name of the column for the given column number
typedef const unsigned int (*PQfType)   (const PQresult* pResult, int Column); // Returns the data type of the column

// Prepared Statements
typedef PQresult* (*PQprepare)     (PQconn* conn, const char* statementName, const char* query, int nParams, const int* paramTypes);
typedef PQresult* (*PQexecPrepared)(PQconn* conn, const char* statementName, int nParams, const char* paramValues, const int* paramLengths, const int* paramFormats, int resultFormat);

// Escaping Strings for Inclusion in SQL Commands
typedef char* (*PQescapeLiteral)(PQconn* conn, const char* str, size_t length); // escapes a string for use within an SQL command
typedef char* (*PQerrorMessage) (const PQconn* conn);

// COPY API
// TODO - define for PQputCopyData and PQputCopyEnd
typedef int         (*PQgetCopyData)(PQconn* conn, char** ppBuffer, int async);
typedef int         (*PQfformat)    (const PQresult* pResult, int Column);
typedef PQresult*   (*PQgetResult)  (PQconn* conn);
typedef int         (*PQputCopyData)(PQconn* conn, const char* pBuffer, int nbytes);
typedef int         (*PQputCopyEnd) (PQconn* conn, const char* errorMsg);
typedef const char* (*PQcmdStatus)  (const PQresult* pResult);

// These ones we need to use from PGSconnectionPool
PQconnect      pPQConnect       = NULL;
PQfinish       pPQFinish        = NULL;
PQerrorMessage pPQErrorMessage  = NULL;
PQstatus       pPQStatus        = NULL;

// Static means these functions are global to the application
static PQexec           pPQExec          = NULL;
static PQerrorMessage   pPQError         = NULL;
static PQdb             pPQDb            = NULL;
static PQlibVersion     pPQVersion       = NULL;
static PQresultStatus   pPQResultStatus  = NULL;
static PQnTuples        pPQNumberTuples  = NULL;
static PQnFields        pPQNumberFields  = NULL;
static PQgetValue       pPQGetValue      = NULL;
static PQescapeLiteral  pPQEscapeLiteral = NULL;
static PQfName          pPQFieldName     = NULL;
static PQfType          pPQFieldType     = NULL;
static PQprepare        pPQprepare       = NULL;
static PQexecPrepared   pPQexecPrepared  = NULL;
static PQclear          pPQclear         = NULL;
static PQfreemem        pPQfreemem       = NULL;

// For COPY API
// TODO - add the other two.
static PQgetCopyData    pPQgetCopyData   = NULL;
static PQfformat        pPQfformat       = NULL;
static PQgetResult      pPQgetResult     = NULL;
static PQputCopyData    pPQputCopyData   = NULL;
static PQputCopyEnd     pPQputCopyEnd    = NULL;
static PQcmdStatus      pPQcmdStatus     = NULL;

enum PQResultCodes {
   PGRES_EMPTY_QUERY    = 0,	// empty query string was executed 
	PGRES_COMMAND_OK     = 1,	// a query command that doesn't return anything was executed properly by the backend 
	PGRES_TUPLES_OK      = 2,	// a query command that returns tuples was executed properly by the backend, PGresult contains the result tuples 
	PGRES_COPY_OUT       = 3,	// Copy Out data transfer in progress 
	PGRES_COPY_IN        = 4,	// Copy In data transfer in progress 
	PGRES_BAD_RESPONSE   = 5,	// an unexpected response was received from the backend 
	PGRES_NONFATAL_ERROR = 6,	// notice or warning message 
	PGRES_FATAL_ERROR    = 7,	// query failed 
	PGRES_COPY_BOTH      = 8,	// Copy In/Out data transfer in progress 
	PGRES_SINGLE_TUPLE   = 9	// single tuple from larger resultset 
};
enum PQDataTypes {
   PQbool = 16,
   PQname = 19,
   PQint2 = 21,
   PQint4 = 23,
   PQint8 = 20,
   PQfloat8 = 701,
   PQstring = 1043,
   PQtext = 25,
   PQdate = 1082,
   PQtimestamp = 1114,
   PQtsvector = 3614
};

bool PGSload(const COLstring& LibraryPath){
   COL_FUNCTION(PGSload);
   COL_VAR(LibraryPath + PQLIB_NAME);
   // Static in this context means that this object is persistent
   // outside of the scope of this function.  TODO write a wiki on the many uses of static.
   static COLdll m_Dll;
   // TODO handle Windows and Linux platforms.
   COL_DBG("About to load postgres client library.");
   COLdllLoadResult Result = m_Dll.tryLoadLibrary(LibraryPath + PQLIB_NAME);
   COL_VAR(Result);
   if (COL_DLL_LOAD_OK != Result){
      COL_TRC("Was not able to load postgres client library.");
      //*pInfo = "Could not find postgres driver";
      return false;
   }
   COL_TRC("Loaded postgres library");
   pPQConnect       = (PQconnect)       m_Dll.getProcAddress("PQconnectdbParams");
   pPQExec          = (PQexec)          m_Dll.getProcAddress("PQexec");
   pPQError         = (PQerrorMessage)  m_Dll.getProcAddress("PQerrorMessage");
   pPQDb            = (PQdb)            m_Dll.getProcAddress("PQdb");
   pPQVersion       = (PQlibVersion)    m_Dll.getProcAddress("PQlibVersion");
   pPQResultStatus  = (PQresultStatus)  m_Dll.getProcAddress("PQresultStatus");
   pPQNumberTuples  = (PQnTuples)       m_Dll.getProcAddress("PQntuples");
   pPQNumberFields  = (PQnFields)       m_Dll.getProcAddress("PQnfields");
   pPQGetValue      = (PQgetValue)      m_Dll.getProcAddress("PQgetvalue");
   pPQEscapeLiteral = (PQescapeLiteral) m_Dll.getProcAddress("PQescapeLiteral");
   pPQErrorMessage  = (PQerrorMessage)  m_Dll.getProcAddress("PQerrorMessage");
   pPQFieldName     = (PQfName)         m_Dll.getProcAddress("PQfname");
   pPQFieldType     = (PQfType)         m_Dll.getProcAddress("PQftype");
   pPQFinish        = (PQfinish)        m_Dll.getProcAddress("PQfinish");
   pPQStatus        = (PQstatus)        m_Dll.getProcAddress("PQstatus");
   pPQprepare       = (PQprepare)       m_Dll.getProcAddress("PQprepare");
   pPQexecPrepared  = (PQexecPrepared)  m_Dll.getProcAddress("PQexecPrepared");
   pPQclear         = (PQclear)         m_Dll.getProcAddress("PQclear");
   pPQfreemem       = (PQfreemem)       m_Dll.getProcAddress("PQfreemem");
   pPQgetCopyData   = (PQgetCopyData)   m_Dll.getProcAddress("PQgetCopyData");
   pPQfformat       = (PQfformat)       m_Dll.getProcAddress("PQfformat");
   pPQgetResult     = (PQgetResult)     m_Dll.getProcAddress("PQgetResult");
   pPQputCopyData   = (PQputCopyData)   m_Dll.getProcAddress("PQputCopyData");
   pPQputCopyEnd    = (PQputCopyEnd)    m_Dll.getProcAddress("PQputCopyEnd");
   pPQcmdStatus     = (PQcmdStatus)     m_Dll.getProcAddress("PQcmdStatus");
 
   COL_DBG("Found all the functions.");
   int Version = (*pPQVersion)();
   COL_VAR(Version);
   return true;
}

PGSclient::PGSclient(PGSconnectionPool* pPool) : m_Connection(NULL), m_pPool(pPool), m_CopyResult(NULL){
   COL_METHOD(PGSclient::PGSclient);
   if (pPQConnect == NULL){
      throw COLerror("Fatal. Postgres client driver was not loaded!", 1);
   }
}

PGSclient::~PGSclient(){
   COL_METHOD(PGSclient::~PGSclient);
   if (isConnected()){
      COL_TRC("Disconnecting in destructor.");
      disconnect();
   }
}

static void checkError(PQconn* pConnection, PGSconnectionPool* pPool){
   const char* pErrorMessage = (*pPQErrorMessage)(pConnection);
   if (strcmp(pErrorMessage, "") != 0){
      // save error message before closing connection or it won't throw
      COLstring ErrorMessage(pErrorMessage);
      (*pPQFinish)(pConnection);
      pConnection = NULL;
      pPool->disconnect(NULL);  // Call to make sure we keep count of connections in the pool consistent
      throw COLerror(ErrorMessage, COLerror::PostCondition);
   }
}
// PGSclient::connect()
// Maps from a COLvar into the const char array needed by the Postgres Client API libpq.
void PGSclient::connect(const COLvar& ConnectInfo){
   COL_METHOD(PGSclient::connect);
   COL_VAR(ConnectInfo);
   m_Connection = m_pPool->connect(ConnectInfo);
   COL_VAR(m_Connection);
}

void PGSclient::disconnect(bool Force){
   COL_METHOD(PGSclient::disconnect);
   if (isConnected()) {
      m_pPool->disconnect(m_Connection, Force);
   }
   m_Connection = NULL;
}

bool PGSclient::isConnected() const{
   COL_METHOD(PGSclient::isConnected);
   if (m_Connection == NULL)              { COL_TRC(m_Connection); return false; }
   if ((*pPQStatus)(m_Connection) == 0)   { return true; } // Connection is good
   else                                   { return false; }
}

void PGSclient::execute(const COLstring& Sql){
   COL_METHOD(PGSclient::execute);
   if (!isConnected()) { throw COLerror("Could not execute query. No database connection exists.", 1); }
   COL_DBG("Sql=" << Sql);
   PQresult* Result = (*pPQExec)(m_Connection, Sql.c_str());
   COL_TRC("On connection = " << m_Connection << " Executed SQL: " << Result);
   (*pPQclear)(Result);
   const char* pErrorMessage = (*pPQErrorMessage)(m_Connection);
   if (strcmp(pErrorMessage, "") != 0){
      // save error message before closing connection or it won't throw
      COLstring ErrorMessage(pErrorMessage);
      (*pPQFinish)(m_Connection);
      m_Connection = NULL;
      m_pPool->disconnect(NULL);
      throw COLerror(ErrorMessage, COLerror::PostCondition);
   }
   COL_TRC("No errors");
}

void PGSclient::query(const COLstring& Sql, COLvar* pResult){
   COL_METHOD(PGSclient::query);
   if (!isConnected()) { throw COLerror("Could not execute query. No database connection exists.", 1); }
   COLvar& Result = *pResult;
   COL_VAR2(m_Connection, Sql);
   PQresult* RawResult = (*pPQExec)(m_Connection, Sql.c_str());
   const char* pErrorMessage = (*pPQErrorMessage)(m_Connection);
   if (strcmp(pErrorMessage, "") != 0){
      // save error message before closing connection or it won't throw
      COLstring ErrorMessage(pErrorMessage);
      (*pPQclear)(RawResult);
      (*pPQFinish)(m_Connection);
      m_Connection = NULL;
      m_pPool->disconnect(NULL);
      throw COLerror(ErrorMessage, COLerror::PostCondition);
   }
   int NumberOfRows     = (*pPQNumberTuples)(RawResult);
   int NumberOfColumns  = (*pPQNumberFields)(RawResult);
   COL_VAR2(NumberOfColumns, NumberOfRows);
   unsigned int DataType;
   for (int j=0; j < NumberOfColumns; j++){  // TODO - try and reverse order of columns and rows
      DataType = (*pPQFieldType)(RawResult, j);       // Get data type of column
      COL_TRC("column: " << j << " Datatype = " << DataType);
      COLvar& Column = Result[(*pPQFieldName)(RawResult, j)];
      // Next line ensures we always get an array even if zero length and optimizes memory allocation/resizing for each COLvar
      Column.setArrayType();
      Column.array().resize(NumberOfRows);
      for (int i=0; i < NumberOfRows; i++){
         // COL_TRC("row: " << i);
         // Store data as correct type in COLvar.
         switch (DataType) {
            case PQbool:      { Column[i] = (*pPQGetValue)(RawResult, i,j)[0] == 't' ? true : false; break;}       // postgres returns f for false, t for true
            case PQint2:
            case PQint4:
            case PQint8:      { Column[i] = ::atoi((*pPQGetValue)(RawResult, i,j));                  break;}
            case PQfloat8:    { Column[i] = ::atof((*pPQGetValue)(RawResult, i,j));                  break;}
            case PQstring:
            case PQname:
            case PQtext:
            case PQtimestamp:
            case PQdate: 
            case PQtsvector:  { Column[i] = (*pPQGetValue)(RawResult, i,j);                          break;}
            default:          { throw COLerror("Unexpected Data Type.", 0);                          break;}
         }
         // COL_VAR(Column[i]);
      }
   }
   COL_TRC("Select result parsed into COLvar");
   if ( NumberOfRows == 0 ) {
      Result["_isNull"] = true;
   }
   COL_VAR(Result);
   (*pPQclear)(RawResult);
}

void PGSclient::insert(const COLvar& Data){
   COL_METHOD(PGSclient::insert);
   if (!isConnected()) { throw COLerror("Could not execute query. No database connection exists.", 1); }
   COL_DBG(Data);
   COL_TRC("Checking parameters after checking that we are connected.");
   if(!Data.exists("table") || !Data.exists("data")) {
      throw COLerror("Data COLvar is malformed! Check table, and data exist.", 0);
   }
   if      (0 == Data["table"].asString().size()) { throw COLerror("Table name missing!", 0);         }
   else if (0 == Data["data"].size())             { throw COLerror("No data to insert!", 0);          }
   // Start SQL query
   COLstring Sql = "INSERT INTO ";
   Sql.append(Data["table"]);
   Sql.append(" (");
   COL_TRC("Set up first part of SQL insert statement.");
   // Important to take const reference otherwise we can end up copying a lot of data.
   const COLvar::COLvarMap& ColumnMap = Data["data"].map();
   COLvar::COLvarMap::const_iterator it = ColumnMap.begin(), E = ColumnMap.end();
   int CountOfRow = it->second.array().size();
   COL_VAR(CountOfRow);
   for (; it != E; ++it) {
      Sql += it->first + ",";
   }
   Sql.get_buffer()[Sql.size()-1] = ')';
   // Populate rows of data in query
   Sql.append(" VALUES ");
   for (int i =0; i < CountOfRow;i++){
      Sql += "(";
      it = ColumnMap.begin();
      for (; it != E; ++it) {
         const COLvar& Item = it->second[i];
         if (Item.isString()) {
            Sql.append(PGSescapeLiteral(Item));
         } else {
            Sql.append(Item);
         }
         Sql +=",";
      }
      Sql.get_buffer()[Sql.size()-1] = ')';
      Sql += ",";
   }
   Sql.get_buffer()[Sql.size()-1] = ';';
   COL_TRC("Insert query created.");
   COL_VAR(Sql.size());
   //COL_VAR(Sql);
   execute(Sql);
}

void PGSclient::parseColumns (const COLvar& Data, COLstring* pSql) {
   COL_METHOD(PGSclient::parseColumns);
   COLstring &Sql = *pSql;
   COLvar::COLvarVec Columns = Data["columns"].array();
   for (int i = 0; i < Columns.size(); i++) {
      if (Columns[i]["name"].isNone()) {
         throw COLerror("A column name is missing! Check column indices!", 0);
      }
      if (0 == Columns[i]["name"].asString().size()) {
         throw COLerror("Column names cannot be empty strings!", 0);
      }
      Sql.append(Columns[i]["name"].asString());
      if (i != Columns.size() - 1) {
         Sql.append(", ");
      } else {
         Sql.append(")");
      }
   }
}

void PGSclient::parseRows (const COLvar& Data, COLstring* pSql) {
   COL_METHOD(PGSclient::parseRows);
   COLstring& Sql = *pSql;
   COLvar::COLvarVec Rows = Data["data"].array();
   //Populate each row of data
   for (int i = 0; i < Rows.size(); i++) {
      for (int j = 0; j < Rows[i].size(); j++) {
         //Check for any null data
         if (Rows[i][j].isNone()) {
            throw COLerror("Data is missing an entry! Check data indices!", 0);
         }
         //If the data is a string wrap it in quotes otherwise just add it to the query
         if (Rows[i][j].isString()) {
            Sql.append(PGSescapeLiteral(Rows[i][j]));
         } else {
            Sql.append(Rows[i][j]);
         }
         //Append a comma if there is another item otherwise append a closing bracket
         if (j != Rows[i].size() - 1) {
            Sql.append(", ");
         } else {
            Sql.append(")");
         }
      }
      //Start a new row of data if there is another row otherwise append ; to finish query
      if (i != Rows.size() - 1) {
         Sql.append(", (");
      } else {
         Sql.append(";");
      }
   }
}

void PGSclient::copy(const COLstring& Sql, COLvar *pResult){
   COL_METHOD(PGSclient::copy);
   if (!isConnected()) { throw COLerror("Could not execute query. No database connection exists.", 1); }
   COL_DBG("Sql=" << Sql);
   m_CopyResult = (*pPQExec)(m_Connection, Sql.c_str());
   COL_TRC("On connection = " << m_Connection << " Executed SQL: " << m_CopyResult);
   const char* pErrorMessage = (*pPQErrorMessage)(m_Connection);
   const char* pRowsCopied = (*pPQcmdStatus)(m_CopyResult); // Gets number of rows copied
   COLvar &Result = *pResult;
   Result["copied"] = (*pPQcmdStatus)(m_CopyResult);
   COL_TRC("Copied " << pRowsCopied << " rows");
   if (strcmp(pErrorMessage, "") != 0){
      // save error message before closing connection or it won't throw
      COLstring ErrorMessage(pErrorMessage);
      (*pPQclear)(m_CopyResult);
      (*pPQFinish)(m_Connection);
      m_Connection = NULL;
      m_pPool->disconnect(NULL);
      throw COLerror(ErrorMessage, COLerror::PostCondition);
   }
   (*pPQclear)(m_CopyResult);
   COL_TRC("No errors");
}

bool PGSclient::getCopyData(COLstring* pResult){
   COL_METHOD(PGSclient::getCopyData);
   char* pData;
   int Length = (*pPQgetCopyData)(m_Connection, &pData, 0);
   COL_VAR(Length);
   COL_DUMP("Data", pData, Length);
   if (Length != -1){
      const char* pErrorMessage = (*pPQErrorMessage)(m_Connection);
      if (strcmp(pErrorMessage, "") != 0){
         // save error message before closing connection or it won't throw
         COLstring ErrorMessage(pErrorMessage);
         (*pPQFinish)(m_Connection);
         m_Connection = NULL;
         m_pPool->disconnect(NULL);
         (*pPQfreemem)(pData);      
         throw COLerror(ErrorMessage, COLerror::PostCondition);
      }
      pResult->clear();
      pResult->append(pData, Length);
      (*pPQfreemem)(pData);      
      return true;
   } 

   m_CopyResult = (*pPQgetResult)(m_Connection);  // obtain the final status of the COPY command and view it with PQresultStatus
   COL_VAR(m_CopyResult);
   int Status   = (*pPQResultStatus)(m_CopyResult); 
   COL_VAR(Status);
   if (Status == PGRES_COMMAND_OK) {
      (*pPQfreemem)(pData);                       
      (*pPQclear)(m_CopyResult);
      COL_TRC("Cleared result");
   } else if (Status == PGRES_FATAL_ERROR){
      checkError(m_Connection, m_pPool);
      (*pPQfreemem)(pData);
      (*pPQclear)(m_CopyResult);
      COL_TRC("Error thrown, cleared result");
   }
   return false;
}

void PGSclient::putCopyData(const COLstring& Data){
   COL_METHOD(PGSclient::putCopyData);
   int ReturnCode = (*pPQputCopyData)(m_Connection, Data.c_str(), Data.size()); // 1 if data was queued, 0 if buffers full (async operation only), -1 if error occured
   COL_VAR(ReturnCode);
   if (ReturnCode != 1){
      checkError(m_Connection, m_pPool);
   }
}

void PGSclient::endCopyData(){
   COL_METHOD(PGSclient::endCopyData);
   int CopyEndStatus = (*pPQputCopyEnd)(m_Connection, NULL); // 1 if data was queued, 0 if buffers full (async operation only), -1 if error occured
   COL_VAR(CopyEndStatus);
   m_CopyResult = (*pPQgetResult)(m_Connection);  
   int Status   = (*pPQResultStatus)(m_CopyResult); 
   COL_VAR(Status);
   (*pPQclear)(m_CopyResult);
   if (CopyEndStatus == 1){
      if (Status != PGRES_COMMAND_OK) {
         checkError(m_Connection, m_pPool);
      } else { COL_TRC("Copy ended with no error"); }
   } else {
      checkError(m_Connection, m_pPool);
   }
}

COLstring PGSescapeLiteral(const COLstring& Input) {
   //COL_FUNCTION(PGSescapeLiteral);  // too noisy
   //COL_VAR(Input); // too noisy
   PQconn Conn;
   COLstring Result;
   // rare but would happen if someone calls this function before the PGSclient has been loaded
   // or if loading it failed...
   COLASSERT(pPQEscapeLiteral != NULL); 
   char* pQuotedInput = (*pPQEscapeLiteral)(&Conn, Input.c_str(), Input.size());
   Result.append(pQuotedInput);
   (*pPQfreemem)((void*)pQuotedInput);
   return Result;
}

// TODO
// * Get the second result set, confirm it is okay    - DONE
// * Implement the put equivalent APIs                - DONE
// * Write tests for these APIS                       - DONE
// * Stream to file - FILfile                   
// * Run under valgrind to make sure we don't leak memory.        - IPR
// * Figure out how invoke GZIP simply - i.e. get all the data and compress in one go.
//   - Can repeatedly call getCopyData until it runs false to get the data row by row
//   - feed that into a string and pass it into GZIPcompress(string, compressed data string)
//   - then FILwrite to a file using the compressed data string
// * Figure out how implement GZIP more smartly with streaming of data.
// * Make PGS library make use of path resolution functions for ../postgres.log and so on for better tracing - DONE
// * See if we can detect running on the wrong type of filesystem issue.
//   - Modify MONexample, PGSexample, monitor applications   - DONE?
