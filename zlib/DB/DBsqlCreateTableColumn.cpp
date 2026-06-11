//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlCreateTableColumn
//
// Description:
//
// Implementation
//
// Author: Henry Tran
// Date:   December 1, 2003 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBsqlCreateTableColumn.h"
#include "DBvariant.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

class DBsqlCreateTableColumnPrivate
{
public:
   DBsqlCreateTableColumnPrivate()
   : QuoteName(false), 
     DataType(DB_DATA_TYPE_NOT_DEFINED),
     Length(0),
     RequiredFlag(false),
     PrimaryKeyFlag(false),
     HasForeignKey(false)
   {}
   
   ~DBsqlCreateTableColumnPrivate() {}
   
   COLstring Name;
   bool QuoteName;
   DBdataType DataType;
   COLstring DataTypeAsString;
   DBvariant DefaultValue;
   COLuint32  Length;
   bool RequiredFlag;
   bool PrimaryKeyFlag;

   bool HasForeignKey;
   DBsqlCreateTableColumnForeignKey ForeignKey;
};

DBsqlCreateTableColumn::DBsqlCreateTableColumn()
{
   COL_METHOD(DBsqlCreateTableColumn::DBsqlCreateTableColumn);
   pMember = new DBsqlCreateTableColumnPrivate();
}

DBsqlCreateTableColumn::DBsqlCreateTableColumn(const COLstring& Name, DBdataType DataType)
{
   COL_METHOD(DBsqlCreateTableColumn::DBsqlCreateTableColumn);
   COL_VAR(Name);
   pMember = new DBsqlCreateTableColumnPrivate();
   setName(Name);
   setDataType(DataType);
};

DBsqlCreateTableColumn::DBsqlCreateTableColumn( const DBsqlCreateTableColumn& Orig )
{
   COL_METHOD(DBsqlCreateTableColumn::DBsqlCreateTableColumn-CopyCtor);
   pMember = new DBsqlCreateTableColumnPrivate(*Orig.pMember);
}

DBsqlCreateTableColumn& DBsqlCreateTableColumn::operator=( const DBsqlCreateTableColumn& Orig )
{
   COL_METHOD(DBsqlCreateTableColumn::operator=);
   *pMember = *Orig.pMember;
   return *this;
}

DBsqlCreateTableColumn::~DBsqlCreateTableColumn()
{
   COL_METHOD(DBsqlCreateTableColumn::~DBsqlCreateTableColumn);
   delete pMember;
}
   
void DBsqlCreateTableColumn::setName( const COLstring& Name )
{
   COL_METHOD(DBsqlCreateTableColumn::setName);
   COL_VAR(Name);
   COLPRECONDITION( !Name.is_null() );
   pMember->Name = Name;
}

const COLstring& DBsqlCreateTableColumn::name() const
{
   return pMember->Name;
}

void DBsqlCreateTableColumn::setQuoteName( bool QuoteName )
{
   COL_METHOD(DBsqlCreateTableColumn::setQuoteName);
   COL_VAR(QuoteName);
   pMember->QuoteName = QuoteName;
}

bool DBsqlCreateTableColumn::quoteName() const
{
   return pMember->QuoteName;
}

void DBsqlCreateTableColumn::setDataType( DBdataType DataType )
{
   COL_METHOD(DBsqlCreateTableColumn::setDataType);
   pMember->DataType = DataType;
   pMember->DataTypeAsString = dataTypeAsString();
}

const DBdataType DBsqlCreateTableColumn::dataType() const
{
   return pMember->DataType;
}

COLstring DBsqlCreateTableColumn::dataTypeAsString() const
{
   switch(pMember->DataType)
   {
      case DB_DATA_TYPE_NOT_DEFINED:
         return "Undefined Type";
      case DB_STRING:
         return "String";
      case DB_INTEGER:
         return "Integer";
      case DB_DOUBLE:
         return "Double";
      case DB_DATETIME:
         return "Date/Time";
      case DB_TEXT:
         return "Text";
     case DB_BINARY:
        return "Binary";
      default:
         return "UNSPECIFIED";
   }
}


void DBsqlCreateTableColumn::setDefaultValue( const DBvariant& DefaultValue )
{
   COL_METHOD(DBsqlCreateTableColumn::setDefaultValue);
   pMember->DefaultValue = DefaultValue;
}

const DBvariant& DBsqlCreateTableColumn::defaultValue() const
{
   return pMember->DefaultValue;
}

void DBsqlCreateTableColumn::setLength( const COLuint32 Length )
{
   COL_METHOD(DBsqlCreateTableColumn::setLength);
   COL_VAR(Length);
   pMember->Length = Length;
}

const COLuint32& DBsqlCreateTableColumn::length() const
{
   return pMember->Length;
}

void DBsqlCreateTableColumn::setRequiredFlag( bool RequiredFlag )
{
   COL_METHOD(DBsqlCreateTableColumn::setRequiredFlag);
   COL_VAR(RequiredFlag);
   pMember->RequiredFlag = RequiredFlag;
}

const bool& DBsqlCreateTableColumn::requiredFlag() const
{
   return pMember->RequiredFlag;
}

void DBsqlCreateTableColumn::setPrimaryKeyFlag( bool PrimaryKeyFlag )
{
   COL_METHOD(DBsqlCreateTableColumn::setPrimaryKeyFlag);
   COL_VAR(PrimaryKeyFlag);
   pMember->PrimaryKeyFlag = PrimaryKeyFlag;
}

const bool& DBsqlCreateTableColumn::primaryKeyFlag() const
{
   return pMember->PrimaryKeyFlag;
}

void DBsqlCreateTableColumn::addForeignKey
( 
   const COLstring& ForeignTableName, 
   const COLstring& ForeignColumnName,
   DBsqlCreateTableColumnForeignKey::eAction OnDeleteAction,
   DBsqlCreateTableColumnForeignKey::eAction OnUpdateAction,
   bool QuoteForeignTableName,
   bool QuoteForeignColumnName 
)
{
   COL_METHOD(DBsqlCreateTableColumn::addForeignKey);
   COL_VAR2(ForeignTableName, ForeignColumnName);
   COL_VAR2(QuoteForeignTableName, QuoteForeignColumnName);
   COLPRECONDITION( !ForeignTableName.is_null() );
   COLPRECONDITION( !ForeignColumnName.is_null() );
   COLPRECONDITION(pMember->HasForeignKey == false);
   pMember->HasForeignKey = true;
   pMember->ForeignKey.setTableName( ForeignTableName );
   pMember->ForeignKey.setQuoteTableName( QuoteForeignTableName );
   pMember->ForeignKey.setColumnName( ForeignColumnName );
   pMember->ForeignKey.setQuoteColumnName( QuoteForeignColumnName );
   pMember->ForeignKey.setOnDeleteAction( OnDeleteAction );
   pMember->ForeignKey.setOnUpdateAction( OnUpdateAction );
}

const COLstring& DBsqlCreateTableColumn::foreignTableName() const
{
   COLPRECONDITION(pMember->HasForeignKey);
   return pMember->ForeignKey.tableName();
}

bool DBsqlCreateTableColumn::quoteForeignTableName() const
{
   COLPRECONDITION(pMember->HasForeignKey);
   return pMember->ForeignKey.quoteTableName();
}

const COLstring& DBsqlCreateTableColumn::foreignColumnName() const
{
   COLPRECONDITION(pMember->HasForeignKey);
   return pMember->ForeignKey.columnName();
}

bool DBsqlCreateTableColumn::quoteForeignColumnName() const
{
   COLPRECONDITION(pMember->HasForeignKey);
   return pMember->ForeignKey.quoteColumnName();
}

const DBsqlCreateTableColumnForeignKey::eAction DBsqlCreateTableColumn::foreignKeyOnDeleteAction() const
{
   COLPRECONDITION(pMember->HasForeignKey);
   return pMember->ForeignKey.onDeleteAction();
}

const DBsqlCreateTableColumnForeignKey::eAction DBsqlCreateTableColumn::foreignKeyOnUpdateAction() const
{
   COLPRECONDITION(pMember->HasForeignKey);
   return pMember->ForeignKey.onUpdateAction();
}

const bool DBsqlCreateTableColumn::foreignKeyExists() const
{
   return pMember->HasForeignKey;
}

void DBsqlCreateTableColumn::removeForeignKey()
{
   COL_METHOD(DBsqlCreateTableColumn::removeForeignKey);
   pMember->HasForeignKey = false;
   pMember->ForeignKey = DBsqlCreateTableColumnForeignKey();  // reset this object
}
