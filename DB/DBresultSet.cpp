//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBresultSet
//
// Description:
//
// Implementation.
//
// Author: Henry Tran
// Date:   November 27, 2003 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBresultSet.h"
#include "DBresultSetRow.h"

#include <COL/COLarray.h>
#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


class DBresultSetPrivate
{
public:
   DBresultSetPrivate() 
   : ColumnLookupTable( COLhash<COLstring>::defaultHash ),
     CountOfDatabaseRow(0)
   {
      COL_METHOD(DBresultSetPrivate::DBresultSetPrivate);
   }

   ~DBresultSetPrivate() {}
   
   COLarray< COLstring > ColumnNameVector;
   COLarray< DBdataType > ColumnTypeVector;
   COLarray< DBresultSetRow > RowVector;
   COLuint64 CountOfDatabaseRow;

   /**
    * Synchronizes the 'ColumnNameIndexLookupTable' with the 'ColumnNameVector'.
    * This will ensure that the look-up table contains the correct names even 
    * after populating the 'ColumnNameVector' through de-serialization and not
    * through the API.
    */
   void synchronizeColumnLookup();

   /**
    * Used to look up a column index corresponding to a specified column name.
    * This look-up table facilitates the retrieval of a single value from the 
    * result set by row index and column name.
    */
   COLhashmap< COLstring, COLuint32 > ColumnLookupTable;  
};

void DBresultSetPrivate::synchronizeColumnLookup()
{
   COL_METHOD(DBresultSetPrivate::synchronizeColumnLookup);
   if ( ColumnLookupTable.size() == 0 )
   {
      for ( COLuint32 ColumnIndex = 0; ColumnIndex < ColumnNameVector.size(); ColumnIndex++ )
      {
         COLstring ColumnNameUpperCase = ColumnNameVector[ ColumnIndex ];
         ColumnNameUpperCase.toUpperCase();
         ColumnLookupTable.add( ColumnNameUpperCase, ColumnIndex );
      }
   }
}

DBresultSet::DBresultSet()
{
   COL_METHOD(DBresultSet::DBresultSet);
   pMember = new DBresultSetPrivate();
}

DBresultSet::~DBresultSet()
{
   delete pMember;
}

void DBresultSet::resizeColumnVector( COLuint32 CountOfColumn )
{
   COL_METHOD(DBresultSet::resizeColumnVector);
   if ( CountOfColumn < pMember->ColumnNameVector.size() )
   {
      // If the user resizes the columns to a smaller size than the current
      // number of columns, we need to re-synchronize the column lookup table
      // to ensure that it contains the correct column names.
      pMember->ColumnNameVector.resize( CountOfColumn );
      pMember->ColumnTypeVector.resize( CountOfColumn );
      pMember->ColumnLookupTable.clear();
      pMember->synchronizeColumnLookup();
   }
   else
   {
      pMember->ColumnNameVector.resize( CountOfColumn );
      pMember->ColumnTypeVector.resize( CountOfColumn );
   }
   
   for ( COLuint32 RowIndex = 0; RowIndex < pMember->RowVector.size(); RowIndex++ )
   {
      pMember->RowVector[ RowIndex ].resizeColumnValueVector( CountOfColumn );
   }
}

const COLuint32 DBresultSet::addColumn( const COLstring& ColumnName, DBdataType DataType )
{
   COL_METHOD(DBresultSet::addColumn);
   COLPRECONDITION( !ColumnName.is_null() );
   pMember->synchronizeColumnLookup();

   COLstring ColumnNameUpperCase = ColumnName;
   ColumnNameUpperCase.toUpperCase();

   // Add the column name to both the look-up table and the column name vector.
   pMember->ColumnLookupTable.add( ColumnNameUpperCase, pMember->ColumnNameVector.size() );
   pMember->ColumnNameVector.push_back( ColumnName);
   pMember->ColumnTypeVector.push_back( DataType);

   // Resize any existing row objects to accommodate the added column.
   for ( COLuint32 RowIndex = 0; RowIndex < pMember->RowVector.size(); RowIndex++ )
   {
      pMember->RowVector[ RowIndex ].resizeColumnValueVector( pMember->ColumnNameVector.size() );
   }
   
   return pMember->ColumnNameVector.size() - 1;
}

void DBresultSet::removeColumn( COLuint32 ColumnIndex )
{
   COL_METHOD(DBresultSet::removeColumn);
   COL_VAR(ColumnIndex);
   COLPRECONDITION( ColumnIndex < pMember->ColumnNameVector.size() );
   COLPRECONDITION( ColumnIndex < pMember->ColumnTypeVector.size() );
   pMember->ColumnLookupTable.removeKey( pMember->ColumnNameVector[ ColumnIndex ] );
   pMember->ColumnNameVector.remove( ColumnIndex );
   pMember->ColumnTypeVector.remove( ColumnIndex );
   
   for ( COLuint32 RowIndex = 0; RowIndex < pMember->RowVector.size(); RowIndex++ )
   {
      pMember->RowVector[ RowIndex ].removeColumnValue( ColumnIndex );
   }
}

void DBresultSet::removeColumn( const COLstring& ColumnName )
{
   COL_METHOD(DBresultSet::removeColumn);
   COL_VAR(ColumnName);
   COLPRECONDITION( !ColumnName.is_null() );
   pMember->synchronizeColumnLookup();

   COLstring ColumnNameUpperCase = ColumnName;
   ColumnNameUpperCase.toUpperCase();
   COLhashmapPlace ColumnLookupPlace = pMember->ColumnLookupTable.find( ColumnNameUpperCase );
   if ( ColumnLookupPlace == NULL )
   {
      COL_ERROR_STREAM( COL_T("Column name \"") << ColumnNameUpperCase << COL_T("\" not found in the result set table."), COLerror::PreCondition );
   }
   COLuint32 ColumnIndex = pMember->ColumnLookupTable[ColumnLookupPlace];
   pMember->ColumnNameVector.remove( ColumnIndex );
   pMember->ColumnTypeVector.remove( ColumnIndex );
   pMember->ColumnLookupTable.removeKey( ColumnNameUpperCase );
   
   for ( COLuint32 RowIndex = 0; RowIndex < pMember->RowVector.size(); RowIndex++ )
   {
      pMember->RowVector[ RowIndex ].removeColumnValue( ColumnIndex );
   }
}

void DBresultSet::setColumn( COLuint32 ColumnIndex, const COLstring& NewColumnName, DBdataType NewDataType )
{
   COL_METHOD(DBresultSet::setColumn);
   COL_VAR2(ColumnIndex, NewColumnName);
   COLPRECONDITION( !NewColumnName.is_null() );
   COLPRECONDITION( ColumnIndex < pMember->ColumnNameVector.size() );

   pMember->synchronizeColumnLookup();

   pMember->ColumnLookupTable.removeKey( pMember->ColumnNameVector[ ColumnIndex ] );

   pMember->ColumnNameVector[ ColumnIndex ] = NewColumnName;
   pMember->ColumnTypeVector[ ColumnIndex ] = NewDataType;

   COLstring NewColumnNameUpperCase = NewColumnName;
   NewColumnNameUpperCase.toUpperCase();
   pMember->ColumnLookupTable.add( NewColumnNameUpperCase, ColumnIndex ); 
   COL_TRC("Column set.");  
}

const COLstring& DBresultSet::columnName( COLuint32 ColumnIndex ) const
{
   COL_METHOD(DBresultSet::columnName);
   COL_VAR(ColumnIndex);
   COLPRECONDITION( ColumnIndex < pMember->ColumnNameVector.size() )
   return pMember->ColumnNameVector[ ColumnIndex ];
}

DBdataType DBresultSet::columnType( COLuint32 ColumnIndex ) const
{
   COL_METHOD(DBresultSet::columnType);
   COL_VAR(ColumnIndex);
   COLPRECONDITION( ColumnIndex < pMember->ColumnTypeVector.size() );
   return pMember->ColumnTypeVector[ ColumnIndex ];
}

const COLuint32 DBresultSet::countOfColumn() const
{
   COL_METHOD(DBresultSet::countOfColumn);
   return pMember->ColumnNameVector.size();
}

void DBresultSet::resizeRowVector( COLuint32 CountOfRow )
{
   COL_METHOD(DBresultSet::resizeRowVector);
   pMember->RowVector.resize( CountOfRow );
   COLuint32 CountOfColumn = pMember->ColumnNameVector.size();
   for ( COLuint32 RowIndex = 0; RowIndex < CountOfRow; RowIndex++ )
   {
      pMember->RowVector[ RowIndex ].resizeColumnValueVector( CountOfColumn );
   }
}

DBresultSetRow& DBresultSet::addRow()
{
   COL_METHOD(DBresultSet::addRow);
   DBresultSetRow& AddedRow = pMember->RowVector.push_back();
   AddedRow.resizeColumnValueVector( pMember->ColumnNameVector.size() );
   return AddedRow;
}

void DBresultSet::removeRow( COLuint32 RowIndex )
{
   COL_METHOD(DBresultSet::removeRow);
   COLPRECONDITION( RowIndex < pMember->RowVector.size() )
   pMember->RowVector.remove( RowIndex );
}

DBresultSetRow& DBresultSet::row( COLuint32 RowIndex ) const
{
   COL_METHOD(DBresultSet::row);
   COLPRECONDITION( RowIndex < pMember->RowVector.size() )
   return pMember->RowVector[ RowIndex ];
}

const COLuint32 DBresultSet::countOfRow() const
{
   COL_METHOD(DBresultSet::countOfRow);
   return pMember->RowVector.size();
}

void DBresultSet::setCountOfDatabaseRow( COLuint64 CountOfDatabaseRow )
{
   COL_METHOD(DBresultSet::setCountOfDatabaseRow);
   pMember->CountOfDatabaseRow = CountOfDatabaseRow;
}

COLuint64 DBresultSet::countOfDatabaseRow() const
{
   COL_METHOD(DBresultSet::countOfDatabaseRow);
   return pMember->CountOfDatabaseRow;
}

void DBresultSet::setValue
( 
   COLuint32 RowIndex, 
   COLuint32 ColumnIndex, 
   const DBvariant& Value 
) const
{
   COL_METHOD(DBresultSet::setValue);
   COL_VAR2(RowIndex, ColumnIndex);
   COLPRECONDITION( RowIndex < pMember->RowVector.size() )
   COLPRECONDITION( ColumnIndex < pMember->ColumnNameVector.size() )
   pMember->RowVector[ RowIndex ].setColumnValue( ColumnIndex, Value );
}

void DBresultSet::setValue
( 
   COLuint32 RowIndex, 
   const COLstring& ColumnName,
   const DBvariant& Value 
) const
{
   COL_METHOD(DBresultSet::setValue);
   COL_VAR2(RowIndex, ColumnName);
   COLPRECONDITION( !ColumnName.is_null() );
   COLPRECONDITION( RowIndex < pMember->RowVector.size() )

   pMember->synchronizeColumnLookup();

   COLstring ColumnNameUpperCase = ColumnName;
   ColumnNameUpperCase.toUpperCase();
   COLhashmapPlace ColumnLookupPlace = pMember->ColumnLookupTable.find( ColumnNameUpperCase );
   if ( ColumnLookupPlace == NULL )
   {
      COL_ERROR_STREAM( COL_T("Column name \"") << ColumnNameUpperCase << COL_T("\" not found in the result set table."), COLerror::PreCondition );
   }
   pMember->RowVector[ RowIndex ].setColumnValue( pMember->ColumnLookupTable[ColumnLookupPlace], Value );
}

const DBvariant& DBresultSet::value( COLuint32 RowIndex, COLuint32 ColumnIndex ) const
{
   COL_METHOD(DBresultSet::value);
   COL_VAR2(RowIndex, ColumnIndex);
   COLPRECONDITION( RowIndex < pMember->RowVector.size() )
   COLPRECONDITION( ColumnIndex < pMember->ColumnNameVector.size() )
   return pMember->RowVector[ RowIndex ].columnValue( ColumnIndex );
}

const DBvariant& DBresultSet::value( COLuint32 RowIndex, const COLstring& ColumnName ) const
{
   COL_METHOD(DBresultSet::value);
   COL_VAR2(RowIndex, ColumnName);
   COLPRECONDITION( !ColumnName.is_null() );
   COLPRECONDITION( RowIndex < pMember->RowVector.size() )
   
   pMember->synchronizeColumnLookup();

   COLstring ColumnNameUpperCase = ColumnName;
   ColumnNameUpperCase.toUpperCase();
   COLhashmapPlace ColumnLookupPlace = pMember->ColumnLookupTable.find( ColumnNameUpperCase );
   if ( ColumnLookupPlace == NULL )
   {
      COL_ERROR_STREAM( COL_T("Column name \"") << ColumnNameUpperCase << COL_T("\" not found in the result set table."), COLerror::PreCondition );
   }
   return pMember->RowVector[ RowIndex ].columnValue( pMember->ColumnLookupTable[ColumnLookupPlace] );
}

// returns COLuint32(-1) if non existent
COLuint32 DBresultSet::columnIndex(const COLstring& ColumnName) const{
   COL_METHOD(DBresultSet::columnIndex);
   pMember->synchronizeColumnLookup();
   COLstring ColumnNameUpperCase = ColumnName;
   ColumnNameUpperCase.toUpperCase();
   COLhashmapPlace ColumnLookupPlace = pMember->ColumnLookupTable.find( ColumnNameUpperCase );
   if ( ColumnLookupPlace == NULL ){
      return (COLuint32)-1;
   } else{
      return pMember->ColumnLookupTable[ColumnLookupPlace];
   }
}
