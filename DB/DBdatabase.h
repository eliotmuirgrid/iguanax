#ifndef __DB_DATABASE_H__
#define __DB_DATABASE_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabase
//
// Description:
//
// Abstract base class for the supported database API's. This class 
// provides a common interface to all supported database management
// systems (DBMS) and their associated API's.
//
// Author: Henry Tran
// Date:   November 27, 2003 
//
//
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// JULIAN-CHECK went through file and commented out things that are not used
// and commented on other things also
//
// NOTE: I put 4 slashes "////" in front of each commented out function so 
//       it is easy to see them - and still easy to uncomment press ctrl+/ 2x
//
// Search for "JULIAN-CHECK" to find comments
/////////////////////////////////////////////////////////////////////////////

#include "DBdatabaseType.h"
#include <COL/COLarray.h>

class COLdateTime;
class DBresultSetPtr;
class DBresultSetSequence;
class DBsqlCommand;
class DBsqlCreateTable;
class DBsqlDelete;
class DBsqlDropTable;
class DBsqlInsert;
class DBsqlSelect;
class DBsqlSelectJoin;
class DBsqlSelectOrderBy;
class DBsqlSelectUnion;
class DBsqlUpdate;
class DBsqlWhere;
class DBsqlWhereCondition;
class DBvariant;

class DBsqlCommandSequence;

// Overloaded operator for streaming output; outputs entire database
COLostream& operator<<(COLostream& Stream, DBresultSetPtr& ResultSet);
// For streaming output; outputs database given flexible parameters
// Can output specific rows (row offset and row count, and # of chars to show for each entry) 
COLostream&  DBprintResultSetPtrToStream(COLostream& Stream, DBresultSetPtr& ResultSet, const COLuint32 RowOffset, COLuint32 CountOfRows, const COLuint32 MaxSizeOfDBEntry);

const char* DBdatabaseNameFromAlias(const COLstring& InputName);


class DBdatabase : public COLrefCounted
{
public:
   DBdatabase();
   virtual ~DBdatabase();

   static const char* const DB_DATABASE_MY_SQL_NAME;
   static const char* const DB_DATABASE_SQL_SERVER_NAME;
   
   static const char* const DB_DATABASE_OCI_ORACLE_NAME;
   static const char* const DB_DATABASE_OCI_ORACLE_LEGACY_NAME;  // The old Oracle 9i name

   static const char* const DB_DATABASE_SQLITE_NAME;

   static const char* const DB_DATABASE_ODBC_DB2_NAME;
   static const char* const DB_DATABASE_ODBC_FILEMAKER_NAME;
   static const char* const DB_DATABASE_ODBC_INFORMIX_NAME;
   static const char* const DB_DATABASE_ODBC_INTERBASE_NAME;
   static const char* const DB_DATABASE_ODBC_ORACLE_NAME;
   static const char* const DB_DATABASE_ODBC_ACCESS_NAME;
   static const char* const DB_DATABASE_ODBC_POSTGRESQL_NAME;
   static const char* const DB_DATABASE_ODBC_MYSQL_NAME; 
   static const char* const DB_DATABASE_ODBC_SYBASE_NAME;
   static const char* const DB_DATABASE_ODBC_SYBASE_ASE_NAME;

   static const char* const DB_DATABASE_MARIA_DB_NAME;

   // A null terminated array of the above.
   static struct DBdatabaseNames {
      const char* Name;
   } const AllDatabaseNames[];

  
   // Initializes any required parameters/settings and connects the 
   // DBdatabase object to the DBMS.
   virtual void connect(const char* DataSourceName, const char* UserName, const char* Password) = 0;
   virtual void connect(const char* DataSourceName, const char* UserName, const char* Password, const char* EncryptionKey) {
      connect(DataSourceName, UserName, Password);
   }

   virtual DBdatabaseType databaseType() const=0;
   const char* databaseName() const;
  
   // Deallocates all open database handles and disconnects the 
   // DBdatabase object from the DBMS.
   virtual void disconnect() = 0;

   // Returns if the database object is connected. This can be used to test
   // the connection before issuing further statements to the object.
   // Note that databases may go down, which may result in databases disconnecting on
   // exceptions, and not on just calls to disconnect().
   virtual bool isConnected() = 0;

   // Discovers the type of SQL command at run time and calls one of the
   // specific SQL command execution methods below.
   virtual DBresultSetPtr executeSqlCommand( const DBsqlCommand& SqlCommand, const COLuint32& ResultSetStart = 0, const COLuint32& ResultSetSize = 0 ) const;   

   // Executes a sequence of sql commands. A new DBsqlResultSequence is returned
   // Note that this is NOT TRANSACTIONAL. The commands are executed one after another, 
   // and an exception is raised from the first command to fail, and the returned vector
   // will contain up to the last executed command
   //
   // LIMIT (ResultSetSTart/Size is not supported yet).
   // JULIAN-CHECK executeSqlCommandSequence functions never used - commented out here and DB/DBdatabaseMySql.cpp
   // commented out = no compile errors
   //// void executeSqlCommandSequence(const DBsqlCommandSequence& SqlCommandSequence, DBresultSetSequence& ResultSetSequence) const;
                                   
   // Creates a sql string.
   // JULIAN-CHECK this is used by createSqlCommandWithoutBinding below
   void createSqlCommand(const DBsqlCommand& SqlCommand, COLostream& OutputSql) const;    
   
   // Creates an sql command without binding
   // JULIAN-CHECK this is used in MANY places
   virtual void createSqlCommandWithoutBinding(const DBsqlCommand& SqlCommand, COLostream& OutputSql) const;

   // Traverses the specific SQL command object to generate a
   // SQL command string that is compatible with the DBMS.
   //
   // Constructs the sql statements that are used by executeXXX calls
   // JULIAN-CHECK these funtions are used in various places:
   // - main2/DDB/DDBinsertTable.h/cpp - legit+legacy = create tables imported by Chameleon so can
   //   be removed if Chameleon is not supported in IguanaX - but the use case seems to remain
   //   valid for creating tables DERIVED from VMD files
   // JULIAN-CHECK these 7 funtions are used in DBdatabase::createSqlCommand
   virtual void createSqlCreateTable(const DBsqlCreateTable& SqlCreateTableCommand, COLostream& OutputSql) const;
   virtual void createSqlDelete(const DBsqlDelete& SqlDeleteCommand, COLostream& OutputSql) const;
   virtual void createSqlDropTable(const DBsqlDropTable& SqlDropCommand, COLostream& OutputSql) const;
   virtual void createSqlInsert(const DBsqlInsert& SqlInsertCommand, COLostream& OutputSql) const;
   virtual void createSqlSelect(const DBsqlSelect& SqlSelectCommand, COLostream& OutputSql) const;
   virtual void createSqlSelectUnion( const DBsqlSelectUnion& SqlUnion, COLostream& OutputSql) const;
   virtual void createSqlUpdate( const DBsqlUpdate& SqlUpdateCommand, COLostream& OutputSql  ) const;

   // returns the sql statement terminator for this database (eg ';');
   virtual const COLstring& sqlStatementTerminator() const;

   //return true if the db supports grouping the Lhs and Rhs of a union with 
   //parenthesis, e.g. (SELECT ...) UNION (SELECT...)
   virtual bool selectUnionGroupSelects() const{return true;}
   virtual bool selectUnionSeparateOrderBySupported() const;


   virtual const COLstring& dbKeyModifiers() const;
   virtual const COLstring& dbStringMaxKeyLength() const;
   virtual const COLstring& dbStringType() const;
   virtual const COLstring& dbIntegerType() const;
   virtual const COLstring& dbDoubleType() const;
   virtual const COLstring& dbDateTimeType() const;
   virtual const COLstring& dbTextType() const;
   virtual const COLstring& dbBinaryType() const;

   // Utilizes the
   // executeSqlString() method below to submit the generated SQL command
   // string to the DBMS.    
   //
   // SELECT queries automatically reset (empty) and then populate a 
   // DBresultSet object that may be accessed using the resultSet() function 
   // below. The other SQL commands do not affect this DBresultSet object.  
   // JULIAN-CHECK these 7 funtions are used in DBdatabase::executeSqlCommand
   virtual DBresultSetPtr executeSqlCreateTable( const DBsqlCreateTable& SqlCreateTableCommand ) const;
   virtual DBresultSetPtr executeSqlDelete( const DBsqlDelete& SqlDeleteCommand ) const;
   virtual DBresultSetPtr executeSqlDropTable( const DBsqlDropTable& SqlDropCommand ) const;
   virtual DBresultSetPtr executeSqlInsert( const DBsqlInsert& SqlInsertCommand ) const;
   virtual DBresultSetPtr executeSqlSelect( const DBsqlSelect& SqlSelectCommand, const COLuint32& ResultSetStart = 0, const COLuint32& ResultSetSize = 0 ) const;
   virtual DBresultSetPtr executeSqlSelectUnion( const DBsqlSelectUnion& SqlUnion, COLuint32 ResultSetStart = 0, COLuint32 ResultSetSize = 0 ) const;
   virtual DBresultSetPtr executeSqlUpdate( const DBsqlUpdate& SqlUpdateCommand ) const;

   // Submits a SQL command string to the DBMS. All of the above executeSql...
   // methods call this method.
   //
   // NOTE: Input strings are assumed to be unescaped as this function will
   // escape any special SQL characters for you.
   virtual DBresultSetPtr executeSqlString(const COLstring& SqlString, COLuint32 ResultSetStart = 0, COLuint32 ResultSetSize = 0, COLarray<DBresultSetPtr> *pAllResultSets = NULL) const = 0;

   // Stores a list of database table names into a DBresultSet object.
   // Access this list through the resultSet() function above.
   virtual DBresultSetPtr fetchDatabaseTables() const = 0;

   // Stores a list of non-system database table names into a DBresultSet object.
   // JULIAN-CHECK fetchNonSystemDatabaseTables functions never used - commented out here and DB/DBdatabaseMySql.cpp
   // commented out = no compile errors
   // virtual DBresultSetPtr fetchNonSystemDatabaseTables() const = 0;

   // Stores column information of a specified database table into a DBresultSet
   // object. Parameters such as column name, data type (in terms of 
   // DBdataType), default value (if any), length, required column
   // flag, and primary key flag are stored as separate result set columns 
   // (the result set column names denote which result set column corresponds 
   // to which parameter).
   // 
   // Access this information through the resultSet() function above.
   // JULIAN-CHECK fetchDatabaseTableColumns functions never used - commented out here and DB/DBdatabaseMySql.cpp
   // commented out = no compile errors
   // virtual DBresultSetPtr fetchDatabaseTableColumns( const char* TableName ) const = 0;

   // Returns true if the table with the specified table name is a system
   // table, false otherwise.
   // JULIAN-CHECK isSystemTable functions never used - commented out here and DB/DBdatabaseMySql.cpp
   // commented out = no compile errors
   // virtual bool isSystemTable( const char* TableName ) const = 0;

   virtual COLostream& addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const=0;

   // Transaction functions
   void beginTransaction();
   bool transactionActive() const;
   virtual void commitTransaction() = 0;
   virtual void rollbackTransaction() = 0;

   void setCachedAutoCommitFlag(bool NewValue);
   void setCachedDataSourceName( const COLstring& DataSourceName );
   const COLstring& cachedDataSourceName() const;
   void setCachedUserName( const COLstring& UserName );
   const COLstring& cachedUserName() const;
   void setCachedPassword( const COLstring& Password );
   const COLstring& cachedPassword() const;

   // sets the timeout for operations
   // If the connection is not connected then the setting is effective
   // after the next successful connection attempt.
   // 
   // If the connection is connected, then the setting takes effect
   // immediately.
   //
   // The default timeout value is 5 minutes.
   // 
   virtual void setTimeout(COLuint32 Seconds){};
   // virtual COLuint32 timeout() const{return -1;};

   virtual void setParameter(const COLstring& Name, const COLstring& Value);

   //TODO probably rename
   virtual bool useUnicode() const = 0;

   COLstring dateTimeToString(const COLdateTime& DateTime);

   // TODO the functions here should probably just access a private member variable also defined in this class to check so they don't need overrides in each db class
   // Since binary datatype was recently added, it should be opted-in
   virtual bool supportBinaryType() const{ return false; }
   virtual void setSupportBinaryType(bool Tran = false){ /*no-op*/ }

protected:
   virtual void setAutocommitFlag(bool Flag) const = 0;

   // this is a function for child classes to call in their implementations
   // of commitTransaction, rollbackTransaction, and disconnect
   void endTransaction();

   virtual bool selectCastSupported() const;
   virtual const COLstring& selectCastStringType() const;

   virtual COLostream& streamName( COLostream& Stream, const COLstring& Name, bool QuoteName ) const;

   virtual bool createTableDefaultConstraintSupported() const;
   virtual void streamCreateTableNotNull( COLostream& Stream,const DBsqlCreateTable& SqlCreateTableCommand, COLuint32 ColumnIndex ) const;
   // Sybase ASE requires "NOT NUL" after "DEFAULT", so use this function for Sybase
   // for others databases do nothing
   virtual void streamCreateTableNotNull2( COLostream& Stream, const DBsqlCreateTable& SqlCreateTableCommand, COLuint32 ColumnIndex ) const;
   virtual void streamCreateTableForeignKeyPrefix( COLostream& Stream, const DBsqlCreateTable& SqlCreateTableCommand, COLuint32 ColumnIndex ) const;
   virtual void streamCreateTableForeignKeySuffix( COLostream& Stream, const DBsqlCreateTable& SqlCreateTableCommand, COLuint32 ColumnIndex ) const;
   virtual void streamCreateTableSuffix( COLostream& Stream, const DBsqlCreateTable& SqlCreateTableCommand ) const;

   virtual void streamInsertUpdateValue( COLostream& Stream, const DBsqlInsert& SqlCommand, COLuint32 ColumnIndex ) const;

   virtual void streamDateTime( COLostream& Stream, const COLdateTime& DateTime ) const;
   virtual void streamBinary( COLostream& Stream, const COLstring& Buffer ) const;

   virtual void streamNullValue( COLostream& Stream ) const;

   virtual void streamOrderByColumn( COLostream& Stream, const DBsqlSelectOrderBy& OrderByColumn, bool FirstOrderByColumn ) const;


   // Generates a condition value string to be inserted into a WHERE condition.
   // Basically, it places no quotes around column names, single quotes around 
   // strings, converts COLdateTime into a format compatible with MySQL's 
   // date time type, and it converts the other DBvariant types into an unquoted 
   // string.
   COLostream& addVariantToStream( COLostream& Stream, const DBvariant& VariantValue, bool IsColumnFlag, bool QuoteColumnName ) const;

   // Generates a WHERE clause string from a DBsqlWhere object, for insertion
   // into a generated SQL command string. This function implements recursion 
   // so that sub-expressions may be generated.
   COLostream& addWhereClauseToStream( COLostream& Stream, const DBsqlWhere& WhereClause ) const;

   // Generates a WHERE condition string from a DBsqlWhereCondition object,
   // for insertion into a generated WHERE clause string. The purpose of this 
   // function is to maintain readability of the whereClauseString() code.
   COLostream& addWhereConditionToStream( COLostream& Stream, const DBsqlWhereCondition& WhereCondition ) const;

   COLostream& addJoinClauseToStream( COLostream& Stream, const DBsqlSelectJoin& JoinClause ) const;

   // Generates a SELECT command string from a DBsqlSelect object,
   // for insertion into a generated WHERE condition string (as an operand
   // for "IN_SELECT" conditions) or into an INSERT command. The executeSQLSelect() 
   // also utilizes this function to prevent redundant code.
   COLostream& addSelectCommandToStream( COLostream& Stream, const DBsqlSelect& SqlSelectCommand, bool DoNotStreamOrderBy = false ) const;

private:
   COLstring m_CachedDataSourceName;
   COLstring m_CachedUserName;
   COLstring m_CachedPassword;
   bool m_CachedAutoCommitFlag;

   virtual bool supportsDefinePrimaryKey() const;
   virtual bool supportsDefault() const;
   DBdatabase( DBdatabase& Orig ); // not allowed
   DBdatabase& operator=( DBdatabase& Orig ); // not allowed
};

COLstring DBtranslateShortNameToFactory(const COLstring& InputName);

void DBshowShortNames(COLostream& Stream);

bool DBdatabaseSupportsParam(const COLstring& DbApi, const COLstring& ParamName);

extern const COLstring DB_PARAM_USE_UNICODE;
extern const COLstring DB_PARAM_TIMEOUT;

#endif // end of defensive include
