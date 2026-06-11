#ifndef __DB_DATABASE_MARIA_DB_H__
#define __DB_DATABASE_MARIA_DB_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseMariaDb
//
// Description:
//
// This class handles calls to the MariaDb native C API. This class links the
// API with the common interface provided by the DBdatabase parent class.
//
//
// Author: Julian Muir
// Date:   April 26 2021 
//
//---------------------------------------------------------------------------

// JULIAN NO LONGER NEEDED - this is replaced by Nic's driver modules = DBdriverMariaDb.h

// JULIAN this can safely be deleted as a group of 4 files
//        1 - main2/DB/DBlibImportMariaDb.cpp (this file)
//        2 - main2/DB/DBlibImportMariaDb.h   (header for this file)
//        3 - main2/DB/DBdatabaseMariaDb.cpp  (includes DBlibImportMariaDb.h)
//        4 - main2/DB/DBdatabaseMariaDb.h


#include "DBdatabase.h"

#include <IDB/IDBdatabaseConfig.h>
#include <DB/DBlibImportMariaDb.h>

class DBmariaDbDll;
class DBlibImportMariaDb;

// JULIAN just a skeleton for now

// JULIAN copied the My#SQL class (closest to MariaDb) and renamed
//        because I need the loadConfig method

class DBdatabaseMariaDb : public DBdatabase
{
public:
   DBdatabaseMariaDb(DBdatabaseType Type);
   ~DBdatabaseMariaDb();

   // JULIAN no longer used (delete soon - probably delete whole file...)
   // static bool loadConfig(COLostream& LogStream, const COLstring& FileName);

   // initialize() attempts to load the MariaDB shared library.  If a MariaDB library has
   // already been loaded, it will do nothing and return true.  Otherwise it will
   // return true only if it is able to load a supported library as follows:
   //
   //   * If LibraryPath is specified, it will attempt to load that library.
   //
   //   * If LibraryPath is not specified, it will search the registry (in Windows) and
   //     then the PATH.  If multiple versions are found in the registry, the preferred
   //     version will be loaded.  If the PATH is searched, the first supported library
   //     found is used.
   //
   static bool initialize(COLostream& LogStream, const char* LibraryPath = NULL);

   // reinitialize() will attempt to load a new MariaDB shared library, replacing the one
   // loaded by initialize() if applicable.  It must be used to load a new MariaDB shared
   // library after initialize() is called, but calling initialize() beforehand is not
   // required--it will behave exactly like initialize() in this case.
   //
   // If a new library is successfully loaded, it will return true.  If not, the originally
   // loaded library will continue to be used (if there is one), and it will return false.
   //
   // reinitialize() cannot be used to unload all MariaDB shared libraries, and will always
   // leave DBdatabaseMariaDb in a sane state, even if an exception is thrown.  Also,
   // it is safe to reinitialize() to the same library that is already loaded.
   //
   // *** We no longer support loading multiple MariaDB libraries (#10153).  reinitialize()
   // *** will fail unless no MariaDB library is loaded.  If readyToReinitialize() is false,
   // *** reinitialize() will always fail, even if you supply a valid library.
   //
   static bool readyToReinitialize();
   static bool reinitialize(COLostream& LogStream, const char* LibraryPath = NULL);
   

   // IMPLEMENTATION ONLY
   // This is used to initialize MariaDB's thread local storage
   // no-op if the driver isn't loaded

   // JULIAN not needed 
   // static void initializeForThread();
   // static void uninitializeForThread();

   static bool isInitialized();
   static const COLstring& loadedApiLibraryPath();
   static const COLstring& loadedApiVersion();
   static bool isLibrarySupported(const COLstring&, int* pDllLoadErrorCode = 0);
   static bool isValidLibraryPath(const COLstring& LibraryPath);

   // Initializes any required parameters/settings and connects the 
   // DBdatabase object to the DBMS.
   void connect( const char* pDataSourceName,
                 const char* pUserName,
                 const char* pPassword );

   DBdatabaseType databaseType() const { return m_Type; }
   
   const COLstring& apiLibraryPath() const;
   const COLstring& apiVersion() const;

   // Deallocates all open database handles and disconnects the 
   // DBdatabase object from the DBMS.
   void disconnect();

   // The following function is defined in the parent class DBdatabase:
   // Discovers the type of SQL command at run time and calls one of the
   // specific SQL command execution methods below.
   //
   // virtual DBresultSetPtr executeSqlCommand( const DBsqlCommand& SqlCommand ) const;

   // Returns if the database object is connected. This can be used to test
   // the connection before issuing further statements to the object.
   // Note that databases may go down, which may result in databases disconnecting on
   // exceptions, and not on just calls to disconnect().
   bool isConnected();

   // Submits a SQL command string to the DBMS. All of the above executeSql...
   // methods call this method.
   //
   // NOTE: Input strings are assumed to be unescaped as this function will
   // escape any special SQL characters for you.
   DBresultSetPtr executeSqlString( const COLstring& SqlString,
                                    COLuint32 ResultSetStart = 0,
                                    COLuint32 ResultSetSize = 0,
                                    COLarray<DBresultSetPtr> *pAllResultSets = NULL) const;

   // Stores a list of database table names into a DBresultSet object.
   // Access this list through the resultSet() function above.
   DBresultSetPtr fetchDatabaseTables() const;

   // Stores a list of non-system database table names into a DBresultSet object.
   DBresultSetPtr fetchNonSystemDatabaseTables() const;

   // Stores column information of a specified database table into a DBresultSet
   // object. Parameters such as column name, data type (in terms of 
   // DBdataType), default value (if any), length, required column
   // flag, and primary key flag are stored as separate result set columns 
   // (the result set column names denote which result set column corresponds 
   // to which parameter).
   // 
   // Access this information through the resultSet() function above.
   DBresultSetPtr fetchDatabaseTableColumns( const char* TableName ) const;

   // Stores names of databases on the host into a DBresultSet object.
   // Precondition: A connection to the host must already exist.
   DBresultSetPtr fetchDatabases();

   // Returns true if the table with the specified table name is a system
   // table, false otherwise.
   bool isSystemTable( const char* TableName ) const;

   void setAutocommitFlag( bool Flag ) const;
   void commitTransaction();
   void rollbackTransaction();

   virtual COLostream& addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const;

   bool useUnicode() const;

   void setParameter(const COLstring& Name, const COLstring& Value);

   // Overrides DBdatabase's binary-type support check.
   // JULIAN-CHECK m_BinaryOkay definitely not required as BLOB is supported in all DB now
   bool supportBinaryType() const { return m_BinaryOkay; }
   void setSupportBinaryType(bool Okay){ m_BinaryOkay = Okay; }

private:

   const COLstring& selectCastStringType() const;

   COLostream& streamName( COLostream& Stream, const COLstring& Name, bool QuoteName ) const;

   void streamCreateTableNotNull( COLostream& Stream, 
                                          const DBsqlCreateTable& SqlCreateTableCommand, 
                                          COLuint32 ColumnIndex ) const;

   void streamCreateTableForeignKeyPrefix( COLostream& Stream, 
                                                   const DBsqlCreateTable& SqlCreateTableCommand, 
                                                   COLuint32 ColumnIndex ) const;

   void streamCreateTableSuffix( COLostream& Stream, const DBsqlCreateTable& SqlCreateTableCommand ) const;

   //#5699 Making DB_TEXT on MariaDb -> LONGTEXT
   const COLstring& dbTextType() const; //override DBdatabase

   DBdatabaseType m_Type; //Needed to tell whether MySQL or MariaDB. 

//   DBdatabaseMariaDbPrivate* pMember; // JULIAN
   DBlibImportMariaDb* pMember;
   DBmariaDbDll&       MariaDbDll;
   // JULIAN-CHECK m_BinaryOkay definitely not required as BLOB is supported in all DB now
   bool                m_BinaryOkay;


   DBdatabaseMariaDb( DBdatabaseMariaDb& Orig ); // not allowed
   DBdatabaseMariaDb& operator=( DBdatabaseMariaDb& Orig ); // not allowed
};


#endif // end of defensive include