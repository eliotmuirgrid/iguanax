//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabase
//
// Description:
//
// Implementation
// 
// Author: Henry Tran
// Date:   November 27, 2003 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBdatabase.h"
#include "DBsqlCommandSequence.h"
#include "DBresultSetSequence.h"
#include "DBresultSetPtr.h"
#include "DBsqlCommand.h"
#include "DBsqlWhere.h"
#include "DBsqlWhereItem.h"
#include "DBsqlWhereCondition.h"
#include "DBsqlSelectJoin.h"
#include "DBsqlSelect.h"
#include "DBsqlCreateTable.h"
#include "DBsqlCreateTableColumn.h"
#include "DBsqlDelete.h"
#include "DBsqlInsert.h"
#include "DBsqlSelectUnion.h"
#include "DBsqlSelectOrderBy.h"
#include "DBsqlUpdate.h"
#include "DBvariant.h"
#include "DButils.h"

#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// These are static for serialization.
//
const char* const DBdatabase::DB_DATABASE_MY_SQL_NAME            = "MySQL";
const char* const DBdatabase::DB_DATABASE_ODBC_DB2_NAME          = "ODBC - IBM DB2";
const char* const DBdatabase::DB_DATABASE_ODBC_FILEMAKER_NAME    = "ODBC - Filemaker";
const char* const DBdatabase::DB_DATABASE_ODBC_INFORMIX_NAME     = "ODBC - IBM Informix";
const char* const DBdatabase::DB_DATABASE_ODBC_INTERBASE_NAME    = "ODBC - InterBase/Firebird";
const char* const DBdatabase::DB_DATABASE_SQL_SERVER_NAME        = "ODBC - MS SQL Server";
const char* const DBdatabase::DB_DATABASE_ODBC_ORACLE_NAME       = "ODBC - Oracle";
const char* const DBdatabase::DB_DATABASE_ODBC_ACCESS_NAME       = "ODBC - MS Access";
const char* const DBdatabase::DB_DATABASE_ODBC_POSTGRESQL_NAME   = "ODBC - PostgreSQL";
const char* const DBdatabase::DB_DATABASE_ODBC_MYSQL_NAME        = "ODBC - MySQL";
const char* const DBdatabase::DB_DATABASE_ODBC_SYBASE_NAME       = "ODBC - Sybase ASA";
const char* const DBdatabase::DB_DATABASE_ODBC_SYBASE_ASE_NAME   = "ODBC - Sybase ASE";
const char* const DBdatabase::DB_DATABASE_SQLITE_NAME            = "SQLite";
const char* const DBdatabase::DB_DATABASE_MARIA_DB_NAME          = "MariaDB";

// And now the special case - Eliot
const char* const DBdatabase::DB_DATABASE_OCI_ORACLE_NAME        = "OCI - Oracle";
const char* const DBdatabase::DB_DATABASE_OCI_ORACLE_LEGACY_NAME = "OCI - Oracle 9i+";  // Ye legacy erroneously versioned name



const DBdatabase::DBdatabaseNames DBdatabase::AllDatabaseNames[] = {
   { DBdatabase::DB_DATABASE_MY_SQL_NAME },
   { DBdatabase::DB_DATABASE_OCI_ORACLE_NAME },
   { DBdatabase::DB_DATABASE_SQLITE_NAME},
   { DBdatabase::DB_DATABASE_ODBC_ORACLE_NAME },
   { DBdatabase::DB_DATABASE_ODBC_ACCESS_NAME },
   { DBdatabase::DB_DATABASE_ODBC_DB2_NAME },
   { DBdatabase::DB_DATABASE_ODBC_FILEMAKER_NAME },
   { DBdatabase::DB_DATABASE_ODBC_INFORMIX_NAME },
   { DBdatabase::DB_DATABASE_ODBC_INTERBASE_NAME },
   { DBdatabase::DB_DATABASE_SQL_SERVER_NAME },
   { DBdatabase::DB_DATABASE_ODBC_POSTGRESQL_NAME },
   { DBdatabase::DB_DATABASE_ODBC_MYSQL_NAME },
   { DBdatabase::DB_DATABASE_ODBC_SYBASE_NAME },
   { DBdatabase::DB_DATABASE_ODBC_SYBASE_ASE_NAME },
   { DBdatabase::DB_DATABASE_MARIA_DB_NAME },
   { NULL}
};

const char* DBdatabase::databaseName() const {
   COL_METHOD(DBdatabase::databaseName);
   const DBdatabaseType DatabaseType = databaseType();

   switch(DatabaseType){
   case DB_MY_SQL:          return DB_DATABASE_MY_SQL_NAME;
   case DB_SQLITE:          return DB_DATABASE_SQLITE_NAME;
   case DB_SQL_SERVER:      return DB_DATABASE_SQL_SERVER_NAME;
   case DB_OCI_ORACLE:      return DB_DATABASE_OCI_ORACLE_NAME;
   case DB_ODBC_ORACLE:     return DB_DATABASE_ODBC_ORACLE_NAME;
   case DB_ODBC_DB2:        return DB_DATABASE_ODBC_DB2_NAME;
   case DB_ODBC_FILEMAKER:  return DB_DATABASE_ODBC_FILEMAKER_NAME;
   case DB_ODBC_INTERBASE:  return DB_DATABASE_ODBC_INTERBASE_NAME;
   case DB_ODBC_INFORMIX:   return DB_DATABASE_ODBC_INFORMIX_NAME;
   case DB_ODBC_ACCESS:     return DB_DATABASE_ODBC_ACCESS_NAME;
   case DB_ODBC_POSTGRESQL: return DB_DATABASE_ODBC_POSTGRESQL_NAME;
   case DB_ODBC_MYSQL:      return DB_DATABASE_ODBC_MYSQL_NAME;
   case DB_ODBC_SYBASE:     return DB_DATABASE_ODBC_SYBASE_NAME;
   case DB_ODBC_SYBASE_ASE: return DB_DATABASE_ODBC_SYBASE_ASE_NAME;
   case DB_MARIA_DB:        return DB_DATABASE_MARIA_DB_NAME;
   }

   COL_ERROR_STREAM_PLAIN( COL_T("Database type ") << DatabaseType << COL_T(" (enum) not supported."), COLerror::PreCondition );
}

DBdatabase::DBdatabase() : m_CachedAutoCommitFlag(true){
   COL_METHOD(DBdatabase::DBdatabase);
}

DBdatabase::~DBdatabase(){
   COL_METHOD(DBdatabase::~DBdatabase);
}

COLstring DBdatabase::dateTimeToString(const COLdateTime& DateTime){
   COL_METHOD(DBdatabase::dateTimeToString);
   COLstring DateAsString;

   COLostream DateAsStringStream(DateAsString);
   streamDateTime(DateAsStringStream, DateTime);

   COL_VAR(DateAsString);

   return DateAsString;
} 

void DBdatabase::createSqlCommand( const DBsqlCommand& SqlCommand, COLostream& OutputSql ) const {
   COL_METHOD(DBdatabase::createSqlCommand);
   switch (SqlCommand.commandType()){
   case DB_SELECT_CMD:       createSqlSelect( (DBsqlSelect&) SqlCommand, OutputSql );           break;
   case DB_SELECT_UNION_CMD: createSqlSelectUnion( (DBsqlSelectUnion&) SqlCommand, OutputSql ); break;
   case DB_UPDATE_CMD:       createSqlUpdate( (DBsqlUpdate&) SqlCommand, OutputSql );           break;
   case DB_INSERT_CMD:       createSqlInsert( (DBsqlInsert&) SqlCommand, OutputSql );           break;
   case DB_DELETE_CMD:       createSqlDelete( (DBsqlDelete&) SqlCommand, OutputSql );           break;
   case DB_DROP_CMD:         createSqlDropTable( (DBsqlDropTable&) SqlCommand, OutputSql );     break;
   case DB_CREATE_TABLE_CMD: createSqlCreateTable( (DBsqlCreateTable&) SqlCommand, OutputSql ); break;
   default: throw COLerror( "Specified SQL command not supported.", COLerror::PreCondition ); 
   }
}

void DBdatabase::createSqlCommandWithoutBinding( const DBsqlCommand& SqlCommand, COLostream& OutputSql ) const{
   COL_METHOD(DBdatabase::createSqlCommandWithoutBinding);
   //default is just to call the regular function as most databases don't have parameter binding
   //the databases that do have parameter binding will override this function
   createSqlCommand(SqlCommand, OutputSql);
}

DBresultSetPtr DBdatabase::executeSqlCommand( const DBsqlCommand& SqlCommand, const COLuint32& ResultSetStart, const COLuint32& ResultSetSize) const{
   COL_METHOD(DBdatabase::executeSqlCommand);
   switch (SqlCommand.commandType()){
   case DB_SELECT_CMD:       return executeSqlSelect( (DBsqlSelect&) SqlCommand, ResultSetStart, ResultSetSize );
   case DB_SELECT_UNION_CMD: return executeSqlSelectUnion( (DBsqlSelectUnion&) SqlCommand, ResultSetStart, ResultSetSize );
   case DB_UPDATE_CMD:       return executeSqlUpdate( (DBsqlUpdate&) SqlCommand );
   case DB_INSERT_CMD:       return executeSqlInsert( (DBsqlInsert&) SqlCommand );
   case DB_DELETE_CMD:       return executeSqlDelete( (DBsqlDelete&) SqlCommand );
   case DB_DROP_CMD:         return executeSqlDropTable( (DBsqlDropTable&) SqlCommand );
   case DB_CREATE_TABLE_CMD: return executeSqlCreateTable( (DBsqlCreateTable&) SqlCommand );
   default:                  throw  COLerror( "Specified SQL command not supported.", COLerror::PreCondition ); } 
}

COLostream& DBdatabase::addWhereClauseToStream( COLostream& Stream, const DBsqlWhere& WhereClause ) const{  
   COL_METHOD(DBdatabase::addWhereClauseToStream);
   if ( WhereClause.negateFlag() == true ){
      Stream << COL_T(" NOT ");
   }

   Stream << COL_T(" (");
   for (int i = 0; i < WhereClause.countOfItem(); i++){
      switch ( WhereClause.item(i).itemType() ){
         case DBsqlWhereItem::ITEM_TYPE_CONDITION:
            addWhereConditionToStream( Stream, WhereClause.item(i).condition() );
            break;
         case DBsqlWhereItem::ITEM_TYPE_COMBINER:
            if ( WhereClause.item(i).combiner() == DBsqlWhereItem::AND_COMBINER ){
               Stream << COL_T(" AND ");
            } else {
               Stream << COL_T(" OR ");
            }
            break;
         case DBsqlWhereItem::ITEM_TYPE_NESTED_WHERE:
            addWhereClauseToStream( Stream, WhereClause.item(i).nestedWhere() );
            break;
         default:
            throw COLerror("WHERE item type not defined.", COLerror::PreCondition);
      }
   }
   Stream << COL_T(") ");

   return Stream;
}

COLostream& DBdatabase::addWhereConditionToStream( COLostream& Stream, const DBsqlWhereCondition& WhereCondition ) const {
   COL_METHOD(DBdatabase::addWhereConditionToStream);
   if ( WhereCondition.negateFlag() == true ) {
      Stream << COL_T(" NOT (");
   }

   streamName( Stream, WhereCondition.leftOperandColumnName(), WhereCondition.quoteLeftOperandColumnName() );

   const DBsqlWhereCondition::eOperator ConditionOperator = WhereCondition.conditionOperator();

   switch ( ConditionOperator ) {
      case DBsqlWhereCondition::EQUAL:                 Stream << COL_T(" = ");       break;
      case DBsqlWhereCondition::NOT_EQUAL:             Stream << COL_T(" <> ");      break;
      case DBsqlWhereCondition::LESS_THAN:             Stream << COL_T(" < ");       break;
      case DBsqlWhereCondition::LESS_THAN_OR_EQUAL:    Stream << COL_T(" <= ");      break;
      case DBsqlWhereCondition::GREATER_THAN:          Stream << COL_T(" > ");       break;
      case DBsqlWhereCondition::GREATER_THAN_OR_EQUAL: Stream << COL_T(" >= ");      break;
      case DBsqlWhereCondition::LIKE:                  Stream << COL_T(" LIKE ");    break;
      case DBsqlWhereCondition::IS_NULL:               Stream << COL_T(" IS NULL "); break;
      case DBsqlWhereCondition::BETWEEN:               Stream << COL_T(" BETWEEN "); break;
      case DBsqlWhereCondition::IN_SET: 
      case DBsqlWhereCondition::IN_SELECT:             Stream << COL_T(" IN ");      break;
      default:                                         throw COLerror( "WHERE conditon operator not defined.", COLerror::PreCondition );
   }

   // "IS_NULL" does not require a right operand.
   if ( ConditionOperator != DBsqlWhereCondition::IS_NULL ) {
      if ( ConditionOperator ==  DBsqlWhereCondition::BETWEEN ) {
         // "BETWEEN" requires two right operands (i.e. "abc AND xyz")
         if (  WhereCondition.countOfRightOperandValue() != 2 ) {
            throw COLerror( "BETWEEN condition requires two values for the right operand." , COLerror::PreCondition );
         }

         addVariantToStream( Stream, WhereCondition.rightOperandValue( 0 ), false, false );
         Stream << COL_T(" AND ");
         addVariantToStream( Stream, WhereCondition.rightOperandValue( 1 ), false, false );
      } else if ( ConditionOperator == DBsqlWhereCondition::IN_SET ) {
         // Multiple values in the right operand enclosed in brackets.
         Stream << '(';
         // TODO - unsigned ints are usually bad news.
         for ( COLuint32 ValueIndex = 0; ValueIndex < WhereCondition.countOfRightOperandValue(); ValueIndex++ ) {
            if ( ValueIndex > 0 ) {
               Stream << ',';
            }
            addVariantToStream( Stream, WhereCondition.rightOperandValue( ValueIndex ), false, false );
         }
         Stream << ')';
      } else if ( ConditionOperator == DBsqlWhereCondition::IN_SELECT ) {
         if ( !WhereCondition.rightOperandselectQueryExists() )
         {
            throw COLerror( "IN_SELECT condition requires a SELECT query to be the right operand." , COLerror::PreCondition );
         }

         Stream << '(';
         addSelectCommandToStream( Stream, WhereCondition.rightOperandselectQuery() );
         Stream << ')';
      } else {
         if (WhereCondition.rightOperandselectQueryExists()) {
            Stream << '(';
            addSelectCommandToStream( Stream, WhereCondition.rightOperandselectQuery() );
            Stream << ')';
         } else {
            // All other operators require only one right operand value.
            addVariantToStream( Stream, WhereCondition.rightOperandValue(), WhereCondition.rightOperandIsColumnFlag(), WhereCondition.quoteRightOperandColumnName() );
         }

      }
   }

   if ( WhereCondition.negateFlag() == true ) {
      Stream << COL_T(") ");
   }

   return Stream;
}

static bool DBisOracle(const DBdatabase& Database) {
   DBdatabaseType DatabaseType = Database.databaseType();
   return (DatabaseType == DB_OCI_ORACLE || DatabaseType == DB_ODBC_ORACLE);
}

COLostream& DBdatabase::addJoinClauseToStream( COLostream& Stream, const DBsqlSelectJoin& JoinClause ) const {
   COL_METHOD(DBdatabase::addJoinClauseToStream);
   if ( !JoinClause.leftTableName().is_null() ) {
      Stream << '(';
      streamName( Stream, JoinClause.leftTableName(), JoinClause.quoteLeftTableName() );
      if ( !JoinClause.leftTableAlias().is_null() ) {
         Stream << (DBisOracle(*this)?" ":" AS ");
         streamName( Stream, JoinClause.leftTableAlias(), JoinClause.quoteLeftTableAlias() );
      }
   }

   switch( JoinClause.joinType() ) {
      case DBsqlSelectJoin::LEFT_OUTER:  Stream << COL_T(" LEFT JOIN ");  break;
      case DBsqlSelectJoin::RIGHT_OUTER: Stream << COL_T(" RIGHT JOIN "); break;
      case DBsqlSelectJoin::FULL_OUTER:  Stream << COL_T(" FULL JOIN ");  break;
      case DBsqlSelectJoin::INNER:       Stream << COL_T(" INNER JOIN "); break;
      case DBsqlSelectJoin::CROSS:       Stream << COL_T(" CROSS JOIN "); break;     
      default:
         COLstring ErrorString;
         COLostream ErrorStream( ErrorString );
         ErrorStream << COL_T("Join type undefined.") << newline << flush;
         throw COLerror( ErrorString, COLerror::PreCondition );
   }

   if ( !JoinClause.nestedJoinExists() ) {
      streamName( Stream, JoinClause.rightTableName(), JoinClause.quoteRightTableName() );
      if ( !JoinClause.rightTableAlias().is_null() ) {
         Stream << (DBisOracle(*this)?" ":" AS ");
         streamName( Stream, JoinClause.rightTableAlias(), JoinClause.quoteRightTableAlias() );
      }
   } else {
      addJoinClauseToStream( Stream, JoinClause.nestedJoin() );
   }
   
   if ( JoinClause.onExpressionExists() ) {
      Stream << COL_T(" ON ");
      addWhereClauseToStream( Stream, JoinClause.onExpression() );
   }

   if ( !JoinClause.leftTableName().is_null() ) {
      Stream << ')';
   }
   
   if ( JoinClause.cascadedJoinExists() ) {
      addJoinClauseToStream( Stream, JoinClause.cascadedJoin() );
   }
   
   return Stream;
}

COLostream& DBdatabase::addSelectCommandToStream( COLostream& Stream, const DBsqlSelect& SqlSelectCommand, bool DoNotStreamOrderBy ) const {  
   COL_METHOD(DBdatabase::addSelectCommandToStream);
   for ( COLuint32 ColumnIndex = 0; ColumnIndex < SqlSelectCommand.countOfColumn(); ColumnIndex++ ) {
      if ( ColumnIndex == 0 ) {
         Stream << COL_T("SELECT ");
         if ( SqlSelectCommand.distinctFlag() == true ) {
            Stream << COL_T("DISTINCT ");
         }
      } else {
         Stream << ',';
      }

      if ( selectCastSupported() ) {
         if (SqlSelectCommand.columnCastType( ColumnIndex ) == DB_STRING ) {
            Stream << COL_T("CAST(");
         }
      }

      streamName( Stream, SqlSelectCommand.columnName(ColumnIndex), SqlSelectCommand.quoteColumnName(ColumnIndex) );
      
      if ( selectCastSupported() ) {
         if (SqlSelectCommand.columnCastType( ColumnIndex ) == DB_STRING ) {
            Stream << COL_T(" AS ") << selectCastStringType() << ')';
         }

         if ( SqlSelectCommand.columnAlias( ColumnIndex ).is_null() && (SqlSelectCommand.columnCastType( ColumnIndex ) == DB_STRING) ) {
            Stream << COL_T(" AS ");
            streamName( Stream, SqlSelectCommand.columnName(ColumnIndex), SqlSelectCommand.quoteColumnName(ColumnIndex) );
         }
      }

      if ( !SqlSelectCommand.columnAlias( ColumnIndex ).is_null() ) {
         DB2_LOG( COL_T("DBdatabase::addSelectCommandToStream : ") << SqlSelectCommand.columnAlias(ColumnIndex) );
         Stream << COL_T(" AS ");
         streamName( Stream, SqlSelectCommand.columnAlias(ColumnIndex), SqlSelectCommand.quoteColumnAlias(ColumnIndex) );
      }
   }

   for ( COLuint32 TableIndex = 0; TableIndex < SqlSelectCommand.countOfTableName(); TableIndex++ ) {
      if ( TableIndex == 0 ) {
         Stream << COL_T(" FROM ");   
      } else {
         Stream << ',';
      }

      streamName( Stream, SqlSelectCommand.tableName(TableIndex), SqlSelectCommand.quoteTableName(TableIndex) );

      if ( !SqlSelectCommand.tableAlias( TableIndex ).is_null() ) {
         Stream << (DBisOracle(*this)?" ":" AS ");
         streamName( Stream, SqlSelectCommand.tableAlias(TableIndex), SqlSelectCommand.quoteTableAlias(TableIndex) );
      }
   }

   if ( SqlSelectCommand.joinClauseExists() ) {
      if ( SqlSelectCommand.countOfTableName() == 0 ) {
         Stream << COL_T(" FROM ");
      } else {
         Stream << COL_T(" , ");
      }
      addJoinClauseToStream( Stream, SqlSelectCommand.joinClause() );
   }

   if ( SqlSelectCommand.whereClauseExists() ) {
      Stream << COL_T(" WHERE ");
      addWhereClauseToStream( Stream, SqlSelectCommand.whereClause() );
   }

   bool GroupByStringStarted = false;
   for ( COLuint32 GroupByIndex = 0; GroupByIndex < SqlSelectCommand.countOfGroupByColumnName(); GroupByIndex++ ) {
      if ( !SqlSelectCommand.groupByColumnName(GroupByIndex).is_null() ) {
         if ( !GroupByStringStarted ) {
            Stream << COL_T(" GROUP BY "); 
            GroupByStringStarted = true;
         } else {
            Stream << ',';
         }
         streamName( Stream, SqlSelectCommand.groupByColumnName(GroupByIndex), SqlSelectCommand.quoteGroupByColumnName(GroupByIndex) );
      }
   }
   
   if ( SqlSelectCommand.havingClauseExists() ) {
      Stream << COL_T(" HAVING ");
      addWhereClauseToStream( Stream, SqlSelectCommand.havingClause() );
   }

   if ( !DoNotStreamOrderBy ) {
      bool OrderByFirstColumn = true;
      for ( COLuint32 OrderByIndex = 0; OrderByIndex < SqlSelectCommand.countOfOrderByColumn(); OrderByIndex++ ) {
         if ( !SqlSelectCommand.orderByColumnName(OrderByIndex).is_null() ) {
            streamOrderByColumn( Stream, SqlSelectCommand.orderByColumn(OrderByIndex), OrderByFirstColumn );
            OrderByFirstColumn = false;
         }
      }
   }

   return Stream;
}

void DBdatabase::createSqlCreateTable( const DBsqlCreateTable& SqlCreateTableCommand, COLostream& SqlStream ) const {
   COL_METHOD(DBdatabase::createSqlCreateTable);
   SqlStream << COL_T("CREATE TABLE ");
   streamName( SqlStream, SqlCreateTableCommand.tableName(), SqlCreateTableCommand.quoteTableName() );
   SqlStream << COL_T(" (");

   // Separate primary key constraint is required for primary keys that span multiple columns.
   // Note that this still works for single column primary keys.
   COLstring PrimaryKeyString;
   COLostream PrimaryKeyStream( PrimaryKeyString );

   COL_TRC("Starting loop through columns");
   for ( COLuint32 ColumnIndex = 0; ColumnIndex < SqlCreateTableCommand.countOfColumn(); ColumnIndex++ ) {
      COL_DBG("Currently on column #" << ColumnIndex);
      if ( ColumnIndex > 0 ) {
         SqlStream << ',';
      }
      streamName ( SqlStream, SqlCreateTableCommand.column(ColumnIndex).name(), SqlCreateTableCommand.column(ColumnIndex).quoteName() );
      SqlStream << ' ';

      const DBsqlCreateTableColumn& CurrentColumn = SqlCreateTableCommand.column(ColumnIndex);
      COL_TRC("Current column: name = " << CurrentColumn.name() << ", type = " << CurrentColumn.dataTypeAsString());

      COL_TRC("Adding base type");
      switch ( SqlCreateTableCommand.column( ColumnIndex ).dataType() ) {
         case DB_STRING:
            SqlStream << dbStringType() << '(';
            if ( SqlCreateTableCommand.column( ColumnIndex ).length() == 0 ) {
               if ( SqlCreateTableCommand.column( ColumnIndex ).primaryKeyFlag() == true ) {
                  SqlStream << dbStringMaxKeyLength() << ')';
               } else {
                  SqlStream << COL_T("255)");
               }
            } else {
               SqlStream << SqlCreateTableCommand.column( ColumnIndex ).length() << ')';
            }
            break;
         case DB_INTEGER:
            SqlStream << dbIntegerType();
            if ( SqlCreateTableCommand.column( ColumnIndex ).length() > 0 ) {
               SqlStream << '(' << SqlCreateTableCommand.column( ColumnIndex ).length() << ')';
            }
            break;
         case DB_DOUBLE:
            SqlStream << dbDoubleType();
            break;
         case DB_DATETIME:
            SqlStream << dbDateTimeType();
            break;
       case DB_TEXT:
          SqlStream << dbTextType();
          break;
       case DB_BINARY:
          SqlStream << dbBinaryType();
          if ( SqlCreateTableCommand.column( ColumnIndex ).length() > 0 ) {
             SqlStream << '(' << SqlCreateTableCommand.column( ColumnIndex ).length() << ')';
          }
          break;
         case DB_DATA_TYPE_NOT_DEFINED:
            COL_ERROR_STREAM( "DBdataType not defined in create table statement.", COLerror::PreCondition );
            break;
         default:
            COL_ERROR_STREAM( "Specified DBdataType not supported for create table statement.", COLerror::PreCondition );
      }

      COL_TRC("Adding extra details");

      if ( SqlCreateTableCommand.column( ColumnIndex ).primaryKeyFlag() == true ) {
         COL_TRC("Primary key flag is true");
         SqlStream << dbKeyModifiers();
         if ( !PrimaryKeyString.is_null() ) {
            PrimaryKeyStream << ',';
         }
         
         streamName( PrimaryKeyStream, SqlCreateTableCommand.column(ColumnIndex).name(), SqlCreateTableCommand.column(ColumnIndex).quoteName() );
      } else {
         COL_TRC("Nothing to do for primary key");
      }

      streamCreateTableNotNull( SqlStream, SqlCreateTableCommand, ColumnIndex );

      if ( createTableDefaultConstraintSupported() ) {
         if ( supportsDefault() && SqlCreateTableCommand.column( ColumnIndex ).defaultValue().dataType() != DB_DATA_TYPE_NOT_DEFINED) {
            COL_TRC("Adding default value: " << DBvariantDebugString(CurrentColumn.defaultValue()));
            SqlStream << COL_T(" DEFAULT "); 
            addVariantToStream( SqlStream, SqlCreateTableCommand.column( ColumnIndex ).defaultValue(), false, false );
         }
      } else {
         COL_TRC("Nothing to do for default value");
      }

      streamCreateTableNotNull2( SqlStream, SqlCreateTableCommand, ColumnIndex );

      if ( SqlCreateTableCommand.column( ColumnIndex ).foreignKeyExists() ) {
         COL_TRC("Foreign key exists: table = " << CurrentColumn.foreignTableName() << ", column = " << CurrentColumn.foreignColumnName());
         streamCreateTableForeignKeyPrefix( SqlStream, SqlCreateTableCommand, ColumnIndex );
         
         // Now able to specify foreign key constraint for the local column.
         SqlStream << COL_T(",FOREIGN KEY (");
         streamName( SqlStream, SqlCreateTableCommand.column(ColumnIndex).name(), SqlCreateTableCommand.column(ColumnIndex).quoteName() );
         SqlStream << COL_T(") REFERENCES ");
         streamName( SqlStream, SqlCreateTableCommand.column(ColumnIndex).foreignTableName(), SqlCreateTableCommand.column(ColumnIndex).quoteForeignTableName() );
         SqlStream << COL_T(" (");
         streamName( SqlStream, SqlCreateTableCommand.column(ColumnIndex).foreignColumnName(), SqlCreateTableCommand.column(ColumnIndex).quoteForeignColumnName() );
         SqlStream << COL_T(") ");
         
         streamCreateTableForeignKeySuffix( SqlStream, SqlCreateTableCommand, ColumnIndex );
      } else {
         COL_TRC("Nothing to do for foreign key");
      }
      COL_TRC("Done for this column");
   }
   COL_TRC("Done going through columns");
   
   if ( !PrimaryKeyString.is_null() && supportsDefinePrimaryKey()) {
      COL_TRC("Adding primary key string");
      SqlStream << COL_T(",PRIMARY KEY (") << PrimaryKeyString << ')';
   }

   SqlStream << ')';
   streamCreateTableSuffix( SqlStream, SqlCreateTableCommand );
}

void DBdatabase::createSqlDelete( const DBsqlDelete& SqlDeleteCommand, COLostream& SqlStream  ) const {
   COL_METHOD(DBdatabase::createSqlDelete);
   SqlStream << COL_T("DELETE FROM ");
   streamName( SqlStream, SqlDeleteCommand.tableName(), SqlDeleteCommand.quoteTableName() );
   if ( SqlDeleteCommand.whereClauseExists() ) {
      SqlStream << COL_T(" WHERE ");
      addWhereClauseToStream( SqlStream, SqlDeleteCommand.whereClause() );
   }
}

void DBdatabase::createSqlDropTable( const DBsqlDropTable& SqlDropCommand, COLostream& SqlStream ) const {
   COL_METHOD(DBdatabase::createSqlDropTable);
   SqlStream << COL_T("DROP TABLE ");
   streamName( SqlStream, SqlDropCommand.tableName(), SqlDropCommand.quoteTableName() );
}

void DBdatabase::createSqlInsert( const DBsqlInsert& SqlInsertCommand, COLostream& SqlStream  ) const {
   COL_METHOD(DBdatabase::createSqlInsert);
   SqlStream << COL_T("INSERT INTO ");
   streamName( SqlStream, SqlInsertCommand.tableName(), SqlInsertCommand.quoteTableName() );
   SqlStream << '(';
   
   // Column names and values are specified in separate parentheses.
   COLstring ValuesSting;
   COLostream ValuesStream( ValuesSting );

   for ( COLuint32 ColumnIndex = 0; ColumnIndex < SqlInsertCommand.countOfColumn(); ColumnIndex++ ) {
      if ( ColumnIndex > 0 ) {
         SqlStream << ',';
         ValuesStream << ',';
      }
      streamName( SqlStream, SqlInsertCommand.columnName(ColumnIndex), SqlInsertCommand.quoteColumnName(ColumnIndex) );
      streamInsertUpdateValue( ValuesStream, SqlInsertCommand, ColumnIndex );
   }
   SqlStream << COL_T(") ");
   
   if (SqlInsertCommand.selectQueryExists()) {
      addSelectCommandToStream(SqlStream, SqlInsertCommand.selectQuery());
   } else {
      SqlStream << COL_T("VALUES(") << ValuesSting << ')';
   } 
}

void DBdatabase::createSqlSelect( const DBsqlSelect& SqlSelectCommand, COLostream& SqlStream) const {
   COL_METHOD(DBdatabase::createSqlSelect);
   addSelectCommandToStream( SqlStream, SqlSelectCommand );
}

bool DBdatabase::selectUnionSeparateOrderBySupported() const {
   COL_METHOD(DBdatabase::selectUnionSeparateOrderBySupported);
   return true;
}

void DBdatabase::createSqlSelectUnion( const DBsqlSelectUnion& SqlUnion, COLostream& SqlStream) const {
   COL_METHOD(DBdatabase::createSqlSelectUnion);
   for ( COLuint32 SelectIndex = 0; SelectIndex < SqlUnion.countOfSelectQuery(); SelectIndex++ ) {
      // Most databases support a global ORDER BY clause for select unions. Furthermore, Oracle and
      // SQL Server requires the ORDER BY to be global and not in each individual SELECT query. However,
      // Sybase doesn't support the global ORDER BY constraint at all, hence the hacky looking code.
      if ( !selectUnionSeparateOrderBySupported() ) {
         for ( COLuint32 OrderByIndex = 0; OrderByIndex < SqlUnion.countOfOrderByColumn(); OrderByIndex++ ) {
            DBsqlSelectOrderBy& OrderByColumn = SqlUnion.orderByColumn( OrderByIndex );
            SqlUnion.selectQuery( SelectIndex ).addOrderByColumn( OrderByColumn.columnName(), OrderByColumn.descendingFlag(), OrderByColumn.quoteName() );
         }
      }

      if ( SelectIndex > 0 ) {
         SqlStream << COL_T(" UNION ");
      }

      if ( SqlUnion.allFlag() ) {
         SqlStream << COL_T("ALL ");
      }

      if (selectUnionGroupSelects()) SqlStream << '(';
      addSelectCommandToStream( SqlStream, SqlUnion.selectQuery( SelectIndex ), selectUnionSeparateOrderBySupported() );
      if (selectUnionGroupSelects()) SqlStream << ')';
   }
   
   if ( selectUnionSeparateOrderBySupported() ) {
      for ( COLuint32 OrderByIndex = 0; OrderByIndex < SqlUnion.countOfOrderByColumn(); OrderByIndex++ ) {
         streamOrderByColumn( SqlStream, SqlUnion.orderByColumn(OrderByIndex), (OrderByIndex == 0) );
      }
   }
}

void DBdatabase::createSqlUpdate( const DBsqlUpdate& SqlUpdateCommand, COLostream& SqlStream  ) const {
   COL_METHOD(DBdatabase::createSqlUpdate);
   SqlStream << COL_T("UPDATE ");
   streamName( SqlStream, SqlUpdateCommand.tableName(), SqlUpdateCommand.quoteTableName() );
   SqlStream << COL_T(" SET ");

   for ( COLuint32 ColumnIndex = 0; ColumnIndex < SqlUpdateCommand.countOfColumn(); ColumnIndex++ ) {
      if ( ColumnIndex > 0 ) {
         SqlStream << ',';
      }
      streamName( SqlStream, SqlUpdateCommand.columnName(ColumnIndex), SqlUpdateCommand.quoteColumnName(ColumnIndex) );
      SqlStream << '=';
      streamInsertUpdateValue( SqlStream, SqlUpdateCommand, ColumnIndex );
   }

   if ( SqlUpdateCommand.whereClauseExists() ) {
      SqlStream << COL_T(" WHERE ");
      addWhereClauseToStream( SqlStream, SqlUpdateCommand.whereClause() );
   }
}

static const COLstring DBdatabaseTerminator(";");
const COLstring& DBdatabase::sqlStatementTerminator() const {
   return DBdatabaseTerminator;
}

COLostream& DBdatabase::streamName( COLostream& Stream, const COLstring& Name, bool QuoteName ) const {
   if (Name.find("sysquery") != npos ) {
//      COLcout << COL_T("sysquery in ") << Name << newline;
   }

   if ( QuoteName == true ) {
      Stream << '\"' << Name << '\"';
   } else {
      Stream << Name;
   }
   return Stream;
}

DBresultSetPtr DBdatabase::executeSqlDelete( const DBsqlDelete& SqlDeleteCommand ) const {
   COL_METHOD(DBdatabase::executeSqlDelete);
   COLstring SqlString;
   COLostream SqlStream(SqlString);
   createSqlDelete(SqlDeleteCommand,SqlStream);
   return executeSqlString( SqlString.c_str() );
}

DBresultSetPtr DBdatabase::executeSqlDropTable( const DBsqlDropTable& SqlDropCommand ) const {
   COL_METHOD(DBdatabase::executeSqlDropTable);
   COLstring SqlString;
   COLostream SqlStream( SqlString );
   createSqlDropTable(SqlDropCommand, SqlStream);
   return executeSqlString( SqlString.c_str() );
}

DBresultSetPtr DBdatabase::executeSqlInsert( const DBsqlInsert& SqlInsertCommand ) const {
   COL_METHOD(DBdatabase::executeSqlInsert);
   COL_FUNCTION(executeSqlInsert);
   COLstring SqlString;
   COLostream SqlStream( SqlString );
   createSqlInsert(SqlInsertCommand,SqlStream);
   COL_VAR(SqlString);
   return executeSqlString( SqlString.c_str() );
}

DBresultSetPtr DBdatabase::executeSqlSelect (const DBsqlSelect& SqlSelectCommand, const COLuint32& ResultSetStart, const COLuint32& ResultSetSize) const {
   COL_METHOD(DBdatabase::executeSqlSelect);
   COLstring SqlString;
   COLostream SqlStream( SqlString );
   createSqlSelect(SqlSelectCommand,SqlStream);
   return executeSqlString( SqlString.c_str(), ResultSetStart, ResultSetSize );
}

DBresultSetPtr DBdatabase::executeSqlSelectUnion (const DBsqlSelectUnion& SqlUnion, COLuint32 ResultSetStart, COLuint32 ResultSetSize) const {
   COL_METHOD(DBdatabase::executeSqlSelectUnion);
   COLstring SqlString;
   COLostream SqlStream( SqlString );
   createSqlSelectUnion(SqlUnion,SqlStream);
   DB2_LOG(COL_T("executeSqlSelectUnion: ") << SqlString)
   return executeSqlString( SqlString.c_str(), ResultSetStart, ResultSetSize );
}

DBresultSetPtr DBdatabase::executeSqlUpdate( const DBsqlUpdate& SqlUpdateCommand ) const {
   COL_METHOD(DBdatabase::executeSqlUpdate);
   COLstring SqlString;
   COLostream SqlStream( SqlString );
   createSqlUpdate(SqlUpdateCommand,SqlStream);
   return executeSqlString( SqlString.c_str() );
}

DBresultSetPtr DBdatabase::executeSqlCreateTable( const DBsqlCreateTable& SqlCreateTableCommand ) const {
   COL_METHOD(DBdatabase::executeSqlCreateTable);
   COLstring SqlString;
   COLostream SqlStream( SqlString );
   createSqlCreateTable(SqlCreateTableCommand,SqlStream);
   return executeSqlString( SqlString.c_str() );
}

void DBdatabase::setCachedDataSourceName(const COLstring& DataSourceName) { m_CachedDataSourceName = DataSourceName; }
void DBdatabase::setCachedUserName      (const COLstring& UserName)       { m_CachedUserName = UserName; }
void DBdatabase::setCachedPassword      (const COLstring& Password)       { m_CachedPassword = Password; }

void DBdatabase::setCachedAutoCommitFlag(bool NewValue) {
   COL_METHOD(DBdatabase::setCachedAutoCommitFlag);
   COL_VAR(NewValue);
   m_CachedAutoCommitFlag = NewValue; 
}

const COLstring& DBdatabase::cachedDataSourceName() const { return m_CachedDataSourceName; }
const COLstring& DBdatabase::cachedUserName()       const { return m_CachedUserName; }
const COLstring& DBdatabase::cachedPassword()       const { return m_CachedPassword; }

bool DBdatabase::transactionActive() const{ 
   COL_METHOD(DBdatabase::transactionActive);
   COL_VAR(m_CachedAutoCommitFlag); 
   return !m_CachedAutoCommitFlag; 
}

// TODO - it's just a bad idea having some of the implementation in DBdatabase - it should
// be purely virtual and implemented by each child
void DBdatabase::beginTransaction(){
   COL_METHOD(DBdatabase::beginTransaction);
   setAutocommitFlag(false);
   setCachedAutoCommitFlag(false);
}

void DBdatabase::endTransaction(){
   COL_METHOD(DBdatabase::endTransaction);
   if (isConnected()){
      setAutocommitFlag(true);
   }
   setCachedAutoCommitFlag(true);
}

bool DBdatabase::selectCastSupported() const { return true; }

static const COLstring DBkeyModifiers;
static const COLstring DBstringMaxKeyLength("255");
static const COLstring DBcastStringType("VARCHAR(255)");
static const COLstring DB_DATATYPE_VARCHAR = "VARCHAR";
static const COLstring DBintegerType("INTEGER");
static const COLstring DBdoubleType("DOUBLE");
static const COLstring DBdateTimeType("DATETIME");
static const COLstring DBtextType("TEXT");
static const COLstring DBbinaryType("BINARY");

const COLstring& DBdatabase::dbKeyModifiers() const { return DBkeyModifiers; }
const COLstring& DBdatabase::dbStringMaxKeyLength() const { return DBstringMaxKeyLength; }
const COLstring& DBdatabase::selectCastStringType() const { return DBcastStringType; }
const COLstring& DBdatabase::dbStringType() const { return DB_DATATYPE_VARCHAR; }
const COLstring& DBdatabase::dbIntegerType() const { return DBintegerType; }
const COLstring& DBdatabase::dbDoubleType() const { return DBdoubleType; }
const COLstring& DBdatabase::dbDateTimeType() const { return DBdateTimeType; }
const COLstring& DBdatabase::dbTextType() const { return DBtextType; }
const COLstring& DBdatabase::dbBinaryType() const { return DBbinaryType; }

void DBdatabase::streamCreateTableNotNull (COLostream& Stream,const DBsqlCreateTable& SqlCreateTableCommand,COLuint32 ColumnIndex) const{
   COL_METHOD(DBdatabase::streamCreateTableNotNull);
   if ( SqlCreateTableCommand.column( ColumnIndex ).requiredFlag() == true){
      Stream << COL_T(" NOT NULL ");
   }
}

void DBdatabase::streamCreateTableNotNull2(COLostream& Stream,const DBsqlCreateTable& SqlCreateTableCommand,COLuint32 ColumnIndex) const{
   COL_METHOD(DBdatabase::streamCreateTableNotNull2);
}

bool DBdatabase::createTableDefaultConstraintSupported() const { return true; }

void DBdatabase::streamCreateTableForeignKeyPrefix(COLostream& Stream, const DBsqlCreateTable& Command, COLuint32 ColumnIndexi) const {
   COL_METHOD(DBdatabase::streamCreateTableForeignKeyPrefix);
}

void DBdatabase::streamCreateTableForeignKeySuffix (COLostream& Stream, const DBsqlCreateTable& SqlCreateTableCommand, COLuint32 ColumnIndex) const {
   COL_METHOD(DBdatabase::streamCreateTableForeignKeySuffix);
   Stream << COL_T(" ON DELETE ");
   if ( SqlCreateTableCommand.column( ColumnIndex ).foreignKeyOnDeleteAction() == DBsqlCreateTableColumnForeignKey::RESTRICT){
      Stream << COL_T(" RESTRICT ");
   } else if ( SqlCreateTableCommand.column( ColumnIndex ).foreignKeyOnDeleteAction() == DBsqlCreateTableColumnForeignKey::CASCADE){
      Stream << COL_T(" CASCADE ");
   }

   Stream << COL_T(" ON UPDATE ");
   if ( SqlCreateTableCommand.column( ColumnIndex ).foreignKeyOnUpdateAction() == DBsqlCreateTableColumnForeignKey::RESTRICT){
      Stream << COL_T(" RESTRICT ");
   } else if ( SqlCreateTableCommand.column( ColumnIndex ).foreignKeyOnUpdateAction() == DBsqlCreateTableColumnForeignKey::CASCADE){
      Stream << COL_T(" CASCADE ");
   }
}

void DBdatabase::streamCreateTableSuffix(COLostream& Stream, const DBsqlCreateTable& SqlCreateTableCommand) const {
   COL_METHOD(DBdatabase::streamCreateTableSuffix);
}

void DBdatabase::streamInsertUpdateValue ( COLostream& Stream, const DBsqlInsert& SqlCommand, COLuint32 ColumnIndex) const{
   COL_METHOD(DBdatabase::streamInsertUpdateValue);
   addVariantToStream( Stream, SqlCommand.columnValue(ColumnIndex), SqlCommand.columnValueIsColumnFlag(ColumnIndex), SqlCommand.quoteColumnName(ColumnIndex));
}

void DBdatabase::streamDateTime( COLostream& Stream, const COLdateTime& DateTime ) const {
   COL_METHOD(DBdatabase::streamDateTime);
   if ( !DateTime.isNull() ) {
      // Most SQL datetime values may be specified as strings in the format:
      // 'YYYY-MM-DD HH:MM:SS'
      // It is not necessary to specify two digits for date part values 
      // less that are less than 10 because of the date part delimiters.

      // for DB2, it *is* necessary
      // Filemaker is another one that refuses to accept oddly formatted datetimes

      if(databaseType() == DB_ODBC_FILEMAKER) {
         COL_TRC("Using format for DB_ODBC_FILEMAKER");
         // Filemaker 11 refuses to accept even %Y-%m-%d %H:%M:%S -
         // but it accepts {ts '%Y-%m-%d %H:%M:%S'} -
         // ref "ODBC Datetime Format"
         // http://msdn.microsoft.com/en-us/library/ms190234%28SQL.90%29.aspx
         Stream << "{ts '" << DateTime.format("%Y-%m-%d %H:%M:%S") << "'}";
      } else if(databaseType() == DB_ODBC_DB2) {
         COL_TRC("Using format for DB_ODBC_DB2");
         Stream << '\'' << DateTime.format("%Y-%m-%d %H:%M:%S") << '\'';
      } else {
         COL_TRC("Using generic format for most databases");
         Stream << '\'' << DateTime.format("%Y-%m-%d %H:%M:%S") << '\'';
      }
   } else {
      COL_TRC("Datetime is null");
      streamNullValue( Stream );
   }
}

void DBdatabase::streamBinary( COLostream& Stream, const COLstring &Buffer ) const {
   COL_METHOD(DBdatabase::streamBinary);
   if ( Buffer.size() > 0 ) {
      Stream << "CONVERT (binary, 0x";
      DBbinaryToHex(*Stream.sink(),Buffer.c_str(),Buffer.size());
      Stream << ")";
   } else {
      streamNullValue( Stream );
   }
}


void DBdatabase::streamNullValue( COLostream& Stream ) const {
   COL_METHOD(DBdatabase::streamNullValue);
   Stream << COL_T("NULL");
}

void DBdatabase::streamOrderByColumn (COLostream& Stream, const DBsqlSelectOrderBy& OrderByColumn, bool FirstOrderByColumn) const {
   COL_METHOD(DBdatabase::streamOrderByColumn);
   if ( FirstOrderByColumn ) {
      Stream << COL_T(" ORDER BY ");  
   } else {
      Stream << ',';
   }

   streamName( Stream, OrderByColumn.columnName(), OrderByColumn.quoteName() );

   if ( OrderByColumn.descendingFlag() == true ) {
      Stream << COL_T(" DESC ");
   }
}

COLostream& DBdatabase::addVariantToStream (COLostream& Stream, const DBvariant& VariantValue,  bool IsColumnFlag, bool QuoteColumnName ) const {
   COL_METHOD(DBdatabase::addVariantToStream);
   if ( IsColumnFlag == true ) {
      COL_TRC("This is a column flag, streaming its name");
      // Column names are not quoted in a WHERE condition.
      streamName( Stream, VariantValue.toString(), QuoteColumnName );
   } else {
      switch(VariantValue.dataType()) {
      case DB_STRING:
         COL_TRC("DB_STRING");
         addEscapedStringToStream(Stream, VariantValue.toString());
         break;
      case DB_DATETIME:
         COL_TRC("DB_DATETIME");
         streamDateTime(Stream, VariantValue.dateTime());
         break;
      case DB_BINARY:
         COL_TRC("DB_BINARY");
         streamBinary(Stream, VariantValue.binary());
         break;
      // TODO - I assume we don't need for boolean support - Eliot
      case DB_DATA_TYPE_NOT_DEFINED:
         COL_TRC("DB_DATA_TYPE_NOT_DEFINED");
         streamNullValue(Stream);
         break;
      default:
         COL_TRC("default - streaming VariantValue.toString()");
         Stream << VariantValue.toString();
      }
   }

   return Stream;
}

COLostream& operator<<(COLostream& Stream, DBresultSetPtr& ResultSet) {
   return DBprintResultSetPtrToStream(Stream, ResultSet, 0, 0, 0);
}

COLostream&  DBprintResultSetPtrToStream(COLostream& Stream, DBresultSetPtr& ResultSet, const COLuint32 RowOffset, COLuint32 CountOfRows, const COLuint32 MaxSizeOfDBEntry) {
   // TODO: Read the DatabaseEntry and remove newline characters from it
   // TODO: Perhaps this should be re-written to output big DatabaseEntries on multiple lines

   // If RowOffSet and CountOfRows are 0, print all the rows  
   if (  (RowOffset == 0) && (CountOfRows == 0) ) {
      CountOfRows = (*ResultSet).countOfRow();
   }

   // print the DB column headings
   COLstring ColumnName; {
      Stream << newline;
      Stream << COL_T("========================================================================");
      Stream << newline << COL_T("RowIndex\t\t");
      for (COLuint32 ColumnIndex = 0; ColumnIndex < (*ResultSet).countOfColumn(); ColumnIndex++) {
         ColumnName = (*ResultSet).columnName(ColumnIndex);
         Stream << ColumnName << COL_T("\t\t");
      }
      Stream << newline;
      Stream << COL_T("========================================================================");
      Stream << newline;
   }

   // print the rest of the DB
   COLstring DatabaseEntry;
   for (COLuint32 RowIndex = RowOffset; RowIndex < RowOffset + CountOfRows; RowIndex++) {
      Stream << RowIndex << COL_T("\t\t");
      for (COLuint32 ColumnIndex = 0; ColumnIndex < (*ResultSet).countOfColumn(); ColumnIndex++) {
         DatabaseEntry = (*ResultSet).value(RowIndex, ColumnIndex).toString().c_str();
         
         // print out the database entry (truncate if size is larger than MAX_DB_ENTRY_SIZE
         if (  (MaxSizeOfDBEntry == 0)||(DatabaseEntry.size() <= MaxSizeOfDBEntry) ) {         
            Stream << DatabaseEntry << COL_T("\t\t");
         } else {
            Stream << DatabaseEntry.substr(0,MaxSizeOfDBEntry) << COL_T("...") << COL_T("\t\t");
         }         
      }
      Stream << newline;
      Stream << COL_T("---------------------------------------------------------------------");
      Stream << newline;
   }
   return (Stream);
}

void DBshowShortNames(COLostream& Stream) {
   Stream
      << COL_T("oracle_oci  - Oracle via native OCI interface") << newline
      << COL_T("oracle_odbc - Oracle via ODBC interface") << newline
      << COL_T("mysql       - Mysql via native interface") << newline
      << COL_T("mariadb     - MariaDB via native interface") << newline
      << COL_T("postgres    - Postgres SQL via ODBC") << newline
      << COL_T("mysql_odbc  - MySQL via ODBC") << newline
      << COL_T("sybase_asa  - Sybase ASA via ODBC") << newline
      << COL_T("sybase_ase  - Sybase ASE via ODBC") << newline
      << COL_T("access      - Microsoft Access via ODBC") << newline
      << COL_T("sql_server  - Microsoft SQL Server via ODBC") << newline
      << COL_T("db2         - IBM DB2 via ODBC") << newline
      << COL_T("informix    - IBM Informix via ODBC") << newline
      << COL_T("interbase   - Interbase via ODBC") << newline
      << COL_T("filemaker   - Filemaker via ODBC") << newline
      ;

};

COLstring DBtranslateShortNameToFactory(const COLstring& InputNameRaw) {
   COLstring InputName = InputNameRaw;
   InputName.toLowerCase();

   if(InputName == "oracle_oci") { return DBdatabase::DB_DATABASE_OCI_ORACLE_NAME; }
   if(InputName == "oracle_odbc") { return DBdatabase::DB_DATABASE_ODBC_ORACLE_NAME; }
   if(InputName == "postgres") { return DBdatabase::DB_DATABASE_ODBC_POSTGRESQL_NAME; }
   if(InputName == "sybase_asa") { return DBdatabase::DB_DATABASE_ODBC_SYBASE_NAME; }
   if(InputName == "sybase_ase") { return DBdatabase::DB_DATABASE_ODBC_SYBASE_ASE_NAME; }
   if(InputName == "mysql") { return DBdatabase::DB_DATABASE_MY_SQL_NAME; }
   if(InputName == "mysql_odbc") { return DBdatabase::DB_DATABASE_ODBC_MYSQL_NAME; }
   if(InputName == "mariadb") { return DBdatabase::DB_DATABASE_MARIA_DB_NAME; }
   if(InputName == "access") { return DBdatabase::DB_DATABASE_ODBC_ACCESS_NAME; }
   if(InputName == "sql_server") { return DBdatabase::DB_DATABASE_SQL_SERVER_NAME; }
   if(InputName == "interbase") { return DBdatabase::DB_DATABASE_ODBC_INTERBASE_NAME; }
   if(InputName == "informix") { return DBdatabase::DB_DATABASE_ODBC_INFORMIX_NAME; }
   if(InputName == "db2") { return DBdatabase::DB_DATABASE_ODBC_DB2_NAME; }
   if(InputName == "filemaker") { return DBdatabase::DB_DATABASE_ODBC_FILEMAKER_NAME; }
   if(InputName == "sqlite") { return DBdatabase::DB_DATABASE_SQLITE_NAME; }
   return "";
};

bool DBdatabase::supportsDefinePrimaryKey() const {
   return true;
}

bool DBdatabase::supportsDefault() const {
   return true;
}

void DBdatabase::setParameter(const COLstring& Name, const COLstring& Value) {
   // this is a generic mechanism for setting database-specific parameters (or general, if it makes sense)
   // any supported parameters must be intercepted and validated either here or by the derived databases
   COL_ERROR_STREAM(COL_T("Parameter is unsupported: \"") << Name << COL_T("\"=\"") << Value << '\"', COLerror::PreCondition);
}

const COLstring DB_PARAM_USE_UNICODE = "UseUnicode";

const COLstring DB_PARAM_TIMEOUT = "Timeout";

bool DBdatabaseSupportsParam(const COLstring& DbApi, const COLstring& ParamName) {
   // this indicates "official" support, e.g. for use with Iguana GUI -
   // where we say it's supported

   // as opposed to "untested but possibly working" -
   // where parameters may be accepted by the database anyway, but not advertised as such

   if(ParamName == DB_PARAM_USE_UNICODE) {
      return true;
   }
   if(ParamName == DB_PARAM_TIMEOUT) {
      return (DbApi == DBdatabase::DB_DATABASE_SQL_SERVER_NAME) != 0;
   }
   return false;
}
