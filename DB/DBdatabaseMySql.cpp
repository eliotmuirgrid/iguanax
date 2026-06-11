//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseMySql
//
// Description:
//
// MySql database implementation class. 
//
// Author: YunqiZhang
// Date:   November 27, 2003 
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBdatabaseMySql.h"
#include "DBdatabaseMySqlPrivate.h"
#include "DBdatabaseType.h"
#include "DBdriverFind.h"
#include "DBresultSetPtr.h"
#include "DBsqlCreateTable.h"
#include "DBsqlCreateTableColumn.h"
#include "DBresultSetRow.h"
#include "DBvariant.h"
#include "DBescape.h"
#include "DBdll.h"

#include <COL/COLdateTime.h>
#include <COL/COLlocker.h>
#include <COL/COLarray.h>
#include <COL/COLdll.h>
// #include <COL/COLsimpleBuffer.h>
#include <COL/COLsinkNull.h>

#include <FIL/FILutils.h>
#include <LINK/LINKconstants.h>

#include <COL/COLlog.h>
#include <stdio.h>

COL_LOG_MODULE;

#ifdef _WIN32
   #include <REG/REGregistryWindows.h>
#endif

#include <stdlib.h>  // for atoi


#ifndef _WIN32
   COLarray<DBmySqlDllInfo> DBfindSupportedMySqlDlls(bool)
   {
      COLarray<DBmySqlDllInfo> None;
      return None;
   }
#else // if defined(WIN32)
   COLarray<DBmySqlDllInfo> DBfindSupportedMySqlDlls(bool IgnoreCache)
   {
      COLlocker Lock(DBmySqlDll::dllSection());

      static const char* const KnownMySqlRegistryKeys[] = // In preferred order.
      {
         //
         // TODO_NEW_MYSQL:  Add new registry keys here.  Make sure that they refer to
         // TODO_NEW_MYSQL:  the install path (Location) and that the DLL is installed as
         // TODO_NEW_MYSQL:  "[Location]\[lib or bin]\libmySQL.dll".
         //
         // Some MySQL installers (e.g., 5.5.8) put 64-bit registry keys in
         // the 32-bit namespace, so we search both.  The loader will check
         // the libraries, ensuring that only 64-bit ones are listed.
         //

#undef MySQL
#ifdef CHM_64
#define MySQL(Ver) \
   "SOFTWARE\\MySQL AB\\MySQL Server " ## #Ver, \
   "SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server " ## #Ver
#else
#define MySQL(Ver) \
   "SOFTWARE\\MySQL AB\\MySQL Server " ## #Ver
#endif
         MySQL(8.0),
         MySQL(5.7),
         MySQL(5.6),
         MySQL(5.5),
         MySQL(5.1),
         MySQL(5.0),
         MySQL(4.1),
         MySQL(4.0),
         NULL
#undef MySQL
      };

      static COLarray<DBmySqlDllInfo> CachedResults;
      static bool CacheIsValid = false;

      if( IgnoreCache || !CacheIsValid )
      {
         CachedResults.clear();
         
         COL_TRC("Looping through known Registry Keys...");
         for(const char* const* pKey = KnownMySqlRegistryKeys; *pKey; ++pKey)
         {
            try
            {
               REGregistry Registry(HKEY_LOCAL_MACHINE);

               if (Registry.subKeyExists(*pKey))
               {
                  COL_TRC("Sub-Key Exists!");
                  Registry.openKey(*pKey, false);

                  const COLstring Prefix = Registry.readString("Location", "");
                  COLstring Path;
                  COLdll Dll;
                  COL_VAR(Prefix);
                  if(!Prefix.is_null() && (Dll.tryLoadLibrary(Path = Prefix + "lib\\libmySQL.dll") == COL_DLL_LOAD_OK ||
                                           Dll.tryLoadLibrary(Path = Prefix + "bin\\libmySQL.dll") == COL_DLL_LOAD_OK))
                  {
                     DBmySqlDll MySql(&Dll);
                     DBmySqlDllInfo& Info = CachedResults.push_back();
                     Info.Path    = Path;
                     Info.Version = MySql.versionString();
                     COL_VAR2(Info.Path, Info.Version); 
                  }
               }            
            }
            catch(const COLerror&)
            {
               // The DLL is not supported, or something else bad happened.
               // We check to see if the key exists before we open it.
               COL_TRC("Error");
            }
         }

         CacheIsValid = true;
      }

      return CachedResults;
   }

   static bool loadMySqlDllViaRegistry(COLdll& Dll, int &ErrorCode)
   {
      COLarray<DBmySqlDllInfo> SupportedDlls = DBfindSupportedMySqlDlls(true);
      int CurrentErrorCode;

      for(COLindex DllIndex=0, MaxIndex=SupportedDlls.size(); DllIndex < MaxIndex; ++DllIndex)
      {
         if( (CurrentErrorCode = Dll.tryLoadLibrary(SupportedDlls[DllIndex].Path.c_str()))  == COL_DLL_LOAD_OK )
         {
            return true;
         }
         if((CurrentErrorCode == COL_DLL_LOAD_NO_PERMISSIONS) || (CurrentErrorCode == COL_DLL_LOAD_INVALID_LIBRARY))
         {
            ErrorCode = CurrentErrorCode;
         }
      }

      return false;
   }
#endif // defined(WIN32)


static int tryLoadDefaultMySqlDll(COLdll& Dll, COLostream& LogStream){
   COL_FUNCTION(tryLoadDefaultMySqlDll);
   int ErrorCode = COL_DLL_LOAD_OK;

   #if defined(WIN32)
      loadMySqlDllViaRegistry(Dll, ErrorCode) || (ErrorCode = Dll.tryLoadLibrary("libmySQL.dll"));
   #elif defined(CHM_MAC)
      COLarray<COLstring> List;
      COLstring DllFileName="libmysqlclient.dylib";
      DBsearchPaths("DYLD_LIBRARY_PATH", &List, &LogStream);
      List.push_back("/usr/local/lib");
      bool Success = DBdllFindInDirList(List, "libmysqlclient*dylib", &DllFileName, &LogStream);
      ((ErrorCode = Dll.tryLoadLibrary(DllFileName)) == COL_DLL_LOAD_OK);
   #else // Linux
      COLarray<COLstring> List;
      COLstring DllFileName="libmysqlclient.so";
      DBsearchPaths("LD_LIBRARY_PATH", &List, &LogStream);
      List.push_back("/usr/lib64/mysql/");
      bool Success = DBdllFindInDirList(List, "libmysqlclient.so*", &DllFileName, &LogStream);
      ((ErrorCode = Dll.tryLoadLibrary(DllFileName)) == COL_DLL_LOAD_OK);
   #endif
   
   COL_VAR(ErrorCode);
   return ErrorCode;
}


bool DBdatabaseMySql::isLibrarySupported(const COLstring& LibraryPath, int* pDllLoadErrorCode)
{
   COL_FUNCTION(DBdatabaseMySql::isLibrarySupported);
   try {
      COL_VAR(LibraryPath);
      COLdll* pDll = new COLdll();
      COLostream NullStream(new COLsinkNull(), true);
      int ErrorCode = LibraryPath.is_null() ? tryLoadDefaultMySqlDll(*pDll, NullStream) : pDll->tryLoadLibrary(LibraryPath);
      if( pDllLoadErrorCode ) {
         COL_TRC("ERROR trying to load library.");
         *pDllLoadErrorCode = ErrorCode;
      }

      if( ErrorCode == COL_DLL_LOAD_OK ){
         COL_TRC("LOAD_OK");
         COL_TRC("Creating new DLL Object");
         DBmySqlDll MySql(pDll);  // Throws if it's not a valid MySQL library.
         COL_TRC("Success!");
         return true;
      }
   }
   catch(const COLerror&) {
      // Don't care.
      COL_TRC("Caught an error");
   }

   return false;
}


static DBmySqlDll* pLoadedMySqlDll;  // Leaked on purpose (speed/simplicity).

bool DBdatabaseMySql::initialize(COLostream& LogStream, const char* LibraryPath){
   COL_FUNCTION(DBdatabaseMySql::initialize);
   if (!isInitialized()){
      COLlocker Lock(DBmySqlDll::dllSection());
      COL_TRC("Trying to load MySql compliant library");
      LogStream << COL_T("Trying to load MySql compliant library");
      COLdll* pDll = new COLdll();
      int ErrorCode = COL_DLL_LOAD_OK;
      COLstring RawError;
      COL_TRC("Loading from library path: " << LibraryPath);
      ErrorCode = pDll->tryLoadLibrary(LibraryPath, true, &RawError);
      COL_VAR2(pDll->libraryName(), pDll->loaded());
      if(!pDll->loaded()){
         COL_TRC("NO (mysql compliant support not enabled)");
         LogStream << COL_T("NO (mysql compliant support not enabled)") << newline;
         if(ErrorCode == COL_DLL_LOAD_NO_PERMISSIONS) {
            COL_TRC("Not enough permissions to load MySQL compliant library.");
            LogStream << COL_T("Not enough permissions to load MySQL compliant library.") << newline;
            LogStream << COL_T("For assistance, check the documentation ") << LINKdatabaseSettings << " " << newline;
            LogStream << COL_T("Raw error: " ) << newline << RawError << newline;
         } else if(ErrorCode == COL_DLL_LOAD_INVALID_LIBRARY) {
            COL_TRC("Not enough permissions to load MySQL.");
            LogStream << COL_T("The MySQL compliant library that was found is an invalid library.") << newline;
            LogStream << COL_T("For assistance, check the documentation ") << LINKdatabaseSettings << " " << newline;
            LogStream << COL_T("Raw error: " ) << newline << RawError << newline;
         }
      } else {
         LogStream << pDll->libraryName() << ' ';
         try {
            pLoadedMySqlDll = new DBmySqlDll(pDll);
            COL_TRC("OK (mysql enabled)");
            LogStream << 'v' << pLoadedMySqlDll->versionString() << COL_T(" OK (mysql compliant library enabled)") << newline;
         } catch(const COLerror& Error) {
            COL_TRC("Load Error");
            LogStream << COL_T("load error: ") << Error.description() << COL_T("  For assistance, read "
               "http://www.interfaceware.com/manual/mysql_dll_load_error.html") << newline;
         }
      } 
   } else {
      COL_TRC("Already loaded MySQL compliant library");
      LogStream
         << COL_T("Already loaded MySQL compliant library: ") << pLoadedMySqlDll->libraryPath()
         << COL_T(" v") << pLoadedMySqlDll->versionString() << COL_T(" OK (mysql compliant library enabled)") << newline;
   }
   return isInitialized();
}


bool DBdatabaseMySql::readyToReinitialize()
{
   // Hot-swapping was abandoned.  See #10153.
   //
   return (pLoadedMySqlDll == NULL);
}

bool DBdatabaseMySql::reinitialize(COLostream& LogStream, const char* LibraryPath)
{
   // Hot-swapping was abandoned.  See #10153.
   //
   return initialize(LogStream, LibraryPath);
}


void DBdatabaseMySql::initializeForThread()
{
   if (pLoadedMySqlDll)
   {
      COLlocker Lock(DBmySqlDll::dllSection());
      if (pLoadedMySqlDll->mysql_thread_init() != 0)
      {
         COL_ERROR_STREAM("Unable to initialize thread local storage for MySQL compliant library.", COLerror::PreCondition);
      }
   }
}

void DBdatabaseMySql::uninitializeForThread()
{
   if (pLoadedMySqlDll)
   {
      COLlocker Lock(DBmySqlDll::dllSection());
      pLoadedMySqlDll->mysql_thread_end();
   }   
}


bool DBdatabaseMySql::isInitialized()
{
   return (pLoadedMySqlDll != NULL);
}

const COLstring& DBdatabaseMySql::loadedApiLibraryPath()
{
   COLPRECONDITION(pLoadedMySqlDll != NULL);
   return pLoadedMySqlDll->libraryPath();
}

const COLstring& DBdatabaseMySql::loadedApiVersion()
{
   COLPRECONDITION(pLoadedMySqlDll != NULL);
   return pLoadedMySqlDll->versionString();
}


inline DBmySqlDll& getLoadedMySqlDll()
{
   COLPRECONDITION(pLoadedMySqlDll != NULL);
   return *pLoadedMySqlDll;
}

class DBdatabaseMySqlPrivate
{
public:
   explicit DBdatabaseMySqlPrivate(DBdatabaseMySql* pOwner)
      : MySqlDll(getLoadedMySqlDll())
      , pMySqlDatabase(NULL)
      , UseUnicode(false)
      , m_pOwner(pOwner)
   { }

   void disconnect();

   DBresultSetPtr fetchMySqlResultSet( MYSQL_RES* pMySqlResultSet,
                                       COLuint32 ResultSetStart = 0,
                                       COLuint32 ResultSetSize = 0 );
   
   // The main purpose of this method is to convert MySQL datetime types
   // into the COLdateTime format and adds the resulting value into the
   // specified result set row. Also converts the "ColumnValue" into the
   // appropriate DBvariant type based on the "BaseType" specified.
   void setResultSetColumnValue( DBresultSetRow& ResultSetRow,
                                 COLuint32 ColumnIndex,
                                 const COLstring& ColumnValue, 
                                 DBdataType BaseType ) const;

   // Appends the specified error message with a MySQL error message and 
   // throws it all as a COLerror object.
   void throwMySqlErrorWithMessage( const char* ErrorMessage );

   COLstring getMySqlErrorMessage( const char* ErrorMessage );

   DBmySqlDll& MySqlDll;
   MYSQL* pMySqlDatabase;
   bool UseUnicode;
   DBdatabaseMySql* m_pOwner;
};

void DBdatabaseMySqlPrivate::disconnect()
{
   if( pMySqlDatabase != NULL )
   {
      MySqlDll.mysql_close( pMySqlDatabase );
      pMySqlDatabase = NULL;
   }
}


DBresultSetPtr DBdatabaseMySqlPrivate::fetchMySqlResultSet
( 
   MYSQL_RES* pMySqlResultSet,
   COLuint32 ResultSetStart,
   COLuint32 ResultSetSize
)
{
   if ( pMySqlResultSet == NULL )
   {
      const char* ErrorMessage = "MySQL result set retrieval failed.";
      throwMySqlErrorWithMessage( ErrorMessage );
   }
 
   DBresultSetPtr pResultSet = new DBresultSet();

   // TODO question - does mysql_num_rows ever return -1??? could be fatal!
   COLuint32 CountOfResultSetColumn = MySqlDll.mysql_num_fields( pMySqlResultSet ); 
   pResultSet->resizeColumnVector( CountOfResultSetColumn);

   COLuint64 CountOfMySqlRow = MySqlDll.mysql_num_rows(pMySqlResultSet );
   pResultSet->setCountOfDatabaseRow( CountOfMySqlRow); 
   
   MYSQL_FIELD* pResultSetColumnInformation = MySqlDll.mysql_fetch_fields( pMySqlResultSet );
   // Build the column name vector for the result set.
   for ( COLuint32 ColumnIndex = 0; ColumnIndex < CountOfResultSetColumn; ColumnIndex++ )
   { 
      COLstring FieldName = MySqlDll.fieldName(pResultSetColumnInformation, ColumnIndex);
      if(FieldName.is_null()) {  // Non-aliased expressions are not always named.
         FieldName = "Column_" + COLuint32ToString(ColumnIndex+1);
      }
      pResultSet->setColumn( ColumnIndex, FieldName,
                             MySqlDll.fieldType(pResultSetColumnInformation, ColumnIndex,
                                                m_pOwner->supportBinaryType()) );
   }

   COLuint32 CountOfResultSetRow;
  if ( ResultSetStart >= CountOfMySqlRow )
   {
      CountOfResultSetRow = 0;
   }
   else if ( ( ( ResultSetStart + ResultSetSize ) < CountOfMySqlRow ) && ResultSetSize != 0 )
   {
      CountOfResultSetRow = ResultSetSize;
   }
   else
   {
      COLuint64 TotalCountOfResultSetRow = CountOfMySqlRow - ResultSetStart;
      if ( TotalCountOfResultSetRow > COL_UINT_32_MAX )
      {
         CountOfResultSetRow = COL_UINT_32_MAX;
      }
      else
      {
         CountOfResultSetRow = (COLuint32)TotalCountOfResultSetRow;
      } 
   }
   pResultSet->resizeRowVector( CountOfResultSetRow );
   MySqlDll.mysql_data_seek( pMySqlResultSet, ResultSetStart );

   // Build a vector of rows, where each row contains a vector of column values.
   for ( COLuint32 RowIndex = 0; RowIndex < CountOfResultSetRow; RowIndex++ )
   {
      MYSQL_ROW MySqlResultSetRow = MySqlDll.mysql_fetch_row( pMySqlResultSet );
      unsigned long* aValueLengths = MySqlDll.mysql_fetch_lengths( pMySqlResultSet );
      for ( COLuint32 ColumnIndex = 0; ColumnIndex < CountOfResultSetColumn; ColumnIndex++ )
      {
         const char* pValue = MySqlResultSetRow[ColumnIndex];
         if(NULL == pValue) continue;  // Value is NULL

         DBresultSetRow& Row = pResultSet->row(RowIndex);
         DBdataType Type = pResultSet->columnType(ColumnIndex);
         if(DB_BINARY == Type) {
            unsigned long Size = aValueLengths[ColumnIndex];
            // Eliot - I replaced the use of COLconstString in this line with the 4 lines which follow it.
            //setResultSetColumnValue(Row, ColumnIndex, COLconstString(pValue, Size), Type);
            COLstring Buf; // Vismay COLsimpleBuffer Buf --> COLstring Buf.. hopefully this is ok
            DBvariant Value(Buf); // copies the empty buffer; mainly just to tell the DBvariant that it's a binary type.
            Value.binary().write(pValue, Size);  // We still copy the data once. Oh well.  For blobs one really needs a different strategy.
            Row.setColumnValue(ColumnIndex, Value);
         } else {
            // We do this for backwards compatibility.  Strings may technically
            // contain NUL characters, but some code may not handle that well.
            setResultSetColumnValue(Row, ColumnIndex, COLstring(pValue), Type);
         }
      }
   }

   // TODO - this isn't really that safe - classic problem of what happens if we get
   // an exception that results in pMySqlResultSet not being freed up.  Solution is to
   // to create a helper class which has a destructor which guarantees that this result set is
   // cleaned up - probably good idea to involve Henry in thinking through a solution to this one...
   MySqlDll.mysql_free_result(pMySqlResultSet);
   return pResultSet;
}

void DBdatabaseMySqlPrivate::throwMySqlErrorWithMessage( const char* ErrorMessage)
{
   COLstring ErrorString = getMySqlErrorMessage(ErrorMessage);

   unsigned int ErrorCode = ( pMySqlDatabase!=NULL ) ? MySqlDll.mysql_errno(pMySqlDatabase) : 0;
   throw COLerror(ErrorString, __LINE__, __FILE__, ErrorCode);
}

//Can either be MariaDB or regular MySql. 
DBdatabaseMySql::DBdatabaseMySql(DBdatabaseType Type)
   : pMember(new DBdatabaseMySqlPrivate(this))
   , MySqlDll(pMember->MySqlDll)
   , m_BinaryOkay(false)
   , m_Type(Type)
{
   COL_METHOD(DBdatabaseMySql);
}

DBdatabaseMySql::~DBdatabaseMySql()
{
   COL_METHOD(~DBdatabaseMySql);
   try
   {
      disconnect();
      delete pMember;
   }
   catch(const COLerror& Error)
   {
      DB2_LOG(Error)
   }
}

void DBdatabaseMySql::connect
( 
   const char* pDataSourceName,
   const char* pUserName,
   const char* pPassword 
)
{
   COL_METHOD(connect);
   COL_VAR3(pDataSourceName, pUserName, pPassword);

   if (isConnected()
        && cachedDataSourceName() == pDataSourceName
        && cachedUserName() == pUserName
        && cachedPassword() == pPassword )
   {
      // already connected and with the same parameters - do nothing
      COL_TRC("already connected");
      return;
   };
   // reconnect if not connected, or if the connection parameters don't match previous ones

   disconnect();
   COLstring DataSource, Host;
   ((COLstring)pDataSourceName).split( DataSource, Host, "@" );
   
   unsigned int Port = 0;

   if( Host.find(":") != npos )
   {
      COLstring RealHost, PortString;
      Host.split(RealHost, PortString, ":");
      Host = RealHost;
      Port = atoi(PortString.c_str());
      COL_VAR2(Host, Port);
   }

   {
      //note that mysql_init is not thread safe so we have to lock before calling it
      COLlocker InitLock(DBmySqlDll::dllSection());
      if ( !(pMember->pMySqlDatabase = MySqlDll.mysql_init(NULL)) )
      {
         // in the unlikely case that this fails, we may not have a valid MYSQL handle to work with,
         // which is what throwMySqlErrorWithMessage() uses to fetch additional error information
         // --Dmitri
         const char* ErrorMessage = "Initialization of MySQL database object failed.";
         COL_ERR(ErrorMessage);
         // pMember->throwMySqlErrorWithMessage( ErrorMessage ); // not gonna work
         COL_ERROR_STREAM( ErrorMessage, COLerror::SystemError );
      }
   }

   try {
      if ( !MySqlDll.mysql_real_connect( pMember->pMySqlDatabase, Host.c_str(), pUserName, pPassword, DataSource.c_str(), Port, NULL, 0 ) )
      {
         const char* ErrorMessage = "Attempted connection to MySQL database failed.";
         COL_ERR(ErrorMessage);
         pMember->throwMySqlErrorWithMessage( ErrorMessage );
      }

      if(pMember->UseUnicode)
      {
         executeSqlString("set names 'utf8'");
      }

      setCachedDataSourceName( pDataSourceName );
      setCachedUserName( pUserName );
      setCachedPassword( pPassword );
   } catch(...) {
      try { disconnect(); } catch(...) { /* ignore */ }
      throw;
   }
}

void DBdatabaseMySql::disconnect()
{
   COL_METHOD(disconnect);
   pMember->disconnect();
   DBdatabase::endTransaction();
}

void DBdatabaseMySql::streamCreateTableNotNull
(
   COLostream& Stream, 
   const DBsqlCreateTable& SqlCreateTableCommand, 
   COLuint32 ColumnIndex
) const
{
   if ( SqlCreateTableCommand.column( ColumnIndex ).requiredFlag() == true || 
        SqlCreateTableCommand.column( ColumnIndex ).primaryKeyFlag() == true)
   {
      Stream << COL_T(" NOT NULL ");
   }
}

void DBdatabaseMySql::streamCreateTableForeignKeyPrefix
(
   COLostream& Stream, 
   const DBsqlCreateTable& SqlCreateTableCommand, 
   COLuint32 ColumnIndex
) const
{
   // The local column must be explicitly indexed before linking with a foreign key. 
   Stream << COL_T(",INDEX (") << SqlCreateTableCommand.column( ColumnIndex ).name() << ')';
}

void DBdatabaseMySql::streamCreateTableSuffix
( 
   COLostream& Stream, 
   const DBsqlCreateTable& SqlCreateTableCommand 
) const
{
   Stream << COL_T(" ENGINE = InnoDB");
   if(pMember->UseUnicode)
   {
      Stream << COL_T(" DEFAULT CHARSET='utf8'");
   }
}

DBresultSetPtr DBdatabaseMySql::executeSqlString
( 
   const COLstring& SqlString,
   COLuint32 ResultSetStart,
   COLuint32 ResultSetSize,
   COLarray<DBresultSetPtr> *pAllResultSets
) const
{
   COL_METHOD(executeSqlString);
   COL_VAR3(ResultSetStart, ResultSetSize, SqlString);
   if (!pMember->pMySqlDatabase){ throw COLerror("No connection found for MySql database. Exiting. "); }
   if ( MySqlDll.mysql_query( pMember->pMySqlDatabase, SqlString.c_str() ) )
   {
      COL_ERR(COL_T("executeSqlString failed: ") << SqlString);
      COLstring ErrorString;
      COLostream ErrorStream( ErrorString );
      ErrorStream << COL_T("Execution of the following SQL command failed:") << newline
                  << SqlString;
      pMember->throwMySqlErrorWithMessage( ErrorString.c_str() );
   }

   MYSQL_RES* pMySqlResultSet = MySqlDll.mysql_store_result( pMember->pMySqlDatabase );
   if ( pMySqlResultSet == NULL )
   {
      // Distinguish between a NULL result set and an error.
      // A non-empty error message indicates this is an error according to:
      // http://dev.mysql.com/doc/refman/5.0/en/mysql-store-result.html
      const char* MySqlError = MySqlDll.mysql_error(pMember->pMySqlDatabase);
      if (MySqlError && *MySqlError != 0)
      {
         COL_ERR(COL_T("executeSqlString mysql_store_result failed: ") << SqlString);
         COLstring ErrorString;
         COLostream ErrorStream(ErrorString);
         ErrorStream << COL_T("Retrieving the result of the following SQL command failed:") << newline
                     << SqlString;
         pMember->throwMySqlErrorWithMessage(ErrorString.c_str());
      }
      return NULL;
   }
   else
   {
      return pMember->fetchMySqlResultSet( pMySqlResultSet, ResultSetStart, ResultSetSize );
   }
}

DBresultSetPtr DBdatabaseMySql::fetchDatabaseTables() const
{
   if (!pMember->pMySqlDatabase){ throw COLerror("No connection found for MySql database. Exiting. "); }
   // NULL wildcard returns all tables in the specified database.
   MYSQL_RES* pMySqlResultSet = MySqlDll.mysql_list_tables( pMember->pMySqlDatabase, NULL );
  
   return pMember->fetchMySqlResultSet( pMySqlResultSet );
}

DBresultSetPtr DBdatabaseMySql::fetchNonSystemDatabaseTables() const
{
   // MySQL doesn't seem to return system tables, so we can just call
   // the regular function
   return fetchDatabaseTables();
}

DBresultSetPtr DBdatabaseMySql::fetchDatabaseTableColumns( const char* TableName ) const
{
   COLPRECONDITION( TableName != NULL );
   if (!pMember->pMySqlDatabase){ throw COLerror("No connection found for MySql database. Exiting. "); }

   MYSQL_RES* pMySqlResultSet = MySqlDll.mysql_list_fields( pMember->pMySqlDatabase, TableName, NULL );
   
   if ( pMySqlResultSet == NULL )
   {
      const char* ErrorMessage = "Initialization of database column information retrieval failed.";
      pMember->throwMySqlErrorWithMessage( ErrorMessage );
   }

   DBresultSetPtr pResultSet = new DBresultSet();

   const COLuint32 CountOfResultSetColumn = 6;  // TODO: why 6? majik number? Replace with named-constant - RESULT_SET_TOTAL_COLUMNS ?
   pResultSet->resizeColumnVector( CountOfResultSetColumn );

   COLuint32 ColumnIndex = 0;
   // Build the column name vector for the result set.
   pResultSet->setColumn( ColumnIndex++, "Column_Name",   DB_STRING );
   pResultSet->setColumn( ColumnIndex++, "Data_Type",     DB_INTEGER );
   pResultSet->setColumn( ColumnIndex++, "Default_Value", DB_DATA_TYPE_NOT_DEFINED );
   pResultSet->setColumn( ColumnIndex++, "Length",        DB_LARGE_INTEGER );
   pResultSet->setColumn( ColumnIndex++, "Required",      DB_BOOLEAN );
   pResultSet->setColumn( ColumnIndex++, "Primary_Key",   DB_BOOLEAN );

   COLuint32 CountOfDatabaseColumn = MySqlDll.mysql_num_fields( pMySqlResultSet );
   
   // Each database column will actually be a row in the result set.
   pResultSet->resizeRowVector( CountOfDatabaseColumn );

   MYSQL_FIELD* pDatabaseColumnInformation = MySqlDll.mysql_fetch_fields( pMySqlResultSet );

   for ( COLuint32 DatabaseColumnIndex = 0; DatabaseColumnIndex < CountOfDatabaseColumn; DatabaseColumnIndex++ )
   {  
      ColumnIndex = 0;

      // Column: "Column_Name"
      pResultSet->setValue( DatabaseColumnIndex, ColumnIndex++, COLstring(MySqlDll.fieldName(pDatabaseColumnInformation, DatabaseColumnIndex)) );
     
      // In ticket #6424 it was decided that in the case of fetching column
      // definitions, we will treat DECIMAL columns as STRING columns.
      DBdataType BaseType;

      try
      {
         BaseType = MySqlDll.fieldType(pDatabaseColumnInformation, DatabaseColumnIndex,
                                       supportBinaryType());
      }
      catch (const COLerror& Error)
      {
         BaseType = DB_DATA_TYPE_NOT_DEFINED;
      }
      
      pResultSet->setValue( DatabaseColumnIndex, ColumnIndex++, BaseType );
      
      // Column: "Default_Value"
      const char *pDefault = MySqlDll.fieldDef(pDatabaseColumnInformation, DatabaseColumnIndex);
      if(pDefault) {
         DBresultSetRow& Row = pResultSet->row(DatabaseColumnIndex);
         pMember->setResultSetColumnValue(Row, ColumnIndex, pDefault, BaseType);
      }
      ColumnIndex++;

      // Column: "Length"
      pResultSet->setValue( DatabaseColumnIndex, ColumnIndex++, (COLint64)MySqlDll.fieldLength(pDatabaseColumnInformation, DatabaseColumnIndex) );
      
      // Column: "Required"
      if ( MySqlDll.fieldFlags(pDatabaseColumnInformation, DatabaseColumnIndex) & NOT_NULL_FLAG )
      {
         pResultSet->setValue( DatabaseColumnIndex, ColumnIndex++, true );
      }
      else
      {
         pResultSet->setValue( DatabaseColumnIndex, ColumnIndex++, false );
      }

      // Column: "Primary_Key"
      if ( MySqlDll.fieldFlags(pDatabaseColumnInformation, DatabaseColumnIndex) & PRI_KEY_FLAG )
      {
         pResultSet->setValue( DatabaseColumnIndex, ColumnIndex++, true );
      }
      else
      {
         pResultSet->setValue( DatabaseColumnIndex, ColumnIndex++, false );
      }
   }
   
   MySqlDll.mysql_free_result(pMySqlResultSet);

   return pResultSet;
}

DBresultSetPtr DBdatabaseMySql::fetchDatabases()
{
   COLPRECONDITION(isConnected());
   MYSQL_RES* pMySqlResultSet = MySqlDll.mysql_list_dbs( pMember->pMySqlDatabase, NULL );
   if ( pMySqlResultSet == NULL )
   {
      pMember->throwMySqlErrorWithMessage( "Fetching MySQL compliant database names failed." );
   }
   return pMember->fetchMySqlResultSet( pMySqlResultSet );
}

bool  DBdatabaseMySql::isSystemTable( const char* TableName ) const
{
   return false;
}


void DBdatabaseMySqlPrivate::setResultSetColumnValue
( 
   DBresultSetRow& ResultSetRow,
   COLuint32 ColumnIndex, 
   const COLstring& ColumnValue, 
   DBdataType BaseType
) const
{
   if ( BaseType == DB_DATETIME )
   {
      const COLstring& DateTimeString = ColumnValue;

      if (DateTimeString[4] != '-')
      {

         // Retrieved MySQL timestamps are formatted as YYYYMMDDHHMMSS
         // The following constructs the appropriate COLdateTime object
         // for insertion into the result set.
         COLint32 Year = atoi( DateTimeString.substr( 0, 4 ).c_str() );
         COLint32 Month = atoi( DateTimeString.substr( 4, 2 ).c_str() );
         COLint32 Day = atoi( DateTimeString.substr( 6, 2 ).c_str() );
         COLint32 Hour = atoi( DateTimeString.substr( 8, 2 ).c_str() );
         COLint32 Minute = atoi( DateTimeString.substr( 10, 2 ).c_str() );
         COLint32 Second = atoi( DateTimeString.substr( 12, 2 ).c_str() );
         COLdateTime DateTimeValue( Year, Month, Day, Hour, Minute, Second );
         ResultSetRow.setColumnValue( ColumnIndex, DateTimeValue );

      }
      else
      {

         // Retrieved MySQL datetime objects are formatted as 
         // YYYY-MM-DD HH:MM:SS
         // The following constructs the appropriate COLdateTime object
         // for insertion into the result set.
         COLint32 Year = atoi( DateTimeString.substr( 0, 4 ).c_str() );
         COLint32 Month = atoi( DateTimeString.substr( 5, 2 ).c_str() );
         COLint32 Day = atoi( DateTimeString.substr( 8, 2 ).c_str() );
         COLint32 Hour = atoi( DateTimeString.substr( 11, 2 ).c_str() );
         COLint32 Minute = atoi( DateTimeString.substr( 14, 2 ).c_str() );
         COLint32 Second = atoi( DateTimeString.substr( 17, 2 ).c_str() );
         COLdateTime DateTimeValue( Year, Month, Day, Hour, Minute, Second );
         ResultSetRow.setColumnValue( ColumnIndex, DateTimeValue );
      }
   }
   else
   {
      // Utilizes the DBvariant method fromString() to convert the "ColumnValue"
      // into the appropriate DBvariant typed object.
      DBvariant DefaultValue = DBvariantFromString(ColumnValue, BaseType);
      ResultSetRow.setColumnValue( ColumnIndex, DefaultValue );
   }
}

COLstring DBdatabaseMySqlPrivate::getMySqlErrorMessage( const char* ErrorMessage )
{
   COLPRECONDITION ( ErrorMessage != NULL );
   COLstring ErrorString;
   COLostream ErrorStream( ErrorString );

   ErrorStream << ErrorMessage << newline;
   if( pMySqlDatabase != NULL )
   {
      ErrorStream
         << COL_T("MySQL Compliant Database Error Message:") << newline
         << MySqlDll.mysql_error( pMySqlDatabase );
   };
   return ErrorString;
}

COLostream& DBdatabaseMySql::addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const{
   return DBescapeMySql(Stream, UnescapedString);
}

void DBdatabaseMySql::setAutocommitFlag( bool Flag ) const 
{
   COL_METHOD(setAutocommitFlag);
   COL_VAR(Flag);

   if( MySqlDll.supportsTransactions() )
   {
      if (!pMember->pMySqlDatabase){ throw COLerror("No connection found for MySql database. Exiting. "); }

      if ( MySqlDll.mysql_autocommit(pMember->pMySqlDatabase, Flag) )
      {
         const char* ErrorMessage = "Failed to set autocommit.";
         pMember->throwMySqlErrorWithMessage( ErrorMessage );
      }
   }
}

void DBdatabaseMySql::commitTransaction()
{
   COL_METHOD(commitTransaction);
   
   try {
      if (MySqlDll.supportsTransactions()) {
         if (!pMember->pMySqlDatabase){ throw COLerror("No connection found for MySql database. Exiting. "); }

         if (MySqlDll.mysql_commit(pMember->pMySqlDatabase)) {
            const char* ErrorMessage = "Failed to commit transaction.";
            pMember->throwMySqlErrorWithMessage( ErrorMessage );
         }
      }

      DBdatabase::endTransaction();
   } catch (...) {
      DBdatabase::endTransaction();
      throw;
   }
}

void DBdatabaseMySql::rollbackTransaction()
{
   COL_METHOD(rollbackTransaction);
   
   try {
      if (MySqlDll.supportsTransactions()) {
         if (!pMember->pMySqlDatabase){ throw COLerror("No connection found for MySql database. Exiting. "); }

         if (MySqlDll.mysql_rollback(pMember->pMySqlDatabase)) {
            const char* ErrorMessage = "Failed to rollback transaction.";
            pMember->throwMySqlErrorWithMessage( ErrorMessage );
         }
      }

      DBdatabase::endTransaction();
   } catch (...) {
      DBdatabase::endTransaction();
      throw;
   }
}


bool DBdatabaseMySql::isConnected()
{
   //stat the connection. return if an error was hit
   if( pMember->pMySqlDatabase == NULL )
   {
      return false;
   }
   else
   {  //we have a handle. ping it. This automatically attempts to reconnect with current parameters
      //if the connection is down.
      return MySqlDll.mysql_ping(pMember->pMySqlDatabase) == 0;
   }
}

COLostream& DBdatabaseMySql::streamName( COLostream& Stream, const COLstring& Name, bool QuoteName ) const
{
   if ( QuoteName == true )
   {
      Stream << '`' << Name << '`';
   }
   else
   {
      Stream << Name;
   }
   return Stream;
}

static const COLstring DBcastStringType("CHAR");
const COLstring& DBdatabaseMySql::selectCastStringType() const
{
   return DBcastStringType;
}

static const COLstring DBtextType("LONGTEXT");
const COLstring& DBdatabaseMySql::dbTextType() const
{
   return DBtextType;
}

const COLstring& DBdatabaseMySql::apiLibraryPath() const
{
   return MySqlDll.libraryPath();
}

const COLstring& DBdatabaseMySql::apiVersion() const
{
   return MySqlDll.versionString();
}

bool DBdatabaseMySql::isValidLibraryPath(const COLstring& LibraryPath)
{
   return FILfileExists(LibraryPath);
}

void DBdatabaseMySql::setParameter(const COLstring& Name, const COLstring& Value)
{
   if(Name == DB_PARAM_USE_UNICODE)
   {
      if(Value == "yes")
      {
         pMember->UseUnicode = true;
         return;
      }
      else if(Value == "no")
      {
         pMember->UseUnicode = false;
         return;
      }
   }
   DBdatabase::setParameter(Name, Value);
}

bool DBdatabaseMySql::useUnicode() const
{
   return pMember->UseUnicode;
}
