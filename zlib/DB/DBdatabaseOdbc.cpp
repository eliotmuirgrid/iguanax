//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOdbc
//
// Description:
//
// This class handles calls to the ODBC C API. This class links the
// API with the common interface provided by the DBdatabase parent class.
//
// Author: Henry Tran
// Date:   February 23, 2004 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBdriverFind.h"
#include "DBdatabaseOdbc.h"
#include "DBresultSetPtr.h"
#include "DBsqlInsert.h"
#include "DBresultSetRow.h"
#include "DBsqlUpdate.h"
#include "DBvariant.h"
#include "DButf.h"
#include "DBdll.h"

#include <COL/COLauto.h>
#include <COL/COLdateTime.h>
#include <COL/COLdll.h>
#include <COL/COLarray.h>
#include <COL/COLmutex.h>
#include <COL/COLlocker.h>
#include <COL/COLlist.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifdef _WIN32
   #include <windows.h>
   #include <sqlext.h>
   // gcc build defines for sqlncli.h. TODO: remove when we switch back to cl
   #define __in_z
   #define __in
   #define __out
   #define __out_ecount_z_opt(size)
   // end gcc build defines for sqlncli.h. TODO: remove when we switch back to cl
   #include <sqlncli.h> //SQL Native Driver. 
#else // POSIX
   // Use sql headers in our codebase so we don't have to install odbc to build iguana. #1da950d
   // Used to cause issues in Linux. See https://fogbugzx.interfaceware.com/default.asp?23580
   // TODO: upgrade headers to v3.8
   #include "ODBCinclude/sqlext.h"
#endif

COLstring DBodbcErrorMessage(SQLSMALLINT HandleType, SQLHANDLE Handle, COLstring * pNativeErrorCode);

COLerror createErrorObject(SQLHANDLE Handle);

static bool DBodbcUseLock(){
   return COLlowerCase(getenv("IGUANA_USE_ODBC_LOCK")) != "no";
};

class DBodbcDynamic{
public:
   COLmutex CriticalSection;

   DBodbcDynamic(COLdll* pDll){
      COL_METHOD(DBodbcDynamic::DBodbcDynamic);
      m_pDll = pDll; // pDll is already loaded from the DBdatabaseOdbc::initialize logic, we just want to take ownership
      COL_VAR2(m_pDll->loaded(), m_pDll->libraryName());
      initializeFunctionPointers();
   }

   ~DBodbcDynamic() { 
      COL_METHOD(DBodbcDynamic::~DBodbcDynamic);
      delete m_pDll; 
   }

   SQLRETURN (COL_STD_CALL* sqlAllocHandle)(SQLSMALLINT HandleType, SQLHANDLE InputHandle, SQLHANDLE* pOutputHandle);

   SQLRETURN sqlAllocHandleLocked(SQLSMALLINT HandleType, SQLHANDLE InputHandle, SQLHANDLE* pOutputHandle){
      if(!DBodbcUseLock()) { return sqlAllocHandle(HandleType, InputHandle, pOutputHandle); }      
      COLlocker Locker(CriticalSection);
      return sqlAllocHandle(HandleType, InputHandle, pOutputHandle);
   }
   SQLRETURN (COL_STD_CALL* sqlBindParameter)(SQLHSTMT StatementHandle,SQLUSMALLINT ParameterIndex,SQLSMALLINT InputOutputType,SQLSMALLINT CdataType,SQLSMALLINT SqlDataType,SQLULEN SizeOfColumn,SQLSMALLINT DecimalDigits,SQLPOINTER  pParameterValue,SQLLEN SizeOfParameterValue, SQLLEN* pStrLen_or_pInd);
   SQLRETURN (COL_STD_CALL* sqlColumns)      (SQLHSTMT StatementHandle,SQLCHAR* pCatalogName,SQLSMALLINT SizeOfCatalogName,SQLCHAR* pSchemaName,SQLSMALLINT SizeOfSchemaName, SQLCHAR* pTableName, SQLSMALLINT SizeOfTableName, SQLCHAR* pColumnName, SQLSMALLINT SizeOfColumnName);
   SQLRETURN (COL_STD_CALL* sqlPrimaryKeys)  (SQLHSTMT StatementHandle,SQLCHAR* pCatalogName,SQLSMALLINT SizeOfCatalogName,SQLCHAR* pSchemaName,SQLSMALLINT SizeOfSchemaName,SQLCHAR* pTableName,SQLSMALLINT SizeOfTableName);
   SQLRETURN (COL_STD_CALL* sqlConnect)      (SQLHDBC ConnectionHandle,SQLCHAR* pDataSourceName,SQLSMALLINT SizeOfDataSourceName,SQLCHAR* pUserName,SQLSMALLINT SizeOfUserName,SQLCHAR* pPassword,SQLSMALLINT SizeOfPassword);
   SQLRETURN (COL_STD_CALL* sqlColAttribute) (SQLHSTMT StatementHandle,SQLUSMALLINT ColumnNumber,SQLUSMALLINT FieldIdentifier,SQLPOINTER CharacterAttributePtr,SQLSMALLINT BufferLength,SQLSMALLINT* StringLengthPtr,SQLLEN* NumericAttributePtr);
   SQLRETURN (COL_STD_CALL* sqlDescribeParam)(SQLHSTMT StatementHandle,SQLUSMALLINT ParameterIndex,SQLSMALLINT* pSqlDataType,SQLULEN* pSizeOfParameter,SQLSMALLINT* pDecimalDigits,SQLSMALLINT* pNullable);
   SQLRETURN (COL_STD_CALL* sqlDisconnect)   (SQLHDBC ConnectionHandle);
   SQLRETURN (COL_STD_CALL* sqlEndTran)      (SQLSMALLINT HandleType, SQLHANDLE Handle,
                         SQLSMALLINT CompletionType );

   SQLRETURN (COL_STD_CALL* sqlExecute)(SQLHSTMT StatementHandle);

   SQLRETURN (COL_STD_CALL* sqlExecDirect)(SQLHSTMT StatementHandle, SQLCHAR* pSql, SQLINTEGER SqlSize);

   SQLRETURN (COL_STD_CALL* sqlExecDirectW)(SQLHSTMT Handle, SQLWCHAR* pSql, SQLINTEGER SqlSize);

   SQLRETURN (COL_STD_CALL* sqlFetch) (SQLHSTMT Handle);

   SQLRETURN (COL_STD_CALL* sqlFreeHandle) (SQLSMALLINT HandleType, SQLHANDLE Handle);

   SQLRETURN sqlConnectLocked ( SQLHDBC ConnectionHandle,
      SQLCHAR* pDataSourceName,
      SQLSMALLINT SizeOfDataSourceName,
      SQLCHAR* pUserName,
      SQLSMALLINT SizeOfUserName,
      SQLCHAR* pPassword,
      SQLSMALLINT SizeOfPassword ){
      if(!DBodbcUseLock()) {
         return sqlConnect(ConnectionHandle, pDataSourceName, SizeOfDataSourceName, pUserName, SizeOfUserName, pPassword, SizeOfPassword);
      }      
      COLlocker Locker(CriticalSection);
      return sqlConnect(ConnectionHandle, pDataSourceName, SizeOfDataSourceName, pUserName, SizeOfUserName, pPassword, SizeOfPassword);
   };


   SQLRETURN sqlFreeHandleLocked(SQLSMALLINT HandleType, SQLHANDLE Handle) {
      if(!Handle)          { return SQL_SUCCESS; }
      if(!DBodbcUseLock()) { return sqlFreeHandle(HandleType, Handle); }
      COLlocker Locker(CriticalSection);
      return sqlFreeHandle(HandleType, Handle);
   }

   SQLRETURN (COL_STD_CALL* sqlGetData) ( SQLHSTMT StatementHandle,
                         SQLUSMALLINT ColumnIndex, 
                         SQLSMALLINT CdataType,
                         SQLPOINTER pDataBuffer, 
                         SQLLEN SizeOfDataBuffer,
                         SQLLEN* pStrLen_or_pInd);

   SQLRETURN (COL_STD_CALL* sqlGetDiagRec) (   SQLSMALLINT       HandleType,
                              SQLHANDLE         Handle,
                              SQLSMALLINT       RecNumber,
                              SQLCHAR           *SQLState,
                              SQLINTEGER        *NativeErrorPtr,
                              SQLCHAR           *MessageText,
                              SQLSMALLINT       BufferLength,
                              SQLSMALLINT       *TextLengthPtr);

   SQLRETURN (COL_STD_CALL* sqlGetTypeInfo)(SQLHSTMT Handle, SQLSMALLINT DataType);

   SQLRETURN (COL_STD_CALL* sqlNumResultCols) (SQLHSTMT Handle, SQLSMALLINT* pCountOfColumn);

   SQLRETURN (COL_STD_CALL* sqlPrepare)(SQLHSTMT StatementHandle, SQLCHAR* pSql, SQLINTEGER SqlSize);

   SQLRETURN (COL_STD_CALL* sqlSetConnectAttr) ( SQLHDBC ConnectionHandle,
                                SQLINTEGER ConnectionAttribute,
                                SQLPOINTER pAttributeValue,
                                SQLINTEGER StrLenAttributeValue );

   SQLRETURN (COL_STD_CALL* sqlSetEnvAttr) ( SQLHENV EnvironmentHandle,
                            SQLINTEGER EnvironmentAttribute, 
                            SQLPOINTER pAttributeValue,
                            SQLINTEGER StrLenAttributeValue );

   SQLRETURN (COL_STD_CALL* sqlTables) ( SQLHSTMT StatementHandle,
                        SQLCHAR* pCatalogName,
                        SQLSMALLINT SizeOfCatalogName,
                        SQLCHAR* pSchemaName,
                        SQLSMALLINT SizeOfSchemaName,
                        SQLCHAR* pTableName,
                        SQLSMALLINT SizeOfTableName,
                        SQLCHAR* pTableType,
                        SQLSMALLINT SizeOfTableType);

   SQLRETURN (COL_STD_CALL* sqlMoreResults) (SQLHSTMT StatementHandle);
   
   const COLstring& libraryName() const { return m_pDll->libraryName(); } 
private:
   COLdll* m_pDll;

   void initializeFunctionPointers() {
      assign(sqlAllocHandle,    "SQLAllocHandle");
      assign(sqlBindParameter,  "SQLBindParameter");
      assign(sqlColumns,        "SQLColumns");
      assign(sqlPrimaryKeys,    "SQLPrimaryKeys");
      assign(sqlConnect,        "SQLConnect");
      assign(sqlColAttribute,   "SQLColAttribute");
      assign(sqlDescribeParam,  "SQLDescribeParam");
      assign(sqlDisconnect,     "SQLDisconnect");
      assign(sqlEndTran,        "SQLEndTran");
      assign(sqlExecute,        "SQLExecute");
      assign(sqlExecDirect,     "SQLExecDirect");
      assign(sqlExecDirectW,    "SQLExecDirectW");
      assign(sqlGetDiagRec,     "SQLGetDiagRec");
      assign(sqlFetch,          "SQLFetch");
      assign(sqlFreeHandle,     "SQLFreeHandle");
      assign(sqlGetData,        "SQLGetData");
      assign(sqlGetDiagRec,     "SQLGetDiagRec");
      assign(sqlGetTypeInfo,    "SQLGetTypeInfo");
      assign(sqlNumResultCols,  "SQLNumResultCols");
      assign(sqlPrepare,        "SQLPrepare");
      assign(sqlSetConnectAttr, "SQLSetConnectAttr");
      assign(sqlSetEnvAttr,     "SQLSetEnvAttr");
      assign(sqlTables,         "SQLTables");
      assign(sqlMoreResults,    "SQLMoreResults");
   }

   //templates make this a whole lot easier
   template<class T>
   void assign(T& Function, const char* FunctionName) {
      Function = (T)m_pDll->getProcAddress(FunctionName);
   }
};

static DBodbcDynamic* pLoadedOdbcDll;  // Leaked on purpose (speed/simplicity).

class DBodbcConnection;

class DBodbcEnvironment {
public:
   DBodbcEnvironment();
   ~DBodbcEnvironment();

   SQLHDBC handle() const { return EnvironmentHandle; }
private:
   SQLHENV EnvironmentHandle;
};

class DBodbcStatement;
class DBodbcConnection
{
public:
   DBodbcConnection(DBodbcEnvironment* ipEnvironment);
   ~DBodbcConnection();

   SQLHDBC handle();

   void invalidate();

   void clearHandle();

   void registerStatement(DBodbcStatement*);
   void unregisterStatement(DBodbcStatement*);

private:
   COLlist<DBodbcStatement*> Statements;
   DBodbcEnvironment* pEnvironment;
   SQLHDBC ConnectionHandle;
};

class DBodbcStatement {
public:
   DBodbcStatement(DBodbcConnection* ipConnection);
   ~DBodbcStatement();

   SQLHSTMT handle() const { return StatementHandle; }
   void invalidate();

   void clearConnection(){pConn = 0;}
private:
   DBodbcConnection* pConn;
   SQLHSTMT StatementHandle;
};

// Allow default copy constructor.
struct DBodbcColInfo
{
   SQLSMALLINT SqlDataType;
   SQLSMALLINT DecimalDigits;
   SQLULEN     ParameterSize;
   SQLLEN      ValueLength;

   DBodbcColInfo()
      : SqlDataType(0)
      , DecimalDigits(0)
      , ParameterSize(0)
      , ValueLength(0)
   { }
};

class DBdatabaseOdbcPrivate
{
public:
   DBdatabaseOdbcPrivate(DBdatabaseOdbc* ipOwner)
      : pOwner(ipOwner), 
        TimeoutInSeconds(60 * 5), 
        UseUnicode(false),
        UseBinding(true)
   {}
   
   ~DBdatabaseOdbcPrivate() {}

   DBresultSetPtr fetchOdbcResultSet( SQLHSTMT StatementHandle,
                                      const COLstring& StatementString,
                                      SQLSMALLINT CountOfResultSetColumn,
                                      COLuint32 ResultSetStart,
                                      COLuint32 ResultSetSize );

   // TODO - Eliot - I'd really like to refactor and get rid of this static method.
   static void throwOdbcErrorWithMessage( const SQLSMALLINT& HandleType,
                                          const SQLHANDLE& Handle,
                                          const COLstring& ErrorMessage,
                                          DBdatabaseOdbc* pDatabase,
                                          const COLindex LineNumber);

   DBdataType variantType( SQLSMALLINT OdbcDataType,
                           const COLstring& StatementString,
                           const COLstring& ColumnName );

   void setResultSetColumnValue( const SQLHSTMT& StatementHandle,
                                 DBresultSetRow& ResultSetRow,
                                 const COLuint32& DatabaseColumnIndex,
                                 const COLuint32& ResultSetColumnIndex,
                                 DBdataType BaseType ) const;

   void executeSqlBind( const DBsqlInsert& SqlInsertCommand,
                        const COLstring& SqlInsertString,
                        SQLHSTMT          StatementHandle);

   void prepareForSqlBind( DBodbcStatement& Statement, const COLstring& SqlInsertString, const DBsqlInsert& SqlInsertCommand );

   void rebuildForExecute(COLstring& SqlString, const DBsqlInsert& SqlCommand,
                          void (*createSql)(const DBdatabase*, const DBsqlInsert&, COLostream&),
                          SQLHSTMT SqlHandle);

   // fetch database table names, using the specified table type
   // (see msdn documentation on TABLE_TYPE parameter for SQLTables)
   DBresultSetPtr fetchDatabaseTablesWithTableType(const COLstring& TableType);

   void setOption(SQLINTEGER Attribute, COLuint32 Value);
   void setTimeout(COLuint32 TimeoutInSeconds);

   bool SupportBinaryTypes;
   void setSupportBinaryTypes (bool BinarySupport = false) { SupportBinaryTypes = BinarySupport; }
   bool supportBinaryTypes () { return SupportBinaryTypes; }

   // Crappy C++ compilers need these to be part of the class.
   static bool isBinaryType(SQLINTEGER OdbcDataType);
   static SQLRETURN getColInfo(COLarray<DBodbcColInfo>& InfoOut,
              const DBsqlInsert&        SqlInsertCommand,
              const COLstring&          SqlInsertString,
              SQLHSTMT                  StatementHandle);

   inline void adjustEmptyString(COLstring& ColumnData) const
   {
      // wrapper to make adjustEmptyString() public
      pOwner->adjustEmptyString(ColumnData); 
   };

   COLauto<DBodbcEnvironment> pEnvironment;
   COLauto<DBodbcConnection> pConnection;

   DBdatabaseOdbc* pOwner;

   bool UseUnicode;
   bool UseBinding;
   COLuint32 TimeoutInSeconds;
};

// substitute the real values in the SQL query
COLstring DBdatabaseOdbcRecreateSqlQuery(const DBdatabaseOdbc& Database, const DBsqlInsert& SqlCommand, const COLstring& SqlString);
COLstring DBdatabaseOdbcRecreateSqlQuery(const DBdatabaseOdbc& Database, const DBsqlUpdate& SqlCommand, const COLstring& SqlString);

void DBodbcThrowErrorMessage (
   const COLstring ErrorMessage, 
   COLindex LineNumber, 
   const COLstring& BindingStatement, 
   const COLstring& ColumnName,
   SQLHSTMT StatementHandle,
   DBdatabaseOdbc* pDatabase
) {
   COLstring OutputErrorMessage;
   COLostream Output(OutputErrorMessage);
   Output << ErrorMessage << newline 
          << COL_T("Sql Statement: ") << BindingStatement << newline 
          << COL_T("Column: ") << ColumnName << newline;
   DBdatabaseOdbcPrivate::throwOdbcErrorWithMessage( SQL_HANDLE_STMT, StatementHandle, OutputErrorMessage.c_str(), pDatabase, LineNumber);
}

void DBodbcBindDataTime (
   DBvariant& Variant, 
   SQLSMALLINT SqlDataType, 
   COLarray< COLauto<SQL_TIMESTAMP_STRUCT> >& TimeStampArray,
   SQLHSTMT StatementHandle,
   SQLSMALLINT ParameterIndex,
   const COLstring& BindingStatement,
   const COLstring& ColumnName,
   DBdatabaseOdbc* pDatabase
) {
   if ( SqlDataType == SQL_UNKNOWN_TYPE ){ SqlDataType = SQL_TIMESTAMP; }

   const COLdateTime& DateTimeValue = Variant.dateTime();
   SQL_TIMESTAMP_STRUCT* pNewTimeStamp = TimeStampArray.push_back( new SQL_TIMESTAMP_STRUCT() ).get();
   pNewTimeStamp->year = DateTimeValue.year();
   pNewTimeStamp->month = DateTimeValue.month();
   pNewTimeStamp->day = DateTimeValue.day();
   pNewTimeStamp->hour = DateTimeValue.hour();
   pNewTimeStamp->minute = DateTimeValue.minute();
   pNewTimeStamp->second = DateTimeValue.second();
   pNewTimeStamp->fraction = 0; // No fractional seconds support for now

   if (SQL_ERROR == pLoadedOdbcDll->sqlBindParameter(StatementHandle,     
                                        ParameterIndex,
                                        SQL_PARAM_INPUT,
                                        SQL_C_TYPE_TIMESTAMP,
                                        SqlDataType,
                                        19, // The number of characters in yyyy-mm-dd hh:mm:ss
                                        0,
                                        (SQLPOINTER) pNewTimeStamp,
                                        0,
                                        NULL) ){
      COLstring Error;
      COLostream ErrorStream(Error);
      ErrorStream << COL_T("Error while attempting to bind to datetime: ") << Variant.dateTime();
      DBodbcThrowErrorMessage(Error, __LINE__, BindingStatement, ColumnName, StatementHandle, pDatabase);
   }
}

void DBodbcBindDouble
(
   DBvariant& Variant, 
   SQLULEN     ColumnSize,
   SQLSMALLINT DecimalDigits,
   SQLSMALLINT SqlDataType, 
   SQLHSTMT StatementHandle,
   SQLSMALLINT ParameterIndex,
   const COLstring& BindingStatement,
   const COLstring& ColumnName,
   DBdatabaseOdbc* pDatabase
) {
   if ( SqlDataType == SQL_UNKNOWN_TYPE ){ SqlDataType = SQL_DOUBLE; }

   if ( SQL_ERROR == pLoadedOdbcDll->sqlBindParameter(StatementHandle,     
                                        ParameterIndex,
                                        SQL_PARAM_INPUT,
                                        SQL_C_DOUBLE,
                                        SqlDataType,
                                        ColumnSize,
                                        DecimalDigits,
                                        (SQLPOINTER) &Variant.float64(),
                                        0,
                                        NULL) ){
      COLstring Error;
      COLostream ErrorStream(Error);
      ErrorStream << COL_T("Error while attempting to bind to double: ") << Variant.float64();
      DBodbcThrowErrorMessage(Error, __LINE__, BindingStatement, ColumnName, StatementHandle, pDatabase);
   }
}

void DBodbcBindFloat (
   DBvariant& Variant, 
   SQLULEN     ColumnSize,
   SQLSMALLINT DecimalDigits,
   SQLSMALLINT SqlDataType, 
   SQLHSTMT StatementHandle,
   SQLSMALLINT ParameterIndex,
   const COLstring& BindingStatement,
   const COLstring& ColumnName,
   DBdatabaseOdbc* pDatabase
){
   if ( SqlDataType == SQL_UNKNOWN_TYPE ){ SqlDataType = SQL_FLOAT; }

   if ( SQL_ERROR == pLoadedOdbcDll->sqlBindParameter(StatementHandle,     
                                        ParameterIndex,
                                        SQL_PARAM_INPUT,
                                        SQL_C_FLOAT,
                                        SqlDataType,
                                        ColumnSize,
                                        DecimalDigits,
                                        (SQLPOINTER) &Variant.float32(),
                                        0,
                                        NULL) ){
      COLstring Error;
      COLostream ErrorStream(Error);
      ErrorStream << COL_T("Error while attempting to bind to float: ") << Variant.float32();
      DBodbcThrowErrorMessage(Error, __LINE__, BindingStatement, ColumnName, StatementHandle, pDatabase);
   }
}
 
void DBodbcBindInteger (
   DBvariant& Variant, 
   SQLSMALLINT SqlDataType, 
   SQLHSTMT StatementHandle,
   SQLSMALLINT ParameterIndex,
   const COLstring& BindingStatement,
   const COLstring& ColumnName,
   DBdatabaseOdbc* pDatabase
) {
   if ( SqlDataType == SQL_UNKNOWN_TYPE ) { SqlDataType = SQL_INTEGER; }

   if ( SQL_ERROR == pLoadedOdbcDll->sqlBindParameter(StatementHandle,     
                                        ParameterIndex,
                                        SQL_PARAM_INPUT,
                                        SQL_C_LONG,
                                        SqlDataType,
                                        0,
                                        0,
                                        (SQLPOINTER) &Variant.integer32(),
                                        0,
                                        NULL) ){
      COLstring Error;
      COLostream ErrorStream(Error);
      ErrorStream << COL_T("Error while attempting to bind to integer: ") << Variant.integer32();
      DBodbcThrowErrorMessage(Error, __LINE__, BindingStatement, ColumnName, StatementHandle, pDatabase);
   }
}

void DBodbcBindLargeInteger (
   DBvariant& Variant, 
   SQLSMALLINT SqlDataType, 
   SQLHSTMT StatementHandle,
   SQLSMALLINT ParameterIndex,
   const COLstring& BindingStatement,
   const COLstring& ColumnName,
   DBdatabaseOdbc* pDatabase
) {
   if ( SqlDataType == SQL_UNKNOWN_TYPE ) { SqlDataType = SQL_BIGINT; }

   if ( SQL_ERROR == pLoadedOdbcDll->sqlBindParameter(StatementHandle,     
                                        ParameterIndex,
                                        SQL_PARAM_INPUT,
                                        SQL_C_SBIGINT,
                                        SqlDataType,
                                        0,
                                        0,
                                        (SQLPOINTER) &Variant.integer64(),
                                        0,
                                        NULL) ) {
      COLstring Error;
      COLostream ErrorStream(Error);
      ErrorStream << COL_T("Error while attempting to bind to large integer: ") << Variant.integer64();
      DBodbcThrowErrorMessage(Error, __LINE__, BindingStatement, ColumnName, StatementHandle, pDatabase);
   }
}

bool DBdatabaseOdbcPrivate::isBinaryType(SQLINTEGER OdbcDataType){
   switch(OdbcDataType) {
   // case -11: // Unique Identifier
   case SQL_BINARY:
   case SQL_VARBINARY:
   case SQL_LONGVARBINARY:
      return true;
   }
   return false;
}

void DBodbcBindString (
   DBvariant& Variant, 
   SQLSMALLINT SqlDataType, 
   SQLHSTMT StatementHandle,
   SQLSMALLINT ParameterIndex,
   const COLstring& BindingStatement,
   const COLstring& ColumnName,
   DBdatabaseOdbc* pDatabase,
   SQLULEN         SqlColumnSize,
   SQLLEN*         pValueLength
) {
   if ( SqlDataType == SQL_UNKNOWN_TYPE ) {
      if (pDatabase->supportsLongVarChar()) {
         SqlDataType = SQL_LONGVARCHAR;
      } else {
         SqlDataType = SQL_VARCHAR;
      }
   }
   bool IsBinary = DBdatabaseOdbcPrivate::isBinaryType(SqlDataType);
   const char* pValue = Variant.string().c_str();
   *pValueLength = Variant.string().size();
   if ( SQL_ERROR == pLoadedOdbcDll->sqlBindParameter(StatementHandle,     
                                        ParameterIndex,
                                        SQL_PARAM_INPUT,
                                        IsBinary ? SQL_C_BINARY : SQL_C_CHAR,
                                        SqlDataType,
                                        SqlColumnSize,
                                        0,
                                        (SQLPOINTER)pValue,
                                        *pValueLength,                   //This is because some dbs, like Access, don't like it when the 
                                        IsBinary ? pValueLength : NULL //StrLen_or_IndPtr is specified, and it's really only needed for binary data.
                                        ) ) {
      COLstring Error;
      COLostream ErrorStream(Error);
      ErrorStream << COL_T("Error while attempting to bind to string: ") << Variant.string();
      DBodbcThrowErrorMessage(Error, __LINE__, BindingStatement, ColumnName, StatementHandle, pDatabase);
   }
}

DBodbcEnvironment::DBodbcEnvironment() : EnvironmentHandle(0) {
   SQLRETURN Result = pLoadedOdbcDll->sqlAllocHandleLocked( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &EnvironmentHandle);
   if (Result == SQL_ERROR) {
      COL_ERR("SqlAllocHandle failed.");
      throw COLerror("Unable to allocation ODBC handle", COLerror::SystemError);
   }
   Result = pLoadedOdbcDll->sqlSetEnvAttr( EnvironmentHandle, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0 ); 
   if (Result == SQL_ERROR) {
      DBdatabaseOdbcPrivate::throwOdbcErrorWithMessage( SQL_HANDLE_ENV, EnvironmentHandle, "" ,0,__LINE__);
   }
}

SQLRETURN DBdatabaseOdbcPrivate::
getColInfo(COLarray<DBodbcColInfo>& InfoOut,
           const DBsqlInsert&        SqlInsertCommand,
           const COLstring&          SqlInsertString,
           SQLHSTMT                  StatementHandle)
{
   int r, nrow = (int)SqlInsertCommand.countOfRows();
   int c, ncol = (int)SqlInsertCommand.countOfColumn();
   int togo = ncol;  // How many columns are left to get info on.
   SQLSMALLINT Nullable, ParameterIndex = 0;
   SQLRETURN Result = SQL_SUCCESS;

   // We may have to consider every row.  We cannot assume that a NULL
   // in the first row implies that every value in its column is NULL.
   for(r = 0; r < nrow; r++) {
      for(c = 0; c < ncol; c++) {
         const DBvariant& ParameterValue = SqlInsertCommand.columnValue(c,r);
         if(ParameterValue.dataType() == DB_DATA_TYPE_NOT_DEFINED)
            continue;  // NULLs have no placeholders.
         ParameterIndex++;
         if(InfoOut[c].SqlDataType != 0)
            continue;  // We already know about this column.
         Result = pLoadedOdbcDll->sqlDescribeParam( StatementHandle, 
            ParameterIndex,
            &InfoOut[c].SqlDataType,
            &InfoOut[c].ParameterSize,
            &InfoOut[c].DecimalDigits,
            &Nullable );
         if(SQL_ERROR == Result || (InfoOut[c].SqlDataType != 0 && !--togo)) {
            return Result;  // Failed or we got them all.
         }
      }
   }
   return Result;
}

bool DBdatabaseOdbc::supportsDescribeParam() const { return true; }

void DBdatabaseOdbcPrivate::executeSqlBind (
   const DBsqlInsert& SqlInsertCommand, 
   const COLstring&  SqlInsertString,
   SQLHSTMT          StatementHandle
) {
   SQLRETURN Result;
   if(pConnection.get() == NULL){ throw COLerror("No connection found for ODBC database. Exiting. "); }
   COLPRECONDITION( pOwner->useBinding() );

   COLarray<DBodbcColInfo> ColumnInfo(SqlInsertCommand.countOfColumn());
   if(pOwner->supportsDescribeParam()) {
      Result = getColInfo(ColumnInfo, SqlInsertCommand, SqlInsertString, StatementHandle);
      if(SQL_ERROR == Result) {
         throwOdbcErrorWithMessage( SQL_HANDLE_STMT, StatementHandle, 
            DBdatabaseOdbcRecreateSqlQuery(*pOwner, SqlInsertCommand, SqlInsertString), pOwner ,__LINE__);
      }
   }
   int RC= SqlInsertCommand.countOfRows();
   for (int Row = 0; Row < RC; ++Row) {
      COLarray< COLauto<SQL_TIMESTAMP_STRUCT> > TimeStampVector; 
      COLindex CountOfNullSkipped=0;
      for (int ColumnIndex = 0; ColumnIndex < SqlInsertCommand.countOfColumn(); ColumnIndex++) {
         DBvariant& ParameterValue = (DBvariant&)SqlInsertCommand.columnValue( ColumnIndex, Row );
         COLstring ColumnName = SqlInsertCommand.columnName(ColumnIndex);

         if(ParameterValue.dataType() == DB_DATA_TYPE_NOT_DEFINED){
            CountOfNullSkipped++;
            continue;
            // bail out so that we don't try to access the NULL parameter 
            // which doesn't have a placeholder
         }

         SQLSMALLINT ParameterIndex = ColumnIndex + 1 - CountOfNullSkipped;
         // ODBC uses one based indexing for parameters
         // we don't have placeholders for NULL columns,
         // so the parameter index has to be offset

         SQLSMALLINT DecimalDigits = ColumnInfo[ColumnIndex].DecimalDigits;
         SQLSMALLINT SqlDataType   = ColumnInfo[ColumnIndex].SqlDataType;
         SQLULEN     ParameterSize = ColumnInfo[ColumnIndex].ParameterSize;

         switch(ParameterValue.dataType()){
         case DB_DATA_TYPE_NOT_DEFINED:
            // should've been handled above - this should never be reached
            COLMSGPRECONDITION(false, "null variant not expected here");
            break;
         case DB_STRING:        DBodbcBindString(ParameterValue, SqlDataType, StatementHandle, ParameterIndex, SqlInsertString, ColumnName, pOwner, ParameterSize, &ColumnInfo[ColumnIndex].ValueLength); break;
         case DB_INTEGER:       DBodbcBindInteger(ParameterValue, SqlDataType, StatementHandle, ParameterIndex, SqlInsertString, ColumnName, pOwner); break;
         case DB_DATETIME:      DBodbcBindDataTime(ParameterValue, SqlDataType, TimeStampVector, StatementHandle, ParameterIndex, SqlInsertString, ColumnName, pOwner); break;  
         case DB_DOUBLE:        DBodbcBindFloat(ParameterValue, ParameterSize, DecimalDigits, SqlDataType, StatementHandle, ParameterIndex, SqlInsertString, ColumnName, pOwner); break;
         case DB_LARGE_INTEGER: DBodbcBindLargeInteger(ParameterValue, SqlDataType, StatementHandle, ParameterIndex, SqlInsertString, ColumnName, pOwner); break;
         case DB_LARGE_DOUBLE:  DBodbcBindDouble(ParameterValue, ParameterSize, DecimalDigits, SqlDataType, StatementHandle, ParameterIndex, SqlInsertString, ColumnName, pOwner); break;
         default: 
            COL_ERROR_STREAM( "Unknown type for binding.", COLerror::PreCondition);
            break;
         };
      }

      Result = pLoadedOdbcDll->sqlExecute( StatementHandle );
      if (Result == SQL_ERROR){
         throwOdbcErrorWithMessage( SQL_HANDLE_STMT, StatementHandle, 
            DBdatabaseOdbcRecreateSqlQuery(*pOwner, SqlInsertCommand, SqlInsertString), pOwner ,__LINE__);
      }

      // Collect all returned tokens.  This is workaround for the bug described in #7067.
      while (Result != SQL_NO_DATA){
         Result = pLoadedOdbcDll->sqlMoreResults( StatementHandle );
         if (Result == SQL_ERROR) {
            throwOdbcErrorWithMessage( SQL_HANDLE_STMT, StatementHandle, 
               DBdatabaseOdbcRecreateSqlQuery(*pOwner, SqlInsertCommand, SqlInsertString), pOwner ,__LINE__);
         }
      }
   }
}

DBodbcEnvironment::~DBodbcEnvironment(){
   COL_METHOD(DBodbcEnvironment::~DBodbcEnvironment);
   if (EnvironmentHandle){
      if ( pLoadedOdbcDll->sqlFreeHandleLocked( SQL_HANDLE_ENV, EnvironmentHandle ) == SQL_ERROR ){
         COL_ERR("ODBC: CANNOT FREE ENVIRONMENT HANDLE");
      }
   }
}

DBodbcConnection::DBodbcConnection(DBodbcEnvironment* ipEnvironment) : ConnectionHandle(0), pEnvironment(ipEnvironment) {
   COL_METHOD(DBodbcConnection::DBodbcConnection);
   COLPRECONDITION(pEnvironment != 0);
   COLPRECONDITION(pEnvironment->handle() != 0)
}

SQLHDBC DBodbcConnection::handle(){
   COL_METHOD(DBodbcConnection::handle);
   if ( ConnectionHandle == 0 ) {
      SQLRETURN Result = pLoadedOdbcDll->sqlAllocHandleLocked(SQL_HANDLE_DBC, pEnvironment->handle(), &ConnectionHandle);
      if ( Result == SQL_ERROR ) {
         DBdatabaseOdbcPrivate::throwOdbcErrorWithMessage( SQL_HANDLE_ENV, pEnvironment->handle(), "" ,0,__LINE__);
      }
   }
   return ConnectionHandle;
}

void DBodbcConnection::clearHandle(){
   COL_METHOD(DBodbcConnection::clearHandle());
   COLPRECONDITION(ConnectionHandle != 0);
   if (pLoadedOdbcDll->sqlFreeHandleLocked(SQL_HANDLE_DBC, ConnectionHandle) == SQL_ERROR ){
      COL_TRC("ODBC: CANNOT FREE CONNECTION HANDLE");
   }
   ConnectionHandle = 0;
   pEnvironment = 0;
}

void DBodbcConnection::invalidate(){
   COL_METHOD(DBodbcConnection::invalidate);
   COL_TRC("Clearing Statements");
   while(Statements.size()){
      DBodbcStatement* pStatement = Statements[Statements.first()];
      pStatement->clearConnection(); //so invalidate doesn't call unregister
      pStatement->invalidate();
      Statements.remove(Statements.first());
   }
   if (ConnectionHandle){
      COL_TRC("Trying to disconnect connection.");
      //always disconnect
      if (pLoadedOdbcDll->sqlDisconnect( ConnectionHandle ) == SQL_ERROR){
         COL_TRC("ODBC: CANNOT DISCONNECT CONNECTION HANDLE");
         COLstring NativeErrorCode;
         COLstring ErrorMessage = DBodbcErrorMessage(SQL_HANDLE_DBC, ConnectionHandle, &NativeErrorCode);
         COL_TRC(ErrorMessage);
      }
      clearHandle();
   }
}

void DBodbcConnection::registerStatement(DBodbcStatement* pIn){
   // TODO - not so nice when there are lots of statements per connection, but this is rare
   COLlistPlace FoundPlace = 0;
   for (COLlistPlace Place = Statements.first(); !FoundPlace && Place; Place = Statements.next(Place)){
      if (Statements[Place] == pIn) FoundPlace = Place;
   }
   if (!FoundPlace){
      Statements.add(pIn);
   }
}

void DBodbcConnection::unregisterStatement(DBodbcStatement* pIn){
   for (COLlistPlace Place = Statements.first(); Place; Place = Statements.next(Place)){
      if (Statements[Place] == pIn) {
         Statements.remove(Place);
         break;   
      }
   }
}

DBodbcConnection::~DBodbcConnection(){
   COL_METHOD(DBodbcConnection::~DBodbcConnection);
   invalidate();
}

DBodbcStatement::DBodbcStatement(DBodbcConnection* pConnection) : StatementHandle(0), pConn(0) {
   COL_METHOD(DBodbcStatement-ctor);
   if(pConnection == NULL|| pConnection->handle() == NULL){ throw COLerror("No connection found for ODBC database. Exiting. "); }
   // TODO check the return codes to see what the error conditions could mean.
   SQLRETURN Result = pLoadedOdbcDll->sqlAllocHandleLocked( SQL_HANDLE_STMT, pConnection->handle(), &StatementHandle );
   COL_TRC("Allocated handle.");
   if (Result == SQL_ERROR) {
      COLerror Error = createErrorObject(pConnection->handle());
      throw Error;
   }
   pConn = pConnection;
   pConn->registerStatement(this);
}

void DBodbcStatement::invalidate(){
   COL_METHOD(DBodbcStatement::invalidate);
   if (StatementHandle){
      if (pLoadedOdbcDll->sqlFreeHandleLocked( SQL_HANDLE_STMT, StatementHandle ) == SQL_ERROR ){
         COL_TRC("ODBC: CANNOT FREE STATEMENT HANDLE");
      }
      StatementHandle = 0;
   }
   if (pConn){
      pConn->unregisterStatement(this);
      pConn = 0;
   }
}

DBodbcStatement::~DBodbcStatement(){
   COL_METHOD(DBodbcStatement::~DBodbcStatement);
   invalidate();
}

DBresultSetPtr DBdatabaseOdbcPrivate::fetchOdbcResultSet( 
   SQLHSTMT StatementHandle,
   const COLstring& StatementString,
   SQLSMALLINT CountOfResultSetColumn,
   COLuint32 ResultSetStart,
   COLuint32 ResultSetSize
) {
   COL_METHOD(DBdatabaseOdbcPrivate::fetchOdbcResultSet);
   COL_VAR3(StatementString, ResultSetStart, ResultSetSize);
   COL_TRC("Creating new result set");
   DBresultSetPtr pResultSet = new DBresultSet();

   COLPRECONDITION(CountOfResultSetColumn != -1)
   pResultSet->resizeColumnVector( CountOfResultSetColumn );   

   COLarray< DBdataType > TreVariantDataTypeVector;

   SQLRETURN Result;
   COLstring ColumnName;

   COL_TRC("Setting columns...");
   for (SQLSMALLINT i = 0; i < CountOfResultSetColumn; i++) {
      ColumnName.clear();
      SQLSMALLINT j = i + 1; // We have to do this because of an error which happens in a very specific usecase (unixODBC Postgres)
      SQLLEN OdbcDataType = 0;
      SQLLEN OdbcDataTypeScale = 0;
      {  //get column name
         // Oracle via unixODBC (and possibly others) will not give you
         // the correct length of a column alias (or name, perhaps).
         // Best just give it a big buffer; if it gets truncated, oh well.
         char Buf[512] = {0};
         Result = pLoadedOdbcDll->sqlColAttribute(StatementHandle,j, SQL_DESC_NAME, Buf, sizeof Buf, NULL, NULL);
         if ( Result == SQL_ERROR ){
            throwOdbcErrorWithMessage( SQL_HANDLE_STMT, StatementHandle, "" ,pOwner,__LINE__);
         }
         ColumnName = Buf;
         if(ColumnName.size() <= 0){
            ColumnName = "Column_" + COLintToString(j);
         }
      }
      {
         //get column type
         COL_TRC("Getting Column Data Type...");
         // Very strange issue: this call to sqlColAttribute changes the value of the second argument (j). This was a problem as the argument used to be i + 1, and it would
         // change i from 1 to -1 in the second iteration. This would cause the for loop to loop infinitely. Its not clear why - we are passing a copy to it, why would a 
         // value on stack change? 
         Result = pLoadedOdbcDll->sqlColAttribute( StatementHandle, j, SQL_DESC_CONCISE_TYPE, NULL, 0, 0, &OdbcDataType); //mimic SQLdescribeCol
         if (Result == SQL_ERROR){
            throwOdbcErrorWithMessage( SQL_HANDLE_STMT, StatementHandle, "" ,pOwner,__LINE__);
         }
         if (OdbcDataType == SQL_DECIMAL || OdbcDataType == SQL_NUMERIC) {
            Result = pLoadedOdbcDll->sqlColAttribute( StatementHandle, j, SQL_COLUMN_SCALE, NULL, 0, 0, &OdbcDataTypeScale);
            if (Result == SQL_ERROR){
               throwOdbcErrorWithMessage( SQL_HANDLE_STMT, StatementHandle, "" ,pOwner,__LINE__);
            }
            if (OdbcDataTypeScale == 0) {
               OdbcDataType = SQL_INTEGER;
            } 
         }
      }
      COL_TRC("Setting Column Data Type");
      DBdataType DbDataType = variantType( (SQLSMALLINT)OdbcDataType, StatementString, ColumnName);
      COL_TRC("Setting Column in result set...");
      COL_VAR2(ColumnName, DbDataType);
      pResultSet->setColumn( i, ColumnName, DbDataType );
      TreVariantDataTypeVector.push_back( DbDataType );
   }   

   // Build a vector of rows, where each row contains a vector of column values.
   COLuint32 RowIndex = 0;
   do {
      Result = pLoadedOdbcDll->sqlFetch( StatementHandle );
      if ( Result == SQL_ERROR ){
         throwOdbcErrorWithMessage( SQL_HANDLE_STMT, StatementHandle, "" ,pOwner,__LINE__);
      }
      if ( Result != SQL_NO_DATA ){
         COL_TRC("Setting data for row...");
         COL_VAR(RowIndex);
         if ( RowIndex >= ResultSetStart 
              && (( ResultSetSize == 0 ) || ( RowIndex < ResultSetStart + ResultSetSize )) ){
            DBresultSetRow& ResultSetRow = pResultSet->addRow();
            for ( SQLSMALLINT i = 0; i < CountOfResultSetColumn; i++ ){
               setResultSetColumnValue( StatementHandle,
                                        ResultSetRow,
                                        i,
                                        i,
                                        TreVariantDataTypeVector[i] );
            }
         }
         RowIndex++;
      }
   } while( Result != SQL_NO_DATA );

   pResultSet->setCountOfDatabaseRow( RowIndex );
   COL_TRC("Returning Result Set!");
   return pResultSet;
}

COLstring DBodbcErrorMessage(SQLSMALLINT HandleType, SQLHANDLE Handle, COLstring *pNativeErrorCode){
   COL_FUNCTION(DBgetOdbcError);
   COLstring Error("ODBC Error:");
   COLostream ErrorStream(Error);
   ErrorStream << newline;
   SQLRETURN Result;
   SQLSMALLINT Count = 1;
   do{
      SQLSMALLINT Size = 0;
      SQLCHAR Buffer[SQL_MAX_MESSAGE_LENGTH+1] = {0};
      SQLCHAR SqlStateRet[16] = {0};
      SQLINTEGER NativeError = 0;
      Result = pLoadedOdbcDll->sqlGetDiagRec(HandleType, Handle, Count, SqlStateRet, &NativeError, Buffer, sizeof(Buffer), &Size);
      pNativeErrorCode->append((char*)SqlStateRet);
      ErrorStream << COLstring ((char *)Buffer, Size) << newline;
      COL_VAR3(Count,Size, Result);
      COLPRECONDITION(Result != SQL_INVALID_HANDLE)
      Count++;
   } while( (Result == SQL_SUCCESS || Result == SQL_SUCCESS_WITH_INFO)  && Count < 100 ); // More than 100 and something is screwy...
   COL_VAR(Error);
   return Error;
}

void DBdatabaseOdbcPrivate::throwOdbcErrorWithMessage ( 
   const SQLSMALLINT& HandleType,
   const SQLHANDLE& Handle,
   const COLstring& ErrorMessage,
   DBdatabaseOdbc* pDatabase,
   const COLindex LineNumber
) {
   COL_FUNCTION(DBdatabaseOdbcPrivate::throwOdbcErrorWithMessage);
   SQLRETURN Result;
   SQLSMALLINT RecordNumber = 1;

   COLstring TotalErrorMessage;
   COLostream TotalErrorStream(TotalErrorMessage);
   
   TotalErrorStream << ErrorMessage << newline << COL_T("ODBC Error:") << newline;
   bool DisconnectRequired = false;
   COL_VAR(TotalErrorMessage);
   SQLINTEGER NativeErrorCode = 0;

   do{
      SQLSMALLINT OdbcErrorMessageLength = 0;
      SQLCHAR ErrorBuffer[SQL_MAX_MESSAGE_LENGTH+1] = {0};
      SQLCHAR SqlStateRet[16] = {0};
      {
         COL_TRC("About to call sqlGetDiagRec");
         Result = pLoadedOdbcDll->sqlGetDiagRec(HandleType,
                                                       Handle,
                                                       RecordNumber,
                                                       SqlStateRet,
                                                       &NativeErrorCode,
                                                       ErrorBuffer, // ODBC error message buffer
                                                       sizeof(ErrorBuffer), // ODBC error message buffer size
                                                       &OdbcErrorMessageLength );
         COL_TRC("Returned");
         COL_VAR3(RecordNumber,OdbcErrorMessageLength, Result);
         if ( Result == SQL_INVALID_HANDLE ){
            TotalErrorStream << "Unable to retrieve error information for ODBC handle " 
                             << Handle << " of type " 
                             << HandleType << ". The handle is invalid (line: " << LineNumber << ")."; 
            break;
         }
      }

      if ( Result == SQL_SUCCESS || Result == SQL_SUCCESS_WITH_INFO) {
         //if the sql state is a connection error, we must disconnect
         // sql state of '08XXX' represents connection problems
         if (SqlStateRet[0] == '0' && SqlStateRet[1] == '8') {
            COL_TRC("Disconnection is required from examining the SqlStateRet struture.");
            DisconnectRequired = true;
         }
         TotalErrorStream.sink()->write((void*)&ErrorBuffer, COL_MIN(OdbcErrorMessageLength, sizeof(ErrorBuffer)));
      }
      RecordNumber++;
   } while( (Result == SQL_SUCCESS || Result == SQL_SUCCESS_WITH_INFO)  && RecordNumber < 100 ); // More than 100 and something is screwy...

   //we disconnect if the error is the result of a connection problem
   if (pDatabase && DisconnectRequired){
      COL_TRC("Calling disconnect.");
      pDatabase->disconnect();
      COL_TRC("Done calling disconnect.");
   }

   COL_VAR(TotalErrorMessage);
   throw COLerror(TotalErrorMessage,LineNumber,__FILE__,NativeErrorCode);
}

DBdataType DBdatabaseOdbcPrivate::variantType(SQLSMALLINT OdbcDataType, const COLstring& StatementString, const COLstring& ColumnName){
   COL_METHOD(DBdatabaseOdbcPrivate::variantType);
   COL_TRC(OdbcDataType);
   switch ( OdbcDataType ){
      case SQL_CHAR: 
      case SQL_VARCHAR: 
      case SQL_WVARCHAR: 
      case SQL_LONGVARCHAR:
      case -8: // nchar
      case -10: // The "text" data type from PostgreSQL returns -10 for its ODBC data type.
                // Could not reverse find its alias from the standard ODBC header files
                // so we leave it as -10 here. Having the actual alias wouldn't make a bit
                // of difference though.
      case -96: // IBM DB2 - VARGRAPHIC
      case -370: // IBM DB2 - XML
         return DB_STRING;     
      case 11:  // It seems that Sybase uses 11 for timestamps, we hardcode it here.
                // in all likeliness, it might be because our odbc headers are out of date. 
      case SQL_DATETIME: case SQL_TYPE_TIME: case SQL_TYPE_TIMESTAMP: case SQL_TYPE_DATE: 
			return DB_DATETIME;
      case SQL_INTEGER:
         return DB_INTEGER;
      case SQL_SMALLINT:
         return DB_INTEGER;
      case SQL_TINYINT: 
      case SQL_BIT:
         return DB_INTEGER; 
      case SQL_BIGINT:
         return DB_LARGE_INTEGER;
      case SQL_FLOAT:
      case SQL_DECIMAL:
      case SQL_NUMERIC:
         return DB_DOUBLE;
      case SQL_DOUBLE: 
      case SQL_REAL:
         return DB_LARGE_DOUBLE;
      #ifdef _WIN32
      //SQL Native driver specific types (reference SQLncli header.)
      case SQL_SS_TIMESTAMPOFFSET: 
         return DB_STRING;
      case SQL_SS_XML:
         return DB_STRING;
      case SQL_SS_TIME2:
         return DB_STRING;
      case SQL_SS_VARIANT:
         return DB_STRING;
      #endif


   }

   if(OdbcDataType==-11 && pOwner->databaseType()==DB_SQL_SERVER) return DB_STRING; // uniqueidentifier

   // Binary types are only supported by translator
   if ( supportBinaryTypes() && isBinaryType(OdbcDataType) ) {
      return DB_BINARY;
   }

   COL_TRC("Database type not supported!!");

   // If the ODBC data type is not supported, we throw a user-friendly error message
   COLstring ErrorMessage;
   COLostream ErrorStream( ErrorMessage );
   ErrorStream << COL_T("Unsupported database type encountered while executing SQL query:") << newline
               << StatementString << newline << newline
               << COL_T("Column: ") << ColumnName << newline
               << COL_T("Database Type: (") << OdbcDataType << COL_T(") ");

   COL_VAR(ErrorMessage);
   COL_ERROR_STREAM( ErrorMessage, COLerror::PreCondition )
}

void DBdatabaseOdbcPrivate::setOption(SQLINTEGER Attribute, COLuint32 Value){ // note that we don't actually call this code in anything but Windows - see setTimeout
   COLuint64 NewValue = Value;
   SQLRETURN Result = pLoadedOdbcDll->sqlSetConnectAttr(pConnection->handle(),Attribute,(SQLPOINTER)NewValue, SQL_IS_UINTEGER );
   if (Result == SQL_ERROR){
      throwOdbcErrorWithMessage(SQL_HANDLE_DBC, pConnection->handle(), "" ,(DBdatabaseOdbc*)this,__LINE__);
   }
}

void DBdatabaseOdbcPrivate::setTimeout(COLuint32 iTimeoutInSeconds){
   COL_METHOD(DBdatabaseOdbcPrivate::setTimeout);
#ifndef _WIN32
   TimeoutInSeconds = iTimeoutInSeconds;
   return;
#endif
   if (!pConnection.get()) {
      TimeoutInSeconds = iTimeoutInSeconds;
      return;
   } else {
      setOption(SQL_ATTR_LOGIN_TIMEOUT, iTimeoutInSeconds);
      setOption(SQL_ATTR_QUERY_TIMEOUT, iTimeoutInSeconds);
      setOption(SQL_ATTR_CONNECTION_TIMEOUT, iTimeoutInSeconds);
      //ok, change stored value
      TimeoutInSeconds = iTimeoutInSeconds;
   }
}

static SQLRETURN
DBsqlGetData(const DBdatabaseOdbcPrivate* pPrivate,
             SQLHSTMT         Handle,
             SQLUSMALLINT     Col,       // Column Index
             SQLSMALLINT      Type,      // ODBC C-type Code (for output)
             SQLLEN           TypeSize,  // Size of C-type
             bool             IsBinary,  // Are we getting a string, or pure binary?
             SQLLEN          *pOutLen,   // OUT: Size of output or SQL_NULL_DATA
             COLstring       *pOut)      // OUT: Output buffer
{
   COL_FUNCTION(DBsqlGetData);

   SQLRETURN Ret;
   char Buf[4*1024];

   bool IsFilemaker = pPrivate->pOwner->databaseType() == DB_ODBC_FILEMAKER;
   pOut->clear();
   *pOutLen = 0;
   int Left=0;
   bool HaveLeft=false;
   // If we're getting a string, each chunk we get is null-terminated.
   // If we're getting binary data, chunks aren't null-terminated.
   const int AdjustmentSize = IsBinary ? 0 : TypeSize;
   COL_VAR(AdjustmentSize);
   for(;;) {
      SQLLEN Len = 0;
      int ToFetch=sizeof(Buf);
      if(IsFilemaker && HaveLeft){
         ToFetch = Left;
         if(ToFetch > sizeof(Buf))
         {
            ToFetch = sizeof(Buf);
         }
      };
      Ret = pLoadedOdbcDll->sqlGetData(Handle, Col, Type, Buf, ToFetch, &Len);
      if     (Ret == SQL_ERROR)      {                    break; }
      else if(Ret == SQL_NO_DATA)    { Ret = SQL_SUCCESS; break; }
      else if(Len == SQL_NULL_DATA)  { *pOutLen = Len;    break; }
      bool Enough = false;
      if(Len >= sizeof Buf || Len == SQL_NO_TOTAL) {
         COL_VAR3(Len, sizeof(Buf), SQL_NO_TOTAL);
         if(Len != SQL_NO_TOTAL)
         {
            HaveLeft=true;
            // "+ AdjustmentSize" needed to adjust for null characters, if present.
            Left = Len - (sizeof(Buf) - AdjustmentSize) + AdjustmentSize;
            COL_VAR(Left);
         };
         // "- AdjustmentSize" needed to adjust for null characters, if present.
         Len = sizeof(Buf) - AdjustmentSize;
      }
      else
      {
         Enough=true; 
      }
      pOut->write(Buf, (size_t)Len);
      *pOutLen += Len;
      if(IsFilemaker && Enough)
      {
         break;
      }
   }
   return Ret;
}

bool DBfetchString(const DBdatabaseOdbcPrivate* pPrivate, SQLHSTMT StatementHandle, int DatabaseColumnIndex, bool UseWideChar, COLstring& DataString)
{
   SQLRETURN Result;
   COLstring Buffer;
   SQLLEN DataLength=0;

   if(UseWideChar){
      Result = DBsqlGetData(pPrivate, StatementHandle, DatabaseColumnIndex + 1, SQL_WCHAR, sizeof(wchar_t), false, &DataLength, &Buffer);
   } else {
      Result = DBsqlGetData(pPrivate, StatementHandle, DatabaseColumnIndex + 1, SQL_C_CHAR, sizeof(char), false, &DataLength, &Buffer);
   }

   if (Result == SQL_ERROR) {
      pPrivate->throwOdbcErrorWithMessage( SQL_HANDLE_STMT, StatementHandle, "error retrieving string data" ,pPrivate->pOwner,__LINE__);
   }
   if (DataLength == SQL_NULL_DATA) {
      return false;
   }

   if(UseWideChar){
      DBstringWchar Wide;
      Wide.setWcharData((wchar_t*)Buffer.data(), Buffer.size() / sizeof(wchar_t));
      Buffer.clear();
      DataString = DBconvertWcharToUtf8(Wide);
   }else{
      DataString.append((char*)Buffer.data(),Buffer.size());
      Buffer.clear();
   }

   pPrivate->adjustEmptyString(DataString);
   DataString.stripAll( '\0' );

   return true;
}

void DBdatabaseOdbcPrivate::setResultSetColumnValue ( 
   const SQLHSTMT& StatementHandle,
   DBresultSetRow& ResultSetRow,
   const COLuint32& DatabaseColumnIndex,
   const COLuint32& ResultSetColumnIndex,
   DBdataType BaseType
) const {
   COL_METHOD(DBdatabaseOdbcPrivate::setResultSetColumnValue); 
   // a variable used by all branches - 
   // declaring it here so we don't have to have this hairy piece of code N times
   SQLLEN DataLength=0;

   bool UseWideChar = pOwner->useWideChar();
   if(pOwner->databaseType() == DB_ODBC_SYBASE_ASE) {
      UseWideChar = false; // SybaseASE seems to be returning empty strings when fetching in WideChar mode FIXME wtf
   }

   switch (BaseType) {
   case DB_STRING:
      {
         COLstring DataString;
         if(!DBfetchString(this, StatementHandle, DatabaseColumnIndex, UseWideChar, DataString)) // NULL
         {
            return;
         };
         ResultSetRow.setColumnValue(ResultSetColumnIndex, DataString);
      }
      break;
   case DB_DATETIME:
      {
         TIMESTAMP_STRUCT OdbcTimeStamp;
         memset(&OdbcTimeStamp, 0, sizeof(OdbcTimeStamp));
         DataLength=0;
         SQLRETURN Result = pLoadedOdbcDll->sqlGetData( StatementHandle, DatabaseColumnIndex + 1, SQL_C_TIMESTAMP, &OdbcTimeStamp, sizeof(OdbcTimeStamp), &DataLength );
         if ( Result == SQL_ERROR ) {
            throwOdbcErrorWithMessage( SQL_HANDLE_STMT, StatementHandle, "" ,pOwner,__LINE__);
         }
         if ( DataLength != SQL_NULL_DATA ) {
            COLdateTime DateTimeValue( OdbcTimeStamp.year, OdbcTimeStamp.month, OdbcTimeStamp.day, OdbcTimeStamp.hour, OdbcTimeStamp.minute, OdbcTimeStamp.second );
            ResultSetRow.setColumnValue( ResultSetColumnIndex, DateTimeValue );
         }
      }
      break;
   case DB_INTEGER:
      {
         DataLength=0;
         COLint32 IntegerValue=0;
         SQLRETURN Result = pLoadedOdbcDll->sqlGetData(StatementHandle, DatabaseColumnIndex + 1, SQL_C_SLONG, &IntegerValue, 0, &DataLength);
         if ( Result == SQL_ERROR ) {
            throwOdbcErrorWithMessage( SQL_HANDLE_STMT, StatementHandle, "" ,pOwner,__LINE__);
         }
         if ( DataLength != SQL_NULL_DATA ) {
            ResultSetRow.setColumnValue( ResultSetColumnIndex, IntegerValue );
         }
      }
      break;
   case DB_LARGE_INTEGER:
      {
         DataLength=0;
         COLint64 LargeIntegerValue=0;
         SQLRETURN Result = pLoadedOdbcDll->sqlGetData( StatementHandle, DatabaseColumnIndex + 1, SQL_C_SBIGINT, &LargeIntegerValue, 0, &DataLength );
         if ( Result == SQL_ERROR ) {
            throwOdbcErrorWithMessage( SQL_HANDLE_STMT, StatementHandle, "" ,pOwner,__LINE__);
         }
         if ( DataLength != SQL_NULL_DATA ) {
            ResultSetRow.setColumnValue( ResultSetColumnIndex, LargeIntegerValue );
         }
      }
      break;
   case DB_DOUBLE:
      {
         DataLength=0;
         COLfloat32 FloatValue=0;
         SQLRETURN Result = pLoadedOdbcDll->sqlGetData( StatementHandle, DatabaseColumnIndex + 1, SQL_C_FLOAT, &FloatValue, 0, &DataLength);
         if ( Result == SQL_ERROR ) {
            throwOdbcErrorWithMessage( SQL_HANDLE_STMT, StatementHandle, "" ,pOwner,__LINE__);
         }
         if (DataLength != SQL_NULL_DATA) {
            ResultSetRow.setColumnValue( ResultSetColumnIndex, FloatValue );
         }
      }
      break;
   case DB_LARGE_DOUBLE:
      {
         DataLength=0;
         COLfloat64 DoubleValue=0;
         SQLRETURN Result = pLoadedOdbcDll->sqlGetData(StatementHandle, DatabaseColumnIndex + 1, SQL_C_DOUBLE, &DoubleValue, 0, &DataLength);
         if ( Result == SQL_ERROR ) {
            throwOdbcErrorWithMessage( SQL_HANDLE_STMT, StatementHandle, "" ,pOwner,__LINE__);
         }
         if ( DataLength != SQL_NULL_DATA ) {
            ResultSetRow.setColumnValue( ResultSetColumnIndex, DoubleValue );
         }
      }
      break;
   case DB_BINARY:
      {
         COLstring SimpleBuffer;
         SQLRETURN Result = DBsqlGetData(this, StatementHandle, DatabaseColumnIndex + 1, SQL_C_BINARY, sizeof(char), true, &DataLength, &SimpleBuffer);
         if ( Result == SQL_ERROR ) {
            throwOdbcErrorWithMessage( SQL_HANDLE_STMT, StatementHandle, "" ,pOwner,__LINE__);
         }
         if ( DataLength != SQL_NULL_DATA ) {
            ResultSetRow.setColumnValue( ResultSetColumnIndex, DBvariant(SimpleBuffer, DB_BINARY) );
         }
      }
      break;
   default:
      COL_ERROR_STREAM("Unknown data type.", COLerror::PreCondition);
   }
}

DBdatabaseOdbc::DBdatabaseOdbc(){
   COL_METHOD(DBdatabaseOdbc::DBdatabaseOdbc);
   pMember =  new DBdatabaseOdbcPrivate(this);
}

DBdatabaseOdbc::~DBdatabaseOdbc(){
   COL_METHOD(DBdatabaseOdbc::~DBdatabaseOdbc);
   try{
      pMember->pConnection = NULL;
      pMember->pEnvironment = NULL; // destruction order is important
      delete pMember;
   }
   catch(const COLerror& Error){
      COL_TRC("Error raised in destructor:" << Error);
   }
}

bool DBdatabaseOdbc::initialize(COLostream& LogStream, const char* LibraryPath){
   COL_FUNCTION(DBdatabaseOdbc::initialize);
   if ( !isInitialized() ){
      COL_TRC("Trying to load ODBC...");
      LogStream << COL_T("Trying to load ODBC... ");
      COLdll* pDll = new COLdll();
      int ErrorCode = COL_DLL_LOAD_OK;
      COL_TRC("Loading from library path...");
      ErrorCode = pDll->tryLoadLibrary(LibraryPath);
      COL_VAR2(pDll->libraryName(), pDll->loaded());
      if(!pDll->loaded()){ 
         COL_TRC("NO (ODBC support not present)");
         LogStream << COL_T("NO (ODBC support not present)") << newline;
         if(ErrorCode == COL_DLL_LOAD_NO_PERMISSIONS) { 
            COL_TRC("Not enough permissions to load ODBC.");
            LogStream << COL_T("Not enough permissions to load ODBC.") << newline; 
         } 
         else if(ErrorCode == COL_DLL_LOAD_INVALID_LIBRARY) { 
            COL_TRC("Not a valid ODBC library.");
            LogStream << COL_T("The ODBC library that was found in an invalid library.") << newline; 
         }
      } else {
         LogStream << pDll->libraryName() << ' ';
         try {
            pLoadedOdbcDll = new DBodbcDynamic(pDll);
            COL_TRC("OK (ODBC support enabled)");
            LogStream << COL_T(" OK (ODBC support enabled)") << newline;
         } catch(...){
            COL_TRC("Not supported");
            LogStream << COL_T(" NO (not supported)") << newline;
         }
      }
   } else {
      COL_TRC("ODBC already initialized");
   }
   return isInitialized();
}

bool DBdatabaseOdbc::isInitialized(){
   COL_FUNCTION(DBdatabaseOdbc::isInitialized);
   return (pLoadedOdbcDll != NULL);
}

const COLstring& DBdatabaseOdbc::loadedApiLibraryPath(){
   COL_FUNCTION(DBdatabaseOdbc::loadedApiLibraryPath);
   COLPRECONDITION(isInitialized());
   return pLoadedOdbcDll->libraryName();
}

void DBdatabaseOdbc::connect(const char* Source, const char* User, const char* Password){ 
   COL_METHOD(DBdatabaseOdbc::connect);
   COL_VAR3(Source, User, Password);
   if (!isConnected() || cachedDataSourceName() != Source|| cachedUserName() != User || cachedPassword() != Password){
      pMember->pConnection = NULL;
      pMember->pEnvironment = NULL; // destruction order is important
      setCachedAutoCommitFlag(true);
   } else {
      return; //no need to reconnect
   }

   // Every new ODBC connection will have its own environment handle,
   // preventing stale, never deallocated environment handles from occurring.
   pMember->pEnvironment = new DBodbcEnvironment();
   pMember->pConnection = new DBodbcConnection(pMember->pEnvironment);
   //always set up timeout before issuing connect
   pMember->setTimeout(pMember->TimeoutInSeconds);

   SQLRETURN Result = 0;
   COL_TRC("About to call sqlConnectLocked");
   Result = pLoadedOdbcDll->sqlConnectLocked(pMember->pConnection->handle(), (SQLCHAR*)Source, strlen(Source), (SQLCHAR*)User, strlen(User), (SQLCHAR*)Password, strlen(Password) );
   if (Result == SQL_ERROR){
      COL_TRC("Oh dear an error occured.");
      // failed to actually connect -
      // we throw an ODBC error message, but also kill handles and clear transaction on the way out
      COLerror Error = createErrorObject(pMember->pConnection->handle());

      // No need to disconnect since connect failed. See #15272
      // destruction order is important
      pMember->pConnection->clearHandle();
      pMember->pConnection = NULL;
      pMember->pEnvironment = NULL; 
      COL_TRC("Destroyed the connection and environment pointers.");
      setCachedAutoCommitFlag(true);
      throw Error;
   }
   COL_TRC("Connected!");

   if(pMember->UseUnicode) {
      // NOTE : MyOdbc 3.51+ does not support "set names" - the driver itself must be configured to "unicode" mode
      //        If that changes in the future, uncomment the extra check below
      if (databaseType() == DB_ODBC_POSTGRESQL /*|| databaseType() == DB_ODBC_MYSQL*/){
         COL_TRC("Call set names 'utf8' for Postgres/MySql.");
         executeSqlString("set names 'utf8'");
      }
   };

   //Now Connected
   COL_TRC("Caching stuff.");
   setCachedDataSourceName(Source);
   setCachedUserName(User);
   setCachedPassword(Password);
}

void DBdatabaseOdbc::disconnect(){
   COL_METHOD(DBdatabaseOdbc::disconnect);
   pMember->pConnection = NULL;
   pMember->pEnvironment = NULL; // destruction order is important
   setCachedAutoCommitFlag(true);
}

DBresultSetPtr DBdatabaseOdbcPrivate::fetchDatabaseTablesWithTableType(const COLstring& iTableType){
   COL_METHOD(DBdatabaseOdbcPrivate::fetchDatabaseTablesWithTableType);
   DBodbcStatement Statement(pConnection);
   SQLRETURN Result;

   SQLCHAR* TableType = NULL;
   SQLSMALLINT TableTypeSize = 0;
   if (!iTableType.is_null()) {
      TableType = (SQLCHAR*) iTableType.c_str();
      TableTypeSize = iTableType.size();
   }

   Result =  pLoadedOdbcDll->sqlTables( Statement.handle(), NULL, 0, NULL, 0, NULL, 0, TableType, TableTypeSize );
   if ( Result == SQL_ERROR ) {
      throwOdbcErrorWithMessage( SQL_HANDLE_STMT, Statement.handle(), "" ,(DBdatabaseOdbc*)this,__LINE__);
   }

   DBresultSetPtr pResultSet = new DBresultSet();  
   pResultSet->addColumn( "TABLE_NAME", DB_STRING );
   COLuint32 RowIndex = 0;
   // Build a vector of rows, where each row contains a vector of column values.
   while ( true ) {
      Result = pLoadedOdbcDll->sqlFetch( Statement.handle());
      if ( Result == SQL_ERROR ) {
         throwOdbcErrorWithMessage( SQL_HANDLE_STMT, Statement.handle(), "" ,(DBdatabaseOdbc*)this,__LINE__);
      }
      if ( Result == SQL_NO_DATA ) {
         break;
      }
      pResultSet->addRow();
      setResultSetColumnValue( Statement.handle(), pResultSet->row(RowIndex), 2, 0, DB_STRING ); // Table name is in column index 2
      RowIndex++;
   }

   pResultSet->setCountOfDatabaseRow( RowIndex );
   return pResultSet;
}

void DBdatabaseOdbcPrivate::prepareForSqlBind(DBodbcStatement &Statement, const COLstring &SqlInsertString, const DBsqlInsert& SqlInsertCommand) {
   SQLRETURN Result = pLoadedOdbcDll->sqlPrepare( Statement.handle(), (SQLCHAR*)SqlInsertString.c_str(), SqlInsertString.size() );
   if (Result == SQL_ERROR){
      throwOdbcErrorWithMessage( SQL_HANDLE_STMT, Statement.handle(), 
         DBdatabaseOdbcRecreateSqlQuery(*pOwner, SqlInsertCommand, SqlInsertString), pOwner ,__LINE__);
   }
}

DBresultSetPtr DBdatabaseOdbc::fetchDatabaseTables() const         { return pMember->fetchDatabaseTablesWithTableType(""); }
// DBresultSetPtr DBdatabaseOdbc::fetchNonSystemDatabaseTables() const{ return pMember->fetchDatabaseTablesWithTableType("TABLE"); }

bool DBdatabaseOdbc::isSystemTable( const char* TableName ) const{
   COL_METHOD(DBdatabaseOdbc::isSystemTable);
   DBodbcStatement Statement(pMember->pConnection);
   SQLRETURN Result =  pLoadedOdbcDll->sqlTables(Statement.handle(), NULL, 0, NULL, 0, (SQLCHAR*)TableName, strlen(TableName), NULL, 0);
   if (Result == SQL_ERROR){
      pMember->throwOdbcErrorWithMessage( SQL_HANDLE_STMT, Statement.handle(), "" ,(DBdatabaseOdbc*)this,__LINE__);
   }

   DBresultSet ResultSet;  
   ResultSet.addColumn( "TABLE_TYPE", DB_STRING );

   Result = pLoadedOdbcDll->sqlFetch( Statement.handle());
   if (Result == SQL_ERROR ){
      pMember->throwOdbcErrorWithMessage( SQL_HANDLE_STMT, Statement.handle(), "" ,(DBdatabaseOdbc*)this,__LINE__);
   }
   
   if (Result == SQL_NO_DATA ){
      return false;
   }
   ResultSet.addRow();
   pMember->setResultSetColumnValue( Statement.handle(), ResultSet.row(0), 3, 0, DB_STRING); // Table type is in column index 3 
   return ResultSet.value(0, 0).string().compare("TABLE")!=0; // "TABLE" is the ODBC keyword for a regular user table
}

void DBdatabaseOdbc::buildPrimaryKeyList(COLarray<COLstring>& PrimaryKeyList, const char* TableName) const{
   if(pMember->pConnection.get() == NULL){ throw COLerror("No connection found for ODBC database. Exiting. "); }
   PrimaryKeyList.clear();
   
   DBodbcStatement Statement(pMember->pConnection.get());
   SQLRETURN Result = pLoadedOdbcDll->sqlPrimaryKeys( Statement.handle(), NULL, 0, NULL, 0, (unsigned char*)TableName, strlen(TableName) );
   if (Result == SQL_ERROR){
      pMember->throwOdbcErrorWithMessage( SQL_HANDLE_STMT, Statement.handle(), "" ,(DBdatabaseOdbc*)this,__LINE__);
   }

   while (true) {
      Result = pLoadedOdbcDll->sqlFetch( Statement.handle() );
      if ( Result == SQL_ERROR ) {
         pMember->throwOdbcErrorWithMessage( SQL_HANDLE_STMT, Statement.handle(), "" ,(DBdatabaseOdbc*)this,__LINE__);
      } else if ( Result == SQL_NO_DATA ) {
         break;
      }
      // To find out the length
      SQLLEN DataLength = 0;
      char DummyBuffer[1] = {0};
      SQLSMALLINT DataColumn = 4;
      SQLRETURN Result = pLoadedOdbcDll->sqlGetData(Statement.handle(), DataColumn, SQL_C_CHAR, DummyBuffer, sizeof(DummyBuffer), &DataLength);
      if ( Result == SQL_ERROR ) {
         pMember->throwOdbcErrorWithMessage( SQL_HANDLE_STMT, Statement.handle(), "" ,(DBdatabaseOdbc*)this,__LINE__);
      }
      else if ( DataLength == SQL_NULL_DATA ) {
         break;
      }

      // Get the real string
      COLstring ColumnName;
      ColumnName.assign( DataLength, '\0' );

      Result = pLoadedOdbcDll->sqlGetData( Statement.handle(), DataColumn, SQL_C_CHAR, ColumnName.get_buffer(), ColumnName.size() + 1, NULL ); // Length of data
      if (Result == SQL_ERROR) {
         pMember->throwOdbcErrorWithMessage( SQL_HANDLE_STMT, Statement.handle(), "" ,(DBdatabaseOdbc*)this,__LINE__);
      }
      PrimaryKeyList.push_back(ColumnName);
   }
}


DBresultSetPtr DBdatabaseOdbc::fetchDatabaseTableColumns( const char* TableName ) const {
   // Before opening a connection, we will find the primary key(s)
   // If we try accessing a list of the primary keys after a connection
   // is established, ODBC is not happy.
   COLarray<COLstring> PrimaryKeyList;
   buildPrimaryKeyList(PrimaryKeyList, TableName);

   DBodbcStatement Statement(pMember->pConnection);
   SQLRETURN Result;
   
   Result = pLoadedOdbcDll->sqlColumns( Statement.handle(),
                                 NULL, // Catalog name
                                 0, // Catalog name length
                                 NULL, // Schema name search pattern
                                 0, // Schema name length
                                 (unsigned char*)TableName, 
                                 strlen(TableName),
                                 NULL, // Column name search pattern
                                 0 ); // Column name length
   if ( Result == SQL_ERROR ) {
      pMember->throwOdbcErrorWithMessage( SQL_HANDLE_STMT, Statement.handle(), "" ,(DBdatabaseOdbc*)this,__LINE__);
   } 

   DBresultSetPtr pResultSet = new DBresultSet();

   const COLuint32 CountOfResultSetColumn = 6;
   pResultSet->resizeColumnVector( CountOfResultSetColumn );

   COLuint32 ResultSetColumnIndex = 0;
   // Build the column name vector for the result set.
   pResultSet->setColumn( ResultSetColumnIndex++, "COLUMN_NAME",   DB_STRING );
   pResultSet->setColumn( ResultSetColumnIndex++, "DATA_TYPE",     DB_INTEGER );
   pResultSet->setColumn( ResultSetColumnIndex++, "DEFAULT_VALUE", DB_DATA_TYPE_NOT_DEFINED );
   pResultSet->setColumn( ResultSetColumnIndex++, "LENGTH",        DB_INTEGER );
   pResultSet->setColumn( ResultSetColumnIndex++, "REQUIRED",      DB_BOOLEAN );
   pResultSet->setColumn( ResultSetColumnIndex++, "PRIMARY_KEY",   DB_BOOLEAN );

   COLuint32 RowIndex = 0;
   while ( true ) {  
      Result = pLoadedOdbcDll->sqlFetch( Statement.handle() );
      if ( Result == SQL_ERROR ) {
         pMember->throwOdbcErrorWithMessage( SQL_HANDLE_STMT, Statement.handle(), "" ,(DBdatabaseOdbc*)this,__LINE__);
      } 
      if ( Result == SQL_NO_DATA ) {
         break;
      }
      pResultSet->addRow();
      ResultSetColumnIndex = 0;

      // Column: "COLUMN_NAME"
      pMember->setResultSetColumnValue( Statement.handle(), pResultSet->row(RowIndex), 3, ResultSetColumnIndex++, DB_STRING ); // Database column index 
      // Column: "DATA_TYPE"
      SQLSMALLINT OdbcDataType;
      SQLLEN OdbcDataTypeScale = 0;
      Result = pLoadedOdbcDll->sqlGetData( Statement.handle(), 5, SQL_C_SSHORT, &OdbcDataType, 0, NULL );
      if ( Result == SQL_ERROR ) {
         pMember->throwOdbcErrorWithMessage( SQL_HANDLE_STMT, Statement.handle(), "" ,(DBdatabaseOdbc*)this,__LINE__);
      } 
      COLstring TypeErrorMessage ="Retrieving column information from database table '";
      TypeErrorMessage += TableName;
      TypeErrorMessage += "'";
      DBdataType DataType;
      try {
         if (OdbcDataType == SQL_DECIMAL || OdbcDataType == SQL_NUMERIC) {
            Result = pLoadedOdbcDll->sqlColAttribute( Statement.handle(), 5, SQL_COLUMN_SCALE, NULL, 0, 0, &OdbcDataTypeScale);
            if (Result == SQL_ERROR){
               pMember->throwOdbcErrorWithMessage( SQL_HANDLE_STMT, Statement.handle(), "" ,(DBdatabaseOdbc*)this,__LINE__);
            }
            if (OdbcDataTypeScale == 0) {
               OdbcDataType = SQL_INTEGER;
            } 
         }
         DataType = pMember->variantType(OdbcDataType, TypeErrorMessage, "DATA_TYPE");
      } catch (const COLerror& Error) {
         DataType = DB_DATA_TYPE_NOT_DEFINED;
      }
      pResultSet->setValue( RowIndex, ResultSetColumnIndex++, DataType );

      // Column: "DEFAULT_VALUE"     
      ResultSetColumnIndex++; // Cannot get default column values from ODBC API
      // Column: "LENGTH"
      pMember->setResultSetColumnValue( Statement.handle(), pResultSet->row(RowIndex), 7, ResultSetColumnIndex++, DB_INTEGER ); 
      // Column: "REQUIRED"
      SQLSMALLINT NullableFlag;
      Result = pLoadedOdbcDll->sqlGetData( Statement.handle(), 11, SQL_C_SSHORT, &NullableFlag, 0, NULL );
      if ( Result == SQL_ERROR ) {
         pMember->throwOdbcErrorWithMessage( SQL_HANDLE_STMT, Statement.handle(), "" ,(DBdatabaseOdbc*)this,__LINE__);
      } 

      pResultSet->setValue( RowIndex, ResultSetColumnIndex++, NullableFlag == SQL_NO_NULLS );

      // Column: "PRIMARY_KEY"
      bool IsPrimaryKey = false;
      for (COLindex KeyIndex = 0; KeyIndex < PrimaryKeyList.size(); ++KeyIndex) {
         if ((pResultSet->value(RowIndex, 0)).string() == PrimaryKeyList[KeyIndex]) {
            IsPrimaryKey = true;
         }
      }
      pResultSet->setValue( RowIndex, ResultSetColumnIndex++, IsPrimaryKey);
      RowIndex++;
   }
   pResultSet->setCountOfDatabaseRow( RowIndex );
   return pResultSet;
}

void DBdatabaseOdbcCreateSqlUpdateWithoutBinding( const DBdatabaseOdbc& Database, const DBsqlUpdate& SqlCommand, COLostream& OutputSql) {
   COLstring TempSqlString;
   COLostream TempSqlStream(TempSqlString);
   Database.createSqlUpdate( SqlCommand, TempSqlStream);
   if(Database.useBinding()) {
      TempSqlString = DBdatabaseOdbcRecreateSqlQuery(Database, SqlCommand, TempSqlString);
   }
   OutputSql << TempSqlString;
};

void DBdatabaseOdbcCreateSqlInsertWithoutBinding( const DBdatabaseOdbc& Database, const DBsqlInsert& SqlCommand, COLostream& OutputSql) {
   COLstring TempSqlString;
   COLostream TempSqlStream(TempSqlString);
   Database.createSqlInsert( SqlCommand, TempSqlStream);
   if(Database.useBinding()) {
      TempSqlString = DBdatabaseOdbcRecreateSqlQuery(Database, SqlCommand, TempSqlString);
   }
   OutputSql << TempSqlString;
};

void DBdatabaseOdbcPrivate::
rebuildForExecute(COLstring&         SqlString,
                  const DBsqlInsert& SqlCommand,
                  void (*createSql)(const DBdatabase*, const DBsqlInsert&, COLostream&),
                  SQLHSTMT           SqlHandle)
{
   int c, ncol = (int)SqlCommand.countOfColumn();
   int r, nrow = (int)SqlCommand.countOfRows();
   COLarray<DBodbcColInfo> ColInfo(ncol);
   bool MadeChanges = false;

   if(pOwner->supportsDescribeParam() &&
      getColInfo(ColInfo, SqlCommand, SqlString, SqlHandle) != SQL_ERROR)
   {
      // If any target columns are binary, we need to convert all
      // corresponding string values in the command to binary.
      for(r = 0; r < nrow; r++) {
         for(c = 0; c < ncol; c++) {
            if(!isBinaryType(ColInfo[c].SqlDataType))
               continue;
            DBvariant& Value = const_cast<DBvariant&>(SqlCommand.columnValue(c,r));
            if(Value.dataType() != DB_STRING) {
               ColInfo[c].SqlDataType = 0;  // So we undo correctly.
               continue;
            }
            COLstring Str;
            Str.swap(&Value.string());
            Value = DBvariant(COLstring(), DB_BINARY);
            COLstring& Buf = Value.binary();
            Buf.setCapacity(Str.size());
            Buf.setSize(Str.size());
            memcpy((void*)Buf.get_buffer(), Str.c_str(), Str.size());
            MadeChanges = true;
         }
      }
   }

   // We have to remake the SQL without bindings.
   SqlString.clear();
   COLostream SqlStream(SqlString);
   createSql(pOwner, SqlCommand, SqlStream);

   // We must undo any string->binary conversions that were made.
   if(MadeChanges) {
      for(r = 0; r < nrow; r++) {
         for(c = 0; c < ncol; c++) {
            if(!isBinaryType(ColInfo[c].SqlDataType))
               continue;
            DBvariant& Value = const_cast<DBvariant&>(SqlCommand.columnValue(c,r));
            if(Value.dataType() != DB_BINARY)
               continue;
            COLstring& Buf = Value.binary();
            COLstring Str = Buf;
            Value = COLstring();
            Value.string().swap(&Str);  // Avoid second copy.
         }
      }
   }
}

DBresultSetPtr DBdatabaseOdbc::
executeSqlInsertOrUpdate(const DBsqlInsert& SqlCommand,
                         void (*createSql)(const DBdatabase*,
                                           const DBsqlInsert&,
                                           COLostream&)) const
{
   COLstring SqlString;
   COLostream SqlStream(SqlString);

   if(useBinding()) {
      createSql(this, SqlCommand, SqlStream);
      DBodbcStatement Statement(pMember->pConnection);
      pMember->prepareForSqlBind(Statement, SqlString, SqlCommand);
      pMember->executeSqlBind(SqlCommand, SqlString, Statement.handle());
   } else {
      // Okay, this part is a bit shifty.  We need to create a SQL command
      // with bindings so we can ask ODBC about the types of the columns.
      // We need to know if any target columns are binary.  Then we create
      // a new, binding-free SQL command with binary values instead of 
      // strings (as needed).  See rebuildForExecute() for more details.
      bool UseUnicode = pMember->UseUnicode; pMember->UseUnicode = false;
      bool UseBinding = pMember->UseBinding; pMember->UseBinding = true;
      try {
         COLPRECONDITION(useBinding());
         createSql(this, SqlCommand, SqlStream);  // With bindings.
         pMember->UseUnicode = UseUnicode;
         pMember->UseBinding = UseBinding;
         COLPOSTCONDITION(!useBinding());
      } catch(...) {
         pMember->UseUnicode = UseUnicode;
         pMember->UseBinding = UseBinding;
         throw;
      }
      DBodbcStatement Statement(pMember->pConnection);
      pMember->prepareForSqlBind(Statement, SqlString, SqlCommand);
      pMember->rebuildForExecute(SqlString, SqlCommand, createSql, Statement.handle());
      executeSqlString(SqlString);
   };

   COL_VAR(SqlString);
   return NULL;
}

// JULIAN-CHECK this appears to be legit overide (of createSqlInsert from DBdatabase)
static void createSqlInsert(const DBdatabase* pDb, const DBsqlInsert& Insert, COLostream& Out) {
   pDb->createSqlInsert(Insert, Out);
}
// JULIAN-CHECK this appears to be legit overide (of createSqlUpdate from DBdatabase)
static void createSqlUpdate(const DBdatabase* pDb, const DBsqlInsert& Update, COLostream& Out) {
   pDb->createSqlUpdate(dynamic_cast<const DBsqlUpdate&>(Update), Out);
}

DBresultSetPtr DBdatabaseOdbc::executeSqlInsert( const DBsqlInsert& SqlInsertCommand ) const {
   COL_METHOD(DBdatabaseOdbc::executeSqlInsert);
   return executeSqlInsertOrUpdate(SqlInsertCommand, ::createSqlInsert);
}

DBresultSetPtr DBdatabaseOdbc::executeSqlUpdate( const DBsqlUpdate& SqlUpdateCommand ) const {
   COL_METHOD(DBdatabaseOdbc::executeSqlUpdate);
   return executeSqlInsertOrUpdate(SqlUpdateCommand, ::createSqlUpdate);
}

DBresultSetPtr DBdatabaseOdbc::executeSqlString(const COLstring& SqlString, COLuint32 Start, COLuint32 Size, COLarray<DBresultSetPtr> *pAllResultSets) const{
   COL_METHOD(DBdatabaseOdbc::executeSqlString);
   COLstring SuccessWithInfoMessage;
   if(pMember->pConnection.get() == NULL){ throw COLerror("No connection found for ODBC database. Exiting. "); }
   DBodbcStatement Statement(pMember->pConnection);
   COL_VAR3(SqlString, Start,Size);
   SQLRETURN Result;
   if(useWideChar()){
      DBstringWchar WcharSqlString;
      DBconvertUtf8ToWchar(WcharSqlString, SqlString);
      Result = pLoadedOdbcDll->sqlExecDirectW(Statement.handle(), (SQLWCHAR*)WcharSqlString.data(), WcharSqlString.size());
   }else{
      Result = pLoadedOdbcDll->sqlExecDirect( Statement.handle(), (unsigned char*)SqlString.c_str(), SqlString.size() );
   }

   if (Result == SQL_ERROR){
      pMember->throwOdbcErrorWithMessage( SQL_HANDLE_STMT, Statement.handle(), SqlString ,(DBdatabaseOdbc*)this,__LINE__);
      COL_VAR(SqlString);
   } else if (Result == SQL_SUCCESS_WITH_INFO) {
      // Save the error (it will get overwritten by the next call to ODBC)
      try {
         pMember->throwOdbcErrorWithMessage( SQL_HANDLE_STMT, Statement.handle(), SqlString ,(DBdatabaseOdbc*)this,__LINE__);
      } catch (COLerror &Err) {
         SuccessWithInfoMessage = Err.description();
         SuccessWithInfoMessage.append ("\n\n");
      }
   }

   DBresultSetPtr pResultSet = NULL;
   // Collect all returned tokens.  This is workaround for the bug described in #7067. Note that if the SQL statement 
   // returns more than one result set, this will only return the last one.
   while (Result != SQL_NO_DATA) {
      if ( (Result == SQL_SUCCESS) || (Result == SQL_SUCCESS_WITH_INFO) ) {
         SQLSMALLINT CountOfResultSetColumn = -1;
         Result = pLoadedOdbcDll->sqlNumResultCols( Statement.handle(), &CountOfResultSetColumn );
         if (Result == SQL_ERROR){
            pMember->throwOdbcErrorWithMessage(SQL_HANDLE_STMT, Statement.handle(), SuccessWithInfoMessage + SqlString, (DBdatabaseOdbc*)this, __LINE__);
         }
         if (CountOfResultSetColumn == 0){
            pResultSet = NULL;
         }else{
            pResultSet = pMember->fetchOdbcResultSet(Statement.handle(), SqlString, CountOfResultSetColumn, Start, Size);
         }
      }

      Result = pLoadedOdbcDll->sqlMoreResults(Statement.handle());
      if (Result == SQL_ERROR) {
         pMember->throwOdbcErrorWithMessage(SQL_HANDLE_STMT, Statement.handle(), SuccessWithInfoMessage + SqlString, (DBdatabaseOdbc*)this, __LINE__);
      }
      
      if (pAllResultSets != NULL && pResultSet.get() != NULL) {
         pAllResultSets->push_back(pResultSet);
      }
   }
   return pResultSet; // The last one only
}

void DBdatabaseOdbc::setAutocommitFlag(bool AutoCommit) const{
   COL_METHOD(DBdatabaseOdbc::setAutocommitFlag);
   SQLRETURN Result;
   SQLPOINTER CommitFlag = (SQLPOINTER)(AutoCommit ? SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF);
   Result = pLoadedOdbcDll->sqlSetConnectAttr( pMember->pConnection->handle(), SQL_ATTR_AUTOCOMMIT, CommitFlag, SQL_IS_POINTER );
   if (Result == SQL_ERROR){
      pMember->throwOdbcErrorWithMessage( SQL_HANDLE_DBC, pMember->pConnection->handle(), "" ,(DBdatabaseOdbc*)this,__LINE__);
   }
}

void DBdatabaseOdbc::commitTransaction(){
   COL_METHOD(DBdatabaseOdbc::commitTransaction);
   setCachedAutoCommitFlag(true);
   SQLRETURN Result = pLoadedOdbcDll->sqlEndTran(SQL_HANDLE_DBC, pMember->pConnection->handle(), SQL_COMMIT);
   if (Result == SQL_ERROR){
      COL_TRC("Commit transaction failed.");
      COLerror Error = createErrorObject(pMember->pConnection->handle());

      pMember->pConnection = NULL;
      pMember->pEnvironment = NULL; // destruction order is important
      throw Error;
   } else {
      setAutocommitFlag(true);
   }
}

void DBdatabaseOdbc::rollbackTransaction(){
   COL_METHOD(DBdatabaseOdbc::rollbackTransaction);
   setCachedAutoCommitFlag(true);
   SQLRETURN Result = pLoadedOdbcDll->sqlEndTran(SQL_HANDLE_DBC, pMember->pConnection->handle(), SQL_ROLLBACK); 
   if (Result == SQL_ERROR){
      COL_TRC("Rollback transaction failed.");
      COLerror Error = createErrorObject(pMember->pConnection->handle());
      COL_TRC("Destroy the connection so it will notify statements properly.");
      pMember->pConnection = NULL;
      pMember->pEnvironment = NULL; // destruction order is important
      throw Error;
   } else {
      setAutocommitFlag(true);
   }
}

static const DBodbcErrorList AcceptableErrorStrings[] = {"00000"};
COLstring DBdatabaseOdbc::acceptableErrorString(COLuint32 ErrorIndex) const { return AcceptableErrorStrings[ErrorIndex].Code; }

COLuint32 DBdatabaseOdbc::acceptableErrorStringSize() const{
   COL_METHOD(DBdatabaseOdbc::acceptableErrorStringSize);
   return sizeof(AcceptableErrorStrings) / sizeof(AcceptableErrorStrings[0]);
}

bool DBdatabaseOdbc::connectionExists(){
   COL_METHOD(DBdatabaseOdbc::connectionExists);
   return pMember->pConnection.get() != NULL;
}

DBodbcStatement* DBdatabaseOdbc::statementSucceeds(){  
   COL_METHOD(DBdatabaseOdbc::statementSucceeds); 
   DBodbcStatement* pStatement = NULL;
   try {
      pStatement = new DBodbcStatement(pMember->pConnection);
   }
   catch(...) {
      if (pStatement != NULL) {
         delete pStatement;
      }
      pStatement = NULL;
   }
   return pStatement;
}

bool DBdatabaseOdbc::pingSucceeds(DBodbcStatement* pStatement){
   COL_METHOD(DBdatabaseOdbc::pingSucceeds);
   SQLRETURN Result = pLoadedOdbcDll->sqlExecDirect ( pStatement->handle(), (unsigned char*)sqlPingCommand().c_str(), sqlPingCommand().size() );
   return (Result != SQL_ERROR);
}

bool DBdatabaseOdbc::handledErrorIsOk(DBodbcStatement* pStatement){
   COL_METHOD(DBdatabaseOdbc::handledErrorIsOk);
   //Return code for a syntax error.
   COLuint32 ErrorStringCount = acceptableErrorStringSize();
   SQLRETURN   ResultCode;
   SQLCHAR       SqlState[6];
   SQLCHAR Msg[SQL_MAX_MESSAGE_LENGTH];
   SQLINTEGER  NativeError;
   SQLSMALLINT MsgLength;
   COLstring   ErrorMessage;
   SQLSMALLINT RecordNumber=1;
   
   ResultCode = pLoadedOdbcDll->sqlGetDiagRec(SQL_HANDLE_STMT, pStatement->handle(), RecordNumber, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLength);
   COLstring ResultCodeString = (char*)SqlState;
   for (COLuint32 i = 0; i < ErrorStringCount; ++i) {
      if (ResultCodeString == acceptableErrorString(i)) {
         return true;
      }
   }
   return false;
}

bool DBdatabaseOdbc::isConnected(){
   COL_METHOD(DBdatabase::isConnected()); 
   if (!connectionExists()){
      COL_TRC("No connection object exists so hasConnectionByGuid returns false");
      return false;
   }
   COLauto<DBodbcStatement> pStatement = statementSucceeds();
   if (!pStatement.get()){
      COL_TRC("Statement failed.  hasConnectionByGuid = false");
      return false;
   }

   if (pingSucceeds(pStatement.get()) || handledErrorIsOk(pStatement.get())){
      COL_TRC("Ping succeeded.  We are connected.");
      return true;
   }
   return false;
}

void DBdatabaseOdbc::streamInsertUpdateValue(COLostream& Stream, const DBsqlInsert& SqlCommand, COLuint32 ColumnIndex) const{
   COL_METHOD(DBdatabaseOdbc::streamInsertUpdateValue);
   if (SqlCommand.columnValueIsColumnFlag(ColumnIndex) == false){
      if ( SqlCommand.columnValue(ColumnIndex).dataType() == DB_DATA_TYPE_NOT_DEFINED ){
         streamNullValue( Stream );
      }else if(!useBinding()){
         DBdatabase::streamInsertUpdateValue(Stream, SqlCommand, ColumnIndex);
      }else{
         Stream << '?';
      }
   } else {
      Stream << SqlCommand.columnValue(ColumnIndex).toString(); 
   }
}

void DBdatabaseOdbc::streamDateTime(COLostream& Stream, const COLdateTime& DateTime ) const { DBdatabase::streamDateTime(Stream, DateTime); }

// JULIAN-CHECK this appears to be legit overide (of createSqlCommandWithoutBinding from DBdatabase)
void DBdatabaseOdbc::createSqlCommandWithoutBinding( const DBsqlCommand& SqlCommand, COLostream& OutputSql ) const{
   COL_METHOD(DBdatabaseOdbc::createSqlCommandWithoutBinding);
   switch (SqlCommand.commandType()){
   case DB_SELECT_CMD:
      createSqlSelect( (DBsqlSelect&) SqlCommand, OutputSql );
      break;
   case DB_SELECT_UNION_CMD:
      createSqlSelectUnion( (DBsqlSelectUnion&) SqlCommand, OutputSql );
      break;
   case DB_UPDATE_CMD:
      DBdatabaseOdbcCreateSqlUpdateWithoutBinding(*this, (DBsqlUpdate&) SqlCommand, OutputSql);
      break;
   case DB_INSERT_CMD:
      DBdatabaseOdbcCreateSqlInsertWithoutBinding(*this, (DBsqlInsert&) SqlCommand, OutputSql);
      break;
   case DB_DELETE_CMD:
      createSqlDelete( (DBsqlDelete&) SqlCommand, OutputSql );
      break;
   case DB_DROP_CMD:
      createSqlDropTable( (DBsqlDropTable&) SqlCommand, OutputSql );
      break;
   case DB_CREATE_TABLE_CMD:
      createSqlCreateTable( (DBsqlCreateTable&) SqlCommand, OutputSql );
      break;
   default:
      throw COLerror("Specified SQL command not supported.", COLerror::PreCondition );
   }
}

// Bleah - this is inefficient...!
COLstring DBdatabaseOdbcRecreateSqlQuery(const DBdatabaseOdbc& Database, const DBsqlInsert& SqlCommand, const COLstring& SqlString){
   COL_FUNCTION(DBdatabaseOdbcRecreateSqlQuery);
   if(!Database.useBinding()){
      return SqlString;
   }
   COLstring SqlQuery = SqlString;
   int C = SqlCommand.countOfColumn();
   for (int i = 0; i < C; i++){
      COLstring ColumnValue;
      COLostream Stream(ColumnValue);
      const DBvariant& VariantValue = SqlCommand.columnValue(i);
      if(VariantValue.dataType() == DB_DATA_TYPE_NOT_DEFINED){
         continue;
      }
      if ( VariantValue.dataType() == DB_STRING) {
         Database.addEscapedStringToStream(Stream, VariantValue.toString());
      }else if ( VariantValue.dataType() == DB_DATETIME ){
         Database.streamDateTime( Stream, VariantValue.dateTime() );
      }else{
         ColumnValue = VariantValue.toString();
      }
      COLindex Position = SqlQuery.find("?");
      if (Position != npos){
         SqlQuery.remove(Position, 1);
         SqlQuery.insert(Position, ColumnValue);
      }
   }
   return SqlQuery;
}

COLstring DBdatabaseOdbcRecreateSqlQuery( const DBdatabaseOdbc& Database, const DBsqlUpdate& SqlCommand, const COLstring& SqlString){
   COL_FUNCTION(DBdatabaseOdbcRecreateSqlQuery);
   if(!Database.useBinding()) {
      return SqlString;
   }
   COLstring SqlQuery = SqlString;
   for (int i = 0; i < SqlCommand.countOfColumn(); i++){
      COLstring ColumnValue;
      COLostream Stream(ColumnValue);
      const DBvariant& VariantValue = SqlCommand.columnValue(i);
      if(VariantValue.dataType() == DB_DATA_TYPE_NOT_DEFINED){
         continue;
      }
      if (VariantValue.dataType() == DB_STRING){
         Database.addEscapedStringToStream(Stream, VariantValue.toString());
      }else if ( VariantValue.dataType() == DB_DATETIME){
         Database.streamDateTime( Stream, VariantValue.dateTime() );
      }else{
         ColumnValue = VariantValue.toString();
      }
      COLindex Position = SqlQuery.find("?");
      if (Position != npos){
         SqlQuery.remove(Position, 1);
         SqlQuery.insert(Position, ColumnValue);
      }
   }
   return SqlQuery;
}

static const COLstring OdbcBasePingCommand("SELECT 0;");
const COLstring& DBdatabaseOdbc::sqlPingCommand() const { return OdbcBasePingCommand; }

void DBdatabaseOdbc::setTimeout(COLuint32 Seconds) { pMember->setTimeout(Seconds); }
COLuint32 DBdatabaseOdbc::timeout() const { return pMember->TimeoutInSeconds; }

void DBdatabaseOdbc::setParameter(const COLstring& Name, const COLstring& Value){
   COL_METHOD(DBdatabaseOdbc::setParameter); 
   COL_VAR2(Name, Value);
   if(Name == "UseUnicode") {
      if(Value == "yes") {
         pMember->UseUnicode = true;
         return;
      }
      else if(Value == "no" || Value.is_null()) {
         pMember->UseUnicode = false;
         return;
      }
   }
   if(Name == "UseBinding") {
      if(Value == "yes") {
         pMember->UseBinding = true;
         return;
      } else if(Value == "no" || Value.is_null()) {
         pMember->UseBinding = false;
         return;
      }
   }
   if(Name == DB_PARAM_TIMEOUT) {
      // FIXME quick and dirty - so that we can do this via params
      int Timeout = ::atoi(Value.c_str());
      pMember->setTimeout(Timeout);
      return;      
   }
   // otherwise (invalid values and all other cases)
   // this will fall through to the base implementation,
   // which will throw an "unsupported" message
   DBdatabase::setParameter(Name, Value);
}

bool DBdatabaseOdbc::useBinding() const { return !pMember->UseUnicode && pMember->UseBinding; }

bool DBdatabaseOdbc::useWideChar() const {
#ifdef _WIN32
   return pMember->UseUnicode;
#else
   return false; // no WideChar usage on Posix - UTF8 is the way to go
#endif
}

bool DBdatabaseOdbc::useUnicode() const { return pMember->UseUnicode; }

static const COLstring DBodbcNVarChar  = "NVARCHAR";
static const COLstring DBodbcNVarChar2 = "NVARCHAR2"; // for Oracle
static const COLstring DBodbcUniVarChar = "UNIVARCHAR"; // for Sybase ASE

const COLstring& DBdatabaseOdbc::dbStringType() const{
   COL_METHOD(DBdatabaseOdbc::dbStringType);
   if(useUnicode()){
      if(databaseType() == DB_SQL_SERVER) {
         return DBodbcNVarChar;
      }
      if(databaseType() == DB_ODBC_ORACLE) {
         return DBodbcNVarChar2;
      }
      if(databaseType() == DB_ODBC_SYBASE_ASE) {
         // return DBodbcUniVarChar;
         // not doing this - using regular varchar -
         // these types inflate storage requirements beyond reason,
         // while not doing a lot to improve the Unicode handling.
         // Sybase ASE seems to have some key size limitations too
      }
   }
   return DBdatabase::dbStringType();
}

bool DBdatabaseOdbc::supportBinaryType() const{
   return pMember->SupportBinaryTypes;
}

void DBdatabaseOdbc::setSupportBinaryType( bool Tran /*= false*/ ) {
   pMember->setSupportBinaryTypes (Tran);
}

COLerror createErrorObject(SQLHANDLE Handle)
{
   COLstring NativeErrorCode;
   COLstring ErrorMessage = DBodbcErrorMessage(SQL_HANDLE_DBC, Handle, &NativeErrorCode);
   return COLerror(ErrorMessage, atoi(NativeErrorCode.c_str()));
}
