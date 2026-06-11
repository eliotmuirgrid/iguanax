#ifndef __DB_DATABASE_FACTORY_H__
#define __DB_DATABASE_FACTORY_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseFactory
//
// Description:
//
// This is the factory class to construct and return a pointer to the
// correct database object. Note that the factory needs to know which API
// that the database uses. The API is added to this factory by passing its 
// unique string identifier and its static createFunction() to the 
// addDatabase() function. The correct database object is constructed by
// passing the API string identifier to the getDatabase() function.
//
// Since this class only has a static factory functions with non-static members,
// no objects of this class should be constructed.
//
// Author: Henry Tran
// Date:   November 27, 2003 
//
//---------------------------------------------------------------------------
#include <COL/COLarray.h>
class DBdatabasePtr;
class DBdatabaseFactoryPrivate;
class DBdatabase;

typedef DBdatabase* (*DBcreateDb)();

// TODO - could we eliminate the need for this object and just have a couple of functions?

class DBdatabaseFactory
{
public:
   DBdatabaseFactory();
   ~DBdatabaseFactory();
   
   //set this static callback in main() or some other entry point 
   //so that implementations outside this module can be loaded in
   //e.g. DBSQLdatabaseSqlite
   //note that a section is being held, so be careful
   static void (*onInit)(DBdatabaseFactory& Factory);

   //these should not be called manually
   //use the RAII object DBdatabaseFactoryThreadInit
   //
   // Performs thread initialization for currently loaded databases.
   // Called when threads are started (before db calls are made)
   // and when the threads die.
   //
   void initializeForThread();
   void uninitializeForThread();//no throw   

   void initialize(COLostream& LogStream);
   void addDatabase(DBcreateDb Creator, const char* pName);

   bool isInitialized() const;

   DBdatabasePtr getDatabase( const COLstring& DatabaseAPI );
   bool databaseSupported( const COLstring& DatabaseAPI );
   COLarray<COLstring> listDatabases();
   const COLstring& databaseName( COLuint32 DatabaseIndex );

   COLuint32 countOfDatabase();  
private:
   DBdatabaseFactoryPrivate* pMember;

   DBdatabaseFactory( DBdatabaseFactory& Orig ); // not allowed
   DBdatabaseFactory& operator=( DBdatabaseFactory& Orig ); // not allowed
};

#endif // end of defensive include
