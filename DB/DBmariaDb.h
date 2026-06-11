#ifndef __DB_MY_SQL_H__
#define __DB_MY_SQL_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBmariaDb
//
// Description:
//
// Implementation
//
//
// Author: Julian Muir
// Date:   May 03 2021 
//
//---------------------------------------------------------------------------

class COLvar;

#include "DBdatabase.h"
#include "DBdriverMariaDb.h"

class DBresultSetPtr;
class DBresultSetRow;
class DBmariaDbDll;       // JULIAN is this needed??? equiv in MySQL not used yet???

// MARIADB* DBmariaDbConnect(DBdriverMariaDb* pDriver, const COLvar* pConnectionInfo);

class DBmariaDb { //: public DBdatabase {
public:
   DBmariaDb();
   ~DBmariaDb();

   //Set the dynamic client library this instance of DBmySql will use
   bool setDriver(const COLstring& LibraryPath);

   // Initializes any required parameters/settings and connects the DBdatabase object to the DBMS.
   // TODO Shouldn't this return a bool or something to let code know whether it worked?
   void connect(const COLvar& ConnectionInfo);

   // Do we need this and is this the best way to do it?
   // DBdatabaseType databaseType() const { return m_ComponentType; }
   
   // TODO shift these into the DBmySqlDriver, so then DBmySqlInstance.m_Driver.apiLibraryPath();
   //const COLstring& apiLibraryPath() const;
   //const COLstring& apiVersion() const;

   // Deallocates all open database handles and disconnects the DBdatabase object from the DBMS.
   void disconnect();

   // Returns if the database object is connected. This can be used to test
   // the connection before issuing further statements to the object.
   // Note that databases may go down, which may result in databases disconnecting on
   // exceptions, and not on just calls to disconnect().
   // TODO calls mysql_ping which by default does not attempt reconnection
   // TODO reconnect logic should be seperate from hasConnectionByGuid()
   bool isConnected();

   // TODO write a reconnect implementation
   // If connection gone try to connect again, once? three times? Configurable times? Exponential backoff between tries?
   // void setReconnectParams(int Tries, int Delay); //if not set use a default(3, expBack), if 0 == Delay then use exponential backoff
   // if 0 == Tries, don't retry
   // bool reconnect();

   // The following function is defined in the parent class DBdatabase:
   // Discovers the type of SQL command at run time and calls one of the
   // specific SQL command execution methods below.
   // virtual DBresultSetPtr executeSqlCommand( const DBsqlCommand& SqlCommand ) const;

   // Submits a SQL command string to the DBMS. All of the above executeSql... methods call this method.
   // NOTE: Input strings are assumed to be unescaped as this function will escape any special SQL characters for you.
   DBresultSetPtr executeSqlString( const COLstring& SqlString, COLuint32 ResultSetStart = 0, COLuint32 ResultSetSize = 0, COLarray<DBresultSetPtr> *pAllResultSets = NULL);

   // // Stores a list of database table names into a DBresultSet object. Access this list through the resultSet() function above.
   // DBresultSetPtr fetchDatabaseTables() const;

   // // Stores a list of non-system database table names into a DBresultSet object.
   // DBresultSetPtr fetchNonSystemDatabaseTables() const;

   // // Stores column information of a specified database table into a DBresultSet
   // // object. Parameters such as column name, data type (in terms of 
   // // DBdataType), default value (if any), length, required column
   // // flag, and primary key flag are stored as separate result set columns 
   // // (the result set column names denote which result set column corresponds 
   // // to which parameter).
   // // 
   // // Access this information through the resultSet() function above.
   // DBresultSetPtr fetchDatabaseTableColumns( const char* TableName ) const;

   // // Stores names of databases on the host into a DBresultSet object.
   // // Precondition: A connection to the host must already exist.
   // DBresultSetPtr fetchDatabases();

   // // Returns true if the table with the specified table name is a system
   // // table, false otherwise.
   // //TODO current implementation always returns false
   // bool isSystemTable( const char* TableName ) const;

   // //TODO this flag is used when performing a transaction
   // //void setAutocommitFlag( bool Flag ) const;
   // //TODO instead implement
   // bool beginTransaction();
   // bool endTransaction(); //Remove and move code in here into commitTransaction
   // void commitTransaction();
   // void rollbackTransaction();

   // virtual COLostream& addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const;

   // //TODO what problem are we solving?
   // bool useUnicode() const;

   // ODO sus catch all function that hides what it really does 
   // Also does different things in OCI and ODBC but is defined up in DBdatabase
   // void setParameter(const COLstring& Name, const COLstring& Value);

   // // TODO define these better in DBdatabase
   // // Overrides DBdatabase's binary-type support check.
   // JULIAN-CHECK m_BinaryOkay definitely not required as BLOB is supported in all DB now
   // bool supportBinaryType() const { return m_BinaryOkay; }
   // void setSupportBinaryType(bool Okay){ m_BinaryOkay = Okay; }

private:
   // const COLstring& selectCastStringType() const;

   // COLostream& streamName( COLostream& Stream, const COLstring& Name, bool QuoteName ) const;

   // //TODO are these actually helpful and are they the best way to do it?
   // //Adds the keywords NOT NULL to the Stream
   // void streamCreateTableNotNull( COLostream& Stream, const DBsqlCreateTable& SqlCreateTableCommand, COLuint32 ColumnIndex ) const;
   // //adds INDEX, in a spot
   // void streamCreateTableForeignKeyPrefix( COLostream& Stream, const DBsqlCreateTable& SqlCreateTableCommand, COLuint32 ColumnIndex ) const;
   // //adds enging as InnoDB and sets unicode if support is set
   // void streamCreateTableSuffix( COLostream& Stream, const DBsqlCreateTable& SqlCreateTableCommand ) const;

   // //#5699 Making DB_TEXT on MySql -> LONGTEXT
   // const COLstring& dbTextType() const; //override DBdatabase

   DBdriverMariaDb m_Driver;
   MARIADB* m_pMariaDbObject; //TODO might need to be public?
   // Make this a member of DBdatabase
   // JULIAN-CHECK m_BinaryOkay definitely not required as BLOB is supported in all DB now
   bool m_BinaryOkay;


   DBmariaDb( DBmariaDb& Orig ); // not allowed
   DBmariaDb& operator=( DBmariaDb& Orig ); // not allowed
};

#endif // end of defensive include