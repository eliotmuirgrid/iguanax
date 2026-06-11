#ifndef __DB_DATABASE_OCI_ORACLE_H__
#define __DB_DATABASE_OCI_ORACLE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOciOracle
//
// Description:
//
// This class handles calls to Oracle through the native OCI API. This class 
// links the API with the common interface provided by the DBdatabase parent 
// class.
//
// Author: Henry Tran
// Date:   May 2, 2004 
//
//---------------------------------------------------------------------------

#include "DBdatabase.h"
#include "DBescape.h"

template<class T> class COLarray;


// DBfindSupportedOracleDlls() searches for installed versions of Oracle on
// the system, and returns a list of supported shared libraries in decreasing
// order or preference.  Presently only the Windows registry is searched,
// so outside of Windows, it will always return an empty list.
//
COLarray<COLstring> DBfindSupportedOracleDlls(bool IgnoreCache = false);

enum DBdatabaseOciOracleLoadResult
{
   DB_OCI_LOAD_OK = 1,
   DB_OCI_LOAD_INVALID_LIBRARY = -1, // The file was not a valid DLL or shared object - i.e. a text file
   DB_OCI_LOAD_NOT_EXIST = -2, // The file did not exist
   DB_OCI_LOAD_NO_PERMISSIONS = -3, //  We did not have permissions to load it
   DB_OCI_LOAD_UNKNOWN_ERROR = -4
};


class DBdatabaseOciOraclePrivate;

class DBdatabaseOciOracle : public DBdatabase
{
public:
   DBdatabaseOciOracle();
   virtual ~DBdatabaseOciOracle();

   void setParameter(const COLstring& Name, const COLstring& Value);
   
   static DBdatabaseOciOracleLoadResult libraryLoadHasError(const COLstring& LibraryPath);
   static bool isLibrarySupported(const COLstring&);
   static bool isValidLibraryPath(const COLstring& LibraryPath);

   static bool initialize(COLostream& LogStream, const char* LibraryPath = NULL);

   // Unlike MySQL, we cannot have more than one version of Oracle's client library
   // loaded at the same time, so no instances can exist when we reinitialize.
   //
   static bool readyToReinitialize();
   static bool reinitialize(COLostream& LogStream, const char* LibraryPath = NULL);

   static bool isInitialized();
   static const COLstring& loadedApiLibraryPath();
   static const COLstring& loadedApiVersion();

   virtual DBdatabaseType databaseType() const { return DB_OCI_ORACLE; }

   const COLstring& apiLibraryPath() const { return loadedApiLibraryPath(); }
   const COLstring& apiVersion() const     { return loadedApiVersion();     }

   // Initializes any required parameters/settings and connects the 
   // DBdatabase object to the DBMS.
   virtual void connect( const char* pDataSourceName,
                         const char* pUserName,
                         const char* pPassword );

   /**
    * Deallocates all open database handles and disconnects the 
    * DBdatabase object from the DBMS.
    */
   virtual void disconnect();
   
   /**
    * Returns if the database object is connected. This can be used to test
    * the connection before issuing further statements to the object.
    * Note that databases may go down, which may result in databases disconnecting on
    * exceptions, and not on just calls to disconnect().
    */
   virtual bool isConnected();

   virtual DBresultSetPtr executeSqlInsert( const DBsqlInsert& SqlInsertCommand ) const;
   virtual DBresultSetPtr executeSqlUpdate( const DBsqlUpdate& SqlUpdateCommand ) const;

   virtual COLostream& addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const;

   /**
    * Submits a SQL command string to the DBMS. All of the above executeSql...
    * methods call this method.
    *
    * NOTE: Input strings are assumed to be unescaped as this function will
    * escape any special SQL characters for you.
    */
   virtual DBresultSetPtr executeSqlString( const COLstring& SqlString,
                                            COLuint32 ResultSetStart = 0,
                                            COLuint32 ResultSetSize = 0,
                                            COLarray<DBresultSetPtr> *pAllResultSets = NULL) const;

   // Since OCI uses parameter binding, this function needs to be overriden.
   virtual void createSqlCommandWithoutBinding( const DBsqlCommand& SqlCommand, COLostream& OutputSql ) const;

   // Stores a list of database table names into a DBresultSet object.
   // Access this list through the resultSet() function above.
   virtual DBresultSetPtr fetchDatabaseTables() const;

   // Stores a list of non-system database table names into a DBresultSet object.
   virtual DBresultSetPtr fetchNonSystemDatabaseTables() const;

   // Stores column information of a specified database table into a DBresultSet
   // object. Parameters such as column name, data type (in terms of 
   // DBdataType), default value (if any), length, required column
   // flag, and primary key flag are stored as separate result set columns 
   // (the result set column names denote which result set column corresponds 
   // to which parameter).
   // 
   // Access this information through the resultSet() function above.
   virtual DBresultSetPtr fetchDatabaseTableColumns( const char* TableName ) const;

   // Returns true if the table with the specified table name is a system
   // table, false otherwise.
   virtual bool isSystemTable( const char* TableName ) const;

   virtual void setAutocommitFlag( bool Flag ) const;
   virtual void commitTransaction();
   virtual void rollbackTransaction();

   virtual const COLstring& dbDoubleType() const;
   virtual const COLstring& dbDateTimeType() const;
   virtual const COLstring& dbTextType() const;

   bool useUnicode() const;

   virtual void streamDateTime( COLostream& Stream, const COLdateTime& DateTime ) const;

   const COLstring& dbStringType() const;

   // Overrides DBdatabase's binary-type support check.
   bool supportBinaryType() const { return m_BinaryOkay; }
   void setSupportBinaryType(bool Okay){ m_BinaryOkay = Okay; }

private:
   virtual void streamCreateTableForeignKeySuffix( COLostream& Stream, 
                                                   const DBsqlCreateTable& SqlCreateTableCommand,
                                                   COLuint32 ColumnIndex ) const;

   virtual void streamInsertUpdateValue( COLostream& Stream, 
                                         const DBsqlInsert& SqlCommand, 
                                         COLuint32 ColumnIndex ) const;
   
   virtual void streamNullValue( COLostream& Stream ) const;

   DBdatabaseOciOraclePrivate* pMember;
   DBescapeFunction m_pEscapeFunction;
   bool m_BinaryOkay;

   DBdatabaseOciOracle( DBdatabaseOciOracle& Orig ); // not allowed
   DBdatabaseOciOracle& operator=( DBdatabaseOciOracle& Orig ); // not allowed
};

#endif // end of defensive include
