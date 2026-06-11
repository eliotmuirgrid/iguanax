#ifndef __DB_TRANSACTION_H__
#define __DB_TRANSACTION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBtransaction
//
// Description:  
//
// Simple class for doing a transaction - works along the same principles as
// COLlocker.  i.e. a RA II helper class.
//
// Author: Henry Tran
// Date:   April 13, 2004 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class DBdatabase;
class DBdatabasePtr;

void DBbeginTransaction(DBdatabase* pDatabase);

class DBtransaction
{
public:
   DBtransaction(DBdatabase* pDatabase);
   DBtransaction(DBdatabase& Database);
   ~DBtransaction();

   void commit();
   void rollback();

private:
   DBdatabase* pDatabase;
   bool CommitFlag;

   void beginTransaction();

   DBtransaction( DBtransaction& Orig ); // not allowed
   DBtransaction& operator=( DBtransaction& Orig ); // not allowed
};

#endif // end of defensive include

