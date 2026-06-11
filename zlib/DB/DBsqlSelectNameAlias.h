#ifndef __DB_SQL_SELECT_NAME_ALIAS_H__
#define __DB_SQL_SELECT_NAME_ALIAS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlSelectNameAlias
//
// Description:
//
// A class to encapsulate information about a single column in a 
// SELECT database query. Each column consists of a name and an optional
// alias.
//
// Encapsulating each column's information will better orgranize the SELECT 
// command structure when it is serialized (i.e. all column information is 
// grouped together). However, when using the DB2 API, it is probably more 
// desirable to specify column information directly on the DBsqlSelect 
// object.
//
// Author: Henry Tran
// Date:   December 1, 2003 
//
//---------------------------------------------------------------------------

class DBsqlSelectNameAliasPrivate;

class DBsqlSelectNameAlias 
{
public:
   DBsqlSelectNameAlias();
   // POD - so default copy constructor and assignment operator is fine.
   virtual ~DBsqlSelectNameAlias(); // TODO - this probably doesn't need to be virtual

   void setName( const COLstring& Name );
   const COLstring& name() const;

   void setQuoteName( bool QuoteFlag );
   bool quoteName() const;

   void setAlias( const COLstring& Alias );
   const COLstring& alias() const;  
   
   void setQuoteAlias( bool QuoteFlag );
   bool quoteAlias() const;

private:
   COLstring Name;
   bool QuoteName;
   COLstring Alias;
   bool QuoteAlias;
};

#endif // end of defensive include
