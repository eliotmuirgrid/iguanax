#ifndef __SQL_TRANSACTION_H__
#define __SQL_TRANSACTION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SQLtransaction
//
// Description:
//
// RAII helper class to create a SQLite transaction.
//
// Author:   Henry Tran, Yuriy Kachanov
// Date:     September 2005
//
//---------------------------------------------------------------------------

class SQLsqlite;

class SQLtransaction{
public:
   SQLtransaction( SQLsqlite& iSqlite, const char* pType = "BEGIN");
   virtual ~SQLtransaction();
   void commit();
private:
   SQLsqlite& m_Sqlite;
   bool m_Committed;

   SQLtransaction(const SQLtransaction& Orig);  
   SQLtransaction& operator=(const SQLtransaction& Orig);
};

// other processes can read the database but can not write to
class SQLimmediateTransaction : public SQLtransaction{
public:
   SQLimmediateTransaction( SQLsqlite& Sqlite );
   virtual ~SQLimmediateTransaction() {}
private:
   SQLimmediateTransaction(const SQLimmediateTransaction&);
   SQLimmediateTransaction& operator=(const SQLimmediateTransaction&);
};

// other processes can not read and write the database
class SQLexclusiveTransaction : public SQLtransaction{
public:
   SQLexclusiveTransaction( SQLsqlite& Sqlite );
   virtual ~SQLexclusiveTransaction() {}
private:
   SQLexclusiveTransaction(const SQLexclusiveTransaction&);
   SQLexclusiveTransaction& operator=(const SQLexclusiveTransaction&);
};

#endif // end of defensive include
