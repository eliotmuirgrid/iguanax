//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBSQLdatabaseSqlite
//
// Description:
//
// MySql database implementation class. 
//
// Author: YunqiZhang
// Date:   November 27, 2003 
//
//---------------------------------------------------------------------------
#include "DBSQLprecomp.h"
#pragma hdrstop

#include "DBSQLdatabaseSqlite.h"

#include <DB/DBresultSetPtr.h>
#include <DB/DBsqlCreateTable.h>
#include <DB/DBsqlCreateTableColumn.h>
#include <DB/DBresultSetRow.h>
#include <DB/DBvariant.h>
#include <DB/DBescape.h>
#include <DB/DBsqlInsert.h>
#include <DB/DBsqlUpdate.h>
#include <DB/DButils.h>

#include <COL/COLdateTime.h>
#include <COL/COLarray.h>
#include <COL/COLlist.h>
#include <COL/COLostream.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <COL/COLlog.h>
#include <stdio.h>
#include <ctype.h>

#include <SQL/SQLstatementIterator.h>
#include <SQL/SQLpreparedStatement.h>
#include <SQLTE/sqlite3.h>

COL_LOG_MODULE;

#define DB_SQLITE_DATE_STRFTIME_FORMAT "%Y-%m-%dT%H:%M:%S"
#define DB_SQLITE_DATE_SSCANF_FORMAT "%04d-%02d-%02dT%02d:%02d:%02d"

DBSQLdatabaseSqlite::DBSQLdatabaseSqlite()
   :m_BinarySupported(false)
{
   COL_METHOD(DBSQLdatabaseSqlite);
}

DBSQLdatabaseSqlite::~DBSQLdatabaseSqlite()
{
   COL_METHOD(~DBSQLdatabaseSqlite);
   try
   {
      disconnect();
   }
   catch(const COLerror& Error)
   {
      COL_TRC(Error);
   }
}

void DBSQLdatabaseSqlite::connect(const char* pDataSourceName, const char* pUserName, const char* pPassword) {
   COL_METHOD(connect);
   connect(pDataSourceName, pUserName, pPassword, "");
}

void DBSQLdatabaseSqlite::connect(const char* pDataSourceName, const char* pUserName, const char* pPassword, const char* pEncryptionKey) {
   COL_METHOD(connect);
   COL_VAR4(pDataSourceName, pUserName, pPassword, pEncryptionKey);

   if (isConnected() && cachedDataSourceName() == pDataSourceName) {
      // already connected and with the same parameters - do nothing
      COL_TRC("already connected");
      return;
   };
   // reconnect if not connected, or if the connection parameters don't match previous ones

   disconnect();

   m_Sqlite.open(pDataSourceName, pEncryptionKey);

   bool DatabaseIsValid = m_Sqlite.isValid();
   COL_VAR(DatabaseIsValid);
   
   if (!DatabaseIsValid) {
      disconnect();
      COLstring ErrorMsg;
      COLostream(ErrorMsg) << "Could not properly connect to sqlite database: '" << pDataSourceName << "'." << newline
                           << "The database file is either invalid, is encrypted and a valid passphrase was not supplied, or "
                           << "is unencrypted and a passphrase was incorrectly given.";
      throw COLerror(ErrorMsg);
   }

   setCachedDataSourceName( pDataSourceName );
}

void DBSQLdatabaseSqlite::disconnect()
{
   COL_METHOD(disconnect);
   DBdatabase::endTransaction();
   m_Sqlite.close();
}

void DBSQLdatabaseSqlite::streamCreateTableNotNull
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


static const COLstring DBstringType("TEXT");
const COLstring& DBSQLdatabaseSqlite::dbStringType() const{return DBstringType;}
const COLstring& DBSQLdatabaseSqlite::dbTextType() const{return DBstringType;}

static const COLstring DBintType("INT4");
const COLstring& DBSQLdatabaseSqlite::dbIntegerType() const{return DBintType;}

static const COLstring DBdoubleType("DOUBLE");
static const COLstring DBdateType("TEXT"); //map to text, iso8601. see streamDate
const COLstring& DBSQLdatabaseSqlite::dbDoubleType() const{return DBdoubleType;}
const COLstring& DBSQLdatabaseSqlite::dbDateTimeType() const{return DBdateType;}

static const COLstring DBblobType("BLOB");
const COLstring& DBSQLdatabaseSqlite::dbBinaryType() const{return DBblobType;}

DBdataType DBSQLdatabaseSqlite::sqliteTypeToDbType(int Type) const{
   switch ( Type )
   {
   case SQLITE_INTEGER:
      return DB_LARGE_INTEGER;
      break;
   case SQLITE_NULL:
      return DB_DATA_TYPE_NOT_DEFINED; // this is how DB represents NULL
      break;
   case SQLITE_FLOAT:
      return DB_LARGE_DOUBLE;
      break;
   case SQLITE_TEXT:
      return DB_STRING;
      break;
   case SQLITE_BLOB:
      //depends binary support
      if (m_BinarySupported){
         return DB_BINARY;
      }
      break;
      default:
         COL_ERROR_STREAM( COL_T("SQLlite data type: ") << Type << newline << COL_T("not supported."), COLerror::PreCondition );
   }
   return DB_DATA_TYPE_NOT_DEFINED;
}

DBresultSetPtr DBSQLdatabaseSqlite::executeSqlString
( 
   const COLstring& SqlString,
   COLuint32 ResultSetStart,
   COLuint32 ResultSetSize,
   COLarray<DBresultSetPtr> *pAllResultSets
) const
{
   COL_METHOD(executeSqlString);
   COL_VAR3(ResultSetStart, ResultSetSize, SqlString);

   SQLpreparedStatement Statement;
   Statement.prepare(m_Sqlite,SqlString.c_str());

   DBresultSetPtr pResultSet = new DBresultSet();
   COLuint32 CountOfResultSetColumn = Statement.countOfColumn();
   pResultSet->resizeColumnVector(CountOfResultSetColumn);

   COLint64 RowIndexLast = COL_INT_64_MAX;   
   if (ResultSetSize != 0) {
      RowIndexLast = ResultSetStart;
      RowIndexLast += ResultSetSize;
   } 

   COLint64 RowIndex = 0;
   bool ColumnsDefined = false;
   for (SQLstatementIterator Iter(Statement);Iter.stepNext();RowIndex++) {

      // Figure out the column type definitions based on the data we find. Visit
      // enough rows until we are sure we have non-NULL defs for each column (if
      // the column has any data at all.)
      // TODO: isn't there a way to query the actual SQLite column metadata
      // directly from the schema?
      if (!ColumnsDefined) {
         ColumnsDefined = true;
         for (COLuint32 ColIndex = 0; ColIndex < CountOfResultSetColumn; ColIndex++) {
            if (pResultSet->columnType(ColIndex) == DB_DATA_TYPE_NOT_DEFINED) {
               DBdataType DataType = sqliteTypeToDbType(Iter.columnType(ColIndex));
               
               // If the current type is still undefined, make sure visit another
               // row to be sure.
               if (DataType == DB_DATA_TYPE_NOT_DEFINED) {
                  ColumnsDefined = false;
               }

               // Save the type we found, even if it is undefined.
               pResultSet->setColumn(ColIndex, Statement.columnName(ColIndex), DataType);
            }
         }
      }

      //check range
      //TODO - sqlite may have a way to skip in the result set
      if (RowIndex < ResultSetStart) continue;
      if (RowIndex >= RowIndexLast) break;
      
      DBresultSetRow& NewRow = pResultSet->addRow();
      for (COLuint32 ColIndex = 0; ColIndex < CountOfResultSetColumn; ColIndex++){
         DBdataType ColType = pResultSet->columnType(ColIndex);
         DBvariant Variant;
         switch(ColType){
         case DB_LARGE_INTEGER:
            Variant.integer64() = Iter.integer64Value(ColIndex);
            break;
         case DB_LARGE_DOUBLE:
            Variant.float64() = Iter.doubleValue(ColIndex);
            break;
         case DB_STRING:
            {
               const char* pStr = 0;
               int StrLen = Iter.textValue(ColIndex,&pStr);
               Variant.string().write(pStr,StrLen);
            }
            break;
         case DB_BINARY:
            {
               const void* pData = 0;
               int BlobLen = Iter.blobValue(ColIndex,&pData);
               Variant.binary().write(pData,BlobLen);
            }
            break;
         case DB_DATA_TYPE_NOT_DEFINED:
            break; // do nothing; Variant will already be this type.
         default:
            COL_ERROR_STREAM( COL_T("Unexpected DB variant type: ") << DBvariantTypeName(ColType) << '.', COLerror::PreCondition );
         }
         NewRow.setColumnValue(ColIndex,Variant);
      }
   }

   if (pResultSet->countOfColumn() == 0){
      //this is to be consistent with other db drivers
      pResultSet = 0;
   }

   return pResultSet;
}

DBresultSetPtr DBSQLdatabaseSqlite::executeSqlInsertOrUpdate( const COLstring& SqlStr, const DBsqlInsert& SqlInsertCommand ) const
{
   try{
      int RC= SqlInsertCommand.countOfRows();
      for (int Row = 0; Row < RC; ++Row) {
         COLlist<COLstring> BoundDateTimes;
         SQLpreparedStatement Prepared;
         Prepared.prepare(m_Sqlite,SqlStr.c_str());

         for (unsigned int Col=0; Col < SqlInsertCommand.countOfColumn(); Col++ ){
            const DBvariant& Value = SqlInsertCommand.columnValue(Col,Row);
            int AdjCol = Col+1;
            switch(Value.dataType()){
            case DB_DATA_TYPE_NOT_DEFINED:
               Prepared.bindNullValue(AdjCol);
            break;
            case DB_STRING:
            case DB_TEXT:
               {
                  const COLstring& StrValue = Value.string();
                  Prepared.bindTextValue(AdjCol,StrValue.c_str(),StrValue.size());
               }
               break;
            case DB_INTEGER:
               Prepared.bindIntegerValue(AdjCol,Value.integer32());
               break;
            case DB_DOUBLE:
               Prepared.bindDoubleValue(AdjCol,Value.float32());
               break;
            case DB_DATETIME:
               //TODO - implicit conversion to TEXT here
               {
                  COLstring DateTimeStr = Value.dateTime().format(DB_SQLITE_DATE_STRFTIME_FORMAT);
                  const COLstring& Str = BoundDateTimes[ BoundDateTimes.add( DateTimeStr )];
                  Prepared.bindTextValue(AdjCol,Str.c_str(),Str.size());
               }
               break;
            case DB_LARGE_INTEGER:
               Prepared.bindInteger64Value(AdjCol,Value.integer64());
               break;
            case DB_LARGE_DOUBLE:
               Prepared.bindDoubleValue(AdjCol,Value.float64());
               break;
            case DB_BOOLEAN:
               //TODO - implicit conversion to integer here
               Prepared.bindIntegerValue(AdjCol,(Value.boolean() ? 1 : 0));
               break;
            case DB_BINARY:
               {
                  const COLstring& Data = Value.binary();
                  Prepared.bindBlobValue(AdjCol,Data.c_str(),Data.size());
               }
               break;
            default:
               COL_ERROR_STREAM("Unknown Type " << DBvariantTypeName(Value.dataType()),COLerror::PreCondition);
            }
         }

         SQLstatementIterator Iter(Prepared);
         Iter.stepNext();
      }
   }
   catch(const COLerror& Error){
      COLstring ParamList;
      COLostream Stream(ParamList);
      Stream << "Bound Parameters:" << newline;      
      for(COLuint32 RowIndex = 0; RowIndex < SqlInsertCommand.countOfRows(); RowIndex++){
         if (SqlInsertCommand.countOfRows() > 1){
            Stream << "   Row #" << RowIndex+1 << newline; 
         }
         for (unsigned int Col=0; Col < SqlInsertCommand.countOfColumn(); Col++ ){
            const DBvariant& Value = SqlInsertCommand.columnValue(Col,RowIndex);
            Stream << "   " << SqlInsertCommand.columnName(Col) 
                   << " (?" << Col+1 << ", " << DBvariantTypeName(Value.dataType()) << ") : " 
                   << DBvariantDebugString(Value) << newline;
         }
         if (RowIndex+1 < SqlInsertCommand.countOfRows() && RowIndex>0){
            Stream << newline;
         }
      }

      COLerror NewError(Error);
      COLstring NewErrorStr(Error.description());
      COLostream NewStream(NewErrorStr);
      NewStream << newline << ParamList;
      NewError.setDescription(NewErrorStr);

      throw NewError;
   }
   return 0;
}

// JULIAN-CHECK this appears to be legit overide (of executeSqlInsert from DBdatabase)
DBresultSetPtr DBSQLdatabaseSqlite::executeSqlInsert( const DBsqlInsert& SqlInsertCommand ) const{
   COLstring InsertStr;
   COLostream SqlStream(InsertStr);
   createSqlInsert(SqlInsertCommand,SqlStream);
   return executeSqlInsertOrUpdate(InsertStr,SqlInsertCommand);
}

DBresultSetPtr DBSQLdatabaseSqlite::executeSqlUpdate( const DBsqlUpdate& SqlUpdateCommand ) const{
   COLstring InsertStr;
   COLostream SqlStream(InsertStr);
   createSqlUpdate(SqlUpdateCommand,SqlStream);
   return executeSqlInsertOrUpdate(InsertStr,SqlUpdateCommand);
}

void DBSQLdatabaseSqlite::streamInsertUpdateValue
(
 COLostream& Stream, 
 const DBsqlInsert& SqlCommand, 
 COLuint32 ColumnIndex
) const
{
   Stream << '?' << (ColumnIndex+1);
}

void DBSQLdatabaseSqlite::streamDateTime( COLostream& Stream, const COLdateTime& DateTime ) const
{
   if ( !DateTime.isNull() ) Stream << DateTime.format("'" DB_SQLITE_DATE_STRFTIME_FORMAT "'");
   else{
      streamNullValue(Stream);
   }
}

COLdateTime DBSQLdateTimeFromStr(const COLstring& DateStrFromDb){
   //this is not the converse operation to printOn
   int nYear=0, nMonth=0, nDay=0, nHour=0, nMin=0, nSec=0;
   int ReturnCode = sscanf(DateStrFromDb.c_str(), DB_SQLITE_DATE_SSCANF_FORMAT,          
      &nYear,
      &nMonth,
      &nDay, 
      &nHour,
      &nMin,
      &nSec);
   COLdateTime Out;
   if (6 == ReturnCode)
   {
      Out.setDateTime( nYear, nMonth, nDay, nHour, nMin, nSec );
   }
   return Out;
}

DBresultSetPtr DBSQLdatabaseSqlite::fetchDatabaseTables() const
{
   const COLstring SelectTables("SELECT name from sqlite_master where type='table'");
   return executeSqlString(SelectTables);
}

DBresultSetPtr DBSQLdatabaseSqlite::fetchNonSystemDatabaseTables() const
{
   //sqlite system tables aren't listed in sqlite_master anyway
   return fetchDatabaseTables();
}

/* We are replicating the functionality of
** the sqlite private function sqlite3AffinityType
** in order to go from the type declared in a table, 
** to the affinity, which we then convert to the DBdataType
**
** Substring     | Affinity
** --------------------------------
** 'INT'         | SQLITE_AFF_INTEGER
** 'CHAR'        | SQLITE_AFF_TEXT
** 'CLOB'        | SQLITE_AFF_TEXT
** 'TEXT'        | SQLITE_AFF_TEXT
** 'BLOB'        | SQLITE_AFF_NONE
** 'REAL'        | SQLITE_AFF_REAL
** 'FLOA'        | SQLITE_AFF_REAL
** 'DOUB'        | SQLITE_AFF_REAL
*/
static DBdataType DBsqliteAffinityFromType(const COLstring& iType, bool BinarySupported){
   COLstring Type(iType);
   Type.toLowerCase();
   if (Type.find("int") != npos) return DB_LARGE_INTEGER;
   else if (Type.find("char") != npos ||
            Type.find("clob") != npos ||
            Type.find("text") != npos){
      return DB_STRING;
   }
   else if (Type.find("blob") != npos) {
      //TODO - kinda dumb
      return (BinarySupported ? DB_BINARY : DB_STRING);
   }
   else if (Type.find("real") != npos ||
            Type.find("floa") != npos ||
            Type.find("doub") != npos){
      return DB_LARGE_DOUBLE;
   }
   return DB_DATA_TYPE_NOT_DEFINED;
}

void DBSQLdatabaseSqlite::streamBinary( COLostream& Stream, const COLstring& Buffer ) const
{
   if ( Buffer.size() != 0 )
   {
      Stream << COL_T("'x");
      DBbinaryToHex(*Stream.sink(),Buffer.c_str(),Buffer.size());
      Stream << '\'';
   }
   else
   {
      streamNullValue( Stream );
   }
}

DBresultSetPtr DBSQLdatabaseSqlite::fetchDatabaseTableColumns( const char* TableName ) const
{
   COLPRECONDITION( TableName != NULL );
   COLPRECONDITION(m_Sqlite.sqlite() != 0);
   COLstring PragmaStr;
   COLostream PragmaStream(PragmaStr);
   PragmaStream << COL_T("PRAGMA table_info(");
   addEscapedStringToStream(PragmaStream,TableName);
   PragmaStream << ')';

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

   SQLpreparedStatement Statement;
   Statement.prepare(m_Sqlite,PragmaStr.c_str());
   const int Sqlite_name_col = 1;
   const int Sqlite_type_col = 2;
   const int Sqlite_not_null_col = 3;
   const int Sqlite_default_col = 4;
   const int Sqlite_pk_col = 5;

   for (SQLstatementIterator Iter(Statement); Iter.stepNext();){
      DBresultSetRow& Row = pResultSet->addRow();
      ColumnIndex = 0;
      const char* pStr = 0;
      int StrLen = 0;

      // Column: "Column_Name"
      {
         DBvariant Var;
         StrLen = Iter.textValue(Sqlite_name_col,&pStr);
         Var.string().write(pStr,StrLen);
         Row.setColumnValue(ColumnIndex++,Var);
      }

      //Column: Data_Type
      DBdataType BaseType = DB_DATA_TYPE_NOT_DEFINED;
      {
         pStr = 0;
         StrLen = Iter.textValue(Sqlite_type_col,&pStr);
         COLstring TypeStr(pStr,StrLen);
         BaseType = DBsqliteAffinityFromType(TypeStr,m_BinarySupported);
         Row.setColumnValue(ColumnIndex++,DBvariant((COLint32)BaseType));       
      }

      // Column: "Default_Value"
      {
         DBvariant DefaultValue;
         switch(BaseType){
         case DB_LARGE_INTEGER:
            DefaultValue.integer64() = Iter.integer64Value(Sqlite_default_col);
            break;
         case DB_LARGE_DOUBLE:
            DefaultValue.float64() = Iter.doubleValue(Sqlite_default_col);
            break;
         case DB_BINARY:
            {
               const void* pData = 0;
               int DataLen = Iter.blobValue(Sqlite_default_col,&pData);
               DefaultValue.binary().write(pData,DataLen);
            }
            break;
         case DB_STRING:
            //nobreak
         default://assume text
            {
               pStr = 0;
               StrLen = Iter.textValue(Sqlite_default_col,&pStr);
               DefaultValue.string().write(pStr,StrLen);
            }
         }
         Row.setColumnValue(ColumnIndex++,DefaultValue);
      }

      // Column: "Length" -- TODO what should we use here?
      Row.setColumnValue(ColumnIndex++, DBvariant((COLint64)0));

      // Column: "Required"
      {
         bool NotNulVal = Iter.integerValue(Sqlite_not_null_col) != 0;
         Row.setColumnValue(ColumnIndex++,DBvariant(NotNulVal));
      }

      // Column: "Primary_Key"
      {
         bool PkVal = Iter.integerValue(Sqlite_pk_col) != 0;
         Row.setColumnValue(ColumnIndex++,DBvariant(PkVal));
      }
   }

   return pResultSet;
}

DBresultSetPtr DBSQLdatabaseSqlite::fetchDatabases()
{
   //we don't know, so return an empty set
   return new DBresultSet();
}

bool  DBSQLdatabaseSqlite::isSystemTable( const char* pTableName ) const
{
   COLstring TableName = pTableName;
   TableName.toLowerCase();
   if (TableName.find("sqlite_") == 0) return true;
   return false;
}

COLostream& DBSQLdatabaseSqlite::addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const{
   char* pEscaped = ::sqlite3_mprintf("'%q'",UnescapedString.c_str());
   if (!pEscaped)
      COL_ERROR_STREAM("Out of memory in DBSQLdatabaseSqlite::addEscapedStringToStream", COLerror::PreCondition);

   try {  
      Stream.write(pEscaped,strlen(pEscaped));
      ::sqlite3_free(pEscaped);
      pEscaped = 0;
   } catch (...) {
      ::sqlite3_free(pEscaped);
      pEscaped = 0;
      throw;
   }

   return Stream;
}

void DBSQLdatabaseSqlite::setAutocommitFlag( bool Flag ) const 
{
   //#18641 - We use an exclusive transaction here to avoid db locked errors
   //this is because most things that use the DB library don't tweak the transaction type
   if (!Flag) executeSqlString("BEGIN EXCLUSIVE");
}

void DBSQLdatabaseSqlite::commitTransaction()
{
   try {
      m_Sqlite.executeSqlString("COMMIT");
      DBdatabase::endTransaction();
   } catch (...) {
      DBdatabase::endTransaction();
      throw;
   }
}

void DBSQLdatabaseSqlite::rollbackTransaction()
{
   try {
      m_Sqlite.executeSqlString("ROLLBACK");
      DBdatabase::endTransaction();
   } catch(...) {
      DBdatabase::endTransaction();
      throw;
   }
}

bool DBSQLdatabaseSqlite::isConnected()
{
   return m_Sqlite.sqlite() != 0;
}

COLostream& DBSQLdatabaseSqlite::streamName( COLostream& Stream, const COLstring& Name, bool QuoteName ) const
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

void DBSQLdatabaseSqlite::setParameter(const COLstring& Name, const COLstring& Value)
{
   if(Name == "UseUnicode" || Name == "UseBinding") {
      //we accept yes or no here, because really
      //it should make no difference.
      return;
   }
   DBdatabase::setParameter(Name, Value);
}

bool DBSQLdatabaseSqlite::useUnicode() const
{
   return false;
}

class DBSQLsqlTokenizer{
public:
   DBSQLsqlTokenizer(const COLstring& In) : Input(In){
      pIn = Input.c_str();
   }

   COLstring next(){
      COLstring Out;
      COLostream Stream(Out);
      while(*pIn){
         if (*pIn == '?'){
            if (Out.size()){
               //return first
               break;
            }
            //scan for first non digit
            Stream << '?';
            pIn++;
            while(*pIn && isdigit(*pIn)){
               Stream << *pIn;
               pIn++;
            }
            break;
         }
         else{
            Stream << *pIn;
            pIn++;
         }
      }
      return Out;
   } 

private:
   COLstring Input;
   const char* pIn;
};

// JULIAN-CHECK this appears to be legit overide (of recreateSqlInsertWithoutBinding from DBdatabase)
void DBSQLdatabaseSqlite::recreateSqlInsertWithoutBinding( const DBsqlInsert& SqlCommand, const COLstring& SqlString, COLostream& Output) const{
   DBSQLsqlTokenizer Tokener(SqlString);
   COLint32 ParamIndex = 0;
   for(;;){
      COLstring Token = Tokener.next();
      if (Token.size()){
         if (Token[0] == '?'){
            COLstring ColumnValue;
            {
               COLostream Stream(ColumnValue);
               const DBvariant& VariantValue = SqlCommand.columnValue(ParamIndex);
               if ( VariantValue.dataType() == DB_STRING) {
                  addEscapedStringToStream(Stream, VariantValue.toString());
               }else if ( VariantValue.dataType() == DB_DATETIME ){
                  streamDateTime( Stream, VariantValue.dateTime() );
               }else{
                  ColumnValue = DBvariantDebugString(VariantValue);
               }
            }
            Output << ColumnValue;
            ParamIndex++;
         }
         else{
            Output << Token;
         }
      }
      else break;
   }
}

// JULIAN-CHECK this appears to be legit overide (of createSqlCommandWithoutBinding from DBdatabase)
void DBSQLdatabaseSqlite::createSqlCommandWithoutBinding( const DBsqlCommand& SqlCommand, COLostream& OutputSql ) const{
   COLstring SqlString;
   {
      COLostream StrStream(SqlString);
      createSqlCommand(SqlCommand, StrStream);
   }
   switch (SqlCommand.commandType()){
   case DB_DROP_CMD:
   case DB_CREATE_TABLE_CMD:
   case DB_SELECT_CMD:
   case DB_SELECT_UNION_CMD:
   case DB_DELETE_CMD:
      OutputSql << SqlString;
      break;
   case DB_UPDATE_CMD:
   case DB_INSERT_CMD:
      recreateSqlInsertWithoutBinding(*dynamic_cast<const DBsqlInsert*>(&SqlCommand),SqlString,OutputSql);
   break;
   default:
      throw COLerror("Specified SQL command not supported.", COLerror::PreCondition );
   }
}

DBdatabase* DBSQLcreateSqlite(){
   return new DBSQLdatabaseSqlite();
}

COLstring DBSQLresolve(const COLstring& DataSource, const COLstring& Location){
   COL_FUNCTION(DBSQLresolve);
   COLstring ResolvedDataSource = FILexpand(DataSource);
   if (!FILpathIsAbsolute(ResolvedDataSource)){
      COL_TRC("Have a relative path to the sqlite file");
      ResolvedDataSource = FILpathResolve(DataSource, Location);
   }
   COL_TRC("Resolved absolute path to sqlite file:");
   COL_VAR(ResolvedDataSource);
   return ResolvedDataSource;
}