//---------------------------------------------------------------------------
// Copyright (C) 2008-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseMySqlPrivate
//
// Description:
//
// Implementation
//
// Author:   Eric Mulvaney
// Date:     April 1, 2008 
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <COL/COLlocker.h>
#include <COL/COLmutex.h>

#include "DBdatabaseMySqlPrivate.h"


static DBmySqlVersion getMySqlDllVersion(COLdll& Dll, COLstring& VersionString)
{
   COL_FUNCTION(getMySqlDllVersion);
   // Yes, MySQL 4.0 and 4.1 have a better function called mysql_get_client_version,
   // but it was only added in MySQL 4.0.17
   COLdllFcnPtr pDllFcn = Dll.getProcAddress("mysql_get_client_info");
   VersionString = ( *(const char* (COL_STD_CALL*)(void))pDllFcn )();
   COL_VAR(VersionString);
   COL_TRC("Checking found version against supported MySql versions...");
   return MYSQL_60;
}

// note, this is leaked on purpose, since we don't know who else may need it
// during shutdown
static COLmutex *const pDbMySqlDllSection = new COLmutex;

COLmutex& DBmySqlDll::dllSection()
{
   return *pDbMySqlDllSection;
}

DBmySqlDll::DBmySqlDll(COLdll* pDll){
   COL_METHOD(DBmySqlDll::DBmySqlDll);
   m_pDll = pDll; // pDll is already loaded from the DBdatabaseMySql::initialize logic, we just want to take ownership
   COLlocker Lock(dllSection());
   Version = getMySqlDllVersion(*pDll, VersionString);
   COL_VAR(VersionString);
   initFunctionPointers();
   //initialize the library
   COL_TRC("Initalizing Library");
   if (mysql_server_init(0,NULL,NULL)){
      COL_ERROR_STREAM("Could not initialize MySQL library.", COLerror::FileError);
   }
}

DBmySqlDll::~DBmySqlDll(){
   COL_METHOD(DBmySqlDll::~DBmySqlDll);
   delete m_pDll;
}

void DBmySqlDll::initFunctionPointers()
{
   COL_METHOD(DBmySqlDll::initFunctionPointers);
   COL_TRC("Assigning MySql functions");
   assign(mysql_close, "mysql_close");
   assign(mysql_data_seek, "mysql_data_seek");
   assign(mysql_error, "mysql_error");
   assign(mysql_fetch_fields, "mysql_fetch_fields");
   assign(mysql_fetch_lengths, "mysql_fetch_lengths");
   assign(mysql_fetch_row, "mysql_fetch_row");
   assign(mysql_free_result, "mysql_free_result");
   assign(mysql_init, "mysql_init");
   assign(mysql_list_dbs, "mysql_list_dbs");
   assign(mysql_list_tables, "mysql_list_tables");
   assign(mysql_num_fields, "mysql_num_fields");
   assign(mysql_num_rows, "mysql_num_rows");
   assign(mysql_ping, "mysql_ping");
   assign(mysql_query, "mysql_query");
   assign(mysql_errno, "mysql_errno");
   assign(mysql_store_result, "mysql_store_result");
   
   try
   {
      // Although these are deprecated, they'll probably work
      // for some time.  They were just renamed, so we'll
      // try the new names if these are no longer valid.

      assign(mysql_server_init,"mysql_server_init");
      assign(mysql_server_end, "mysql_server_end");
   }
   catch(const COLerror&)
   {
      COL_INF("Using mysql_library_init/_end.");
      
      assign(mysql_server_init,"mysql_library_init");
      assign(mysql_server_end, "mysql_library_end");
   }

   assign(mysql_thread_init,"mysql_thread_init");
   assign(mysql_thread_end,"mysql_thread_end");

   try
   {
      assign(commit, "mysql_commit");
      assign(rollback, "mysql_rollback");
      assign(autocommit, "mysql_autocommit");
   }
   catch(const COLerror&)
   {
      COL_INF("Transactions are not supported.");

      commit = NULL;
      rollback = NULL;
      autocommit = NULL;
   }

   assign(mysql_real_connect, "mysql_real_connect");
   assign(mysql_list_fields, "mysql_list_fields");
}


//
// Mapping MySQL field types to DBdataType
//

enum mysql_field_types  // From MySQL 6.0.4 (consistent with previous versions).
{
   //
   // TODO_NEW_MYSQL: Verify with new MySQL headers; append new items.
   //

   MYSQL_TYPE_DECIMAL,  MYSQL_TYPE_TINY,
   MYSQL_TYPE_SHORT,    MYSQL_TYPE_LONG,
   MYSQL_TYPE_FLOAT,    MYSQL_TYPE_DOUBLE,
   MYSQL_TYPE_NULL,     MYSQL_TYPE_TIMESTAMP,
   MYSQL_TYPE_LONGLONG, MYSQL_TYPE_INT24,
   MYSQL_TYPE_DATE,     MYSQL_TYPE_TIME,
   MYSQL_TYPE_DATETIME, MYSQL_TYPE_YEAR,
   MYSQL_TYPE_NEWDATE,  MYSQL_TYPE_VARCHAR,
   MYSQL_TYPE_BIT,

   MYSQL_TYPE_NEWDECIMAL=246,
   MYSQL_TYPE_ENUM=247,
   MYSQL_TYPE_SET=248,
   MYSQL_TYPE_TINY_BLOB=249,
   MYSQL_TYPE_MEDIUM_BLOB=250,
   MYSQL_TYPE_LONG_BLOB=251,
   MYSQL_TYPE_BLOB=252,
   MYSQL_TYPE_VAR_STRING=253,
   MYSQL_TYPE_STRING=254,
   MYSQL_TYPE_GEOMETRY=255
};

DBdataType DBmySqlDll::fieldType(MYSQL_FIELD* pField, size_t FieldIndex, bool BinOkay)
{
   mysql_field_types MySqlType = (mysql_field_types)fieldTypeRaw(pField, FieldIndex);

   switch ( MySqlType )
   {
      case MYSQL_TYPE_STRING:
      case MYSQL_TYPE_VAR_STRING:
      case MYSQL_TYPE_BLOB:
         return (BinOkay && fieldIsBinary(pField,FieldIndex)) ? DB_BINARY : DB_STRING;

      case MYSQL_TYPE_LONG:
      case MYSQL_TYPE_INT24:
         return DB_INTEGER;

      case MYSQL_TYPE_DOUBLE:
         return DB_LARGE_DOUBLE;

      case MYSQL_TYPE_TIMESTAMP:
      case MYSQL_TYPE_DATE:
      case MYSQL_TYPE_DATETIME:
         return DB_DATETIME;

      case MYSQL_TYPE_YEAR:  // YEAR(2) and YEAR(4)
      case MYSQL_TYPE_TINY:
      case MYSQL_TYPE_SHORT:
         return DB_INTEGER;

      case MYSQL_TYPE_LONGLONG:
         return DB_LARGE_INTEGER;

      case MYSQL_TYPE_FLOAT:
         return DB_DOUBLE;

      case MYSQL_TYPE_DECIMAL:
      case MYSQL_TYPE_NEWDECIMAL:
      case MYSQL_TYPE_TIME:  // HH:MM:SS won't parse as a date/time.
         return DB_STRING;

      case MYSQL_TYPE_BIT:
         return BinOkay ? DB_BINARY : DB_STRING;

      case MYSQL_TYPE_NULL:
         // The NULL type is only used for untyped expression of NULL.
         // We have to choose a type to hold it in the table, however.
         return DB_INTEGER;  // It's always NULL.

      default:
         COL_ERROR_STREAM( COL_T("MySQL data type: ") << MySqlType << newline << COL_T("not supported."), COLerror::PreCondition );
   }
}


//
// Field accessors for each version.
//

struct mysql_field_323  // From MySQL 3.23
{
  char *name;
  char *table;
  char *def;
  enum mysql_field_types type;
  unsigned int length;
  unsigned int max_length;
  unsigned int flags;
  unsigned int decimals;
};

struct mysql_field_40  // From MySQL 4.0
{
  char *name;
  char *table;
  char *org_table;
  char *db;
  char *def;
  unsigned long length;
  unsigned long max_length;
  unsigned int flags;
  unsigned int decimals;
  enum mysql_field_types type;
};

struct mysql_field_50  // From MySQL 5.0 (compatible with 4.1+).
{
  char *name;
  char *org_name;
  char *table;
  char *org_table;
  char *db;
  char *catalog;
  char *def;
  unsigned long length;
  unsigned long max_length;
  unsigned int name_length;
  unsigned int org_name_length;
  unsigned int table_length;
  unsigned int org_table_length;
  unsigned int db_length;
  unsigned int catalog_length;
  unsigned int def_length;
  unsigned int flags;
  unsigned int decimals;
  unsigned int charsetnr;
  enum mysql_field_types type;
};

struct mysql_field_60  // From 6.0.4 (compatible with 5.1+)
{
  char *name;
  char *org_name;
  char *table;
  char *org_table;
  char *db;
  char *catalog;
  char *def;
  unsigned long length;
  unsigned long max_length;
  unsigned int name_length;
  unsigned int org_name_length;
  unsigned int table_length;
  unsigned int org_table_length;
  unsigned int db_length;
  unsigned int catalog_length;
  unsigned int def_length;
  unsigned int flags;
  unsigned int decimals;
  unsigned int charsetnr;
  enum mysql_field_types type;
  void *extension;
};

//
// TODO_NEW_MYSQL:  Add newer mysql_field_XX types above, if necessary, and
// TODO_NEW_MYSQL:  add cases below for each new DBmySqlVersion.  If the structure
// TODO_NEW_MYSQL:  doesn't change between major versions, a typedef will suffice.
// TODO_NEW_MYSQL:  Pay particular attention to member position and size--and type,
// TODO_NEW_MYSQL:  if we use it--remembering that void* can be used in place of any
// TODO_NEW_MYSQL:  other pointer type (these are from C land, not C++).
//

const char* DBmySqlDll::fieldName(MYSQL_FIELD* pField, size_t FieldIndex)
{
   switch(Version)
   {
   case MYSQL_323:  return ((mysql_field_323*)pField)[FieldIndex].name;
   case MYSQL_40:   return ((mysql_field_40 *)pField)[FieldIndex].name;
   case MYSQL_50:   return ((mysql_field_50 *)pField)[FieldIndex].name;
   case MYSQL_60:   return ((mysql_field_60 *)pField)[FieldIndex].name;
   default:
      COL_ERROR_STREAM(COL_T("No case for MySQL v") << VersionString, COLerror::PreCondition);
   }
}

const char* DBmySqlDll::fieldDef(MYSQL_FIELD* pField, size_t FieldIndex)
{
   switch(Version)
   {
   case MYSQL_323:  return ((mysql_field_323*)pField)[FieldIndex].def;
   case MYSQL_40:   return ((mysql_field_40 *)pField)[FieldIndex].def;
   case MYSQL_50:   return ((mysql_field_50 *)pField)[FieldIndex].def;
   case MYSQL_60:   return ((mysql_field_60 *)pField)[FieldIndex].def;
   default:
      COL_ERROR_STREAM(COL_T("No case for MySQL v") << VersionString, COLerror::PreCondition);
   }
}

COLuint32 DBmySqlDll::fieldLength(MYSQL_FIELD* pField, size_t FieldIndex)
{
   switch(Version)
   {
   case MYSQL_323:  return ((mysql_field_323*)pField)[FieldIndex].length;
   case MYSQL_40:   return ((mysql_field_40 *)pField)[FieldIndex].length;
   case MYSQL_50:   return ((mysql_field_50 *)pField)[FieldIndex].length;
   case MYSQL_60:   return ((mysql_field_60 *)pField)[FieldIndex].length;
   default:
      COL_ERROR_STREAM(COL_T("No case for MySQL v") << VersionString, COLerror::PreCondition);
   }
}

COLuint32 DBmySqlDll::fieldFlags(MYSQL_FIELD* pField, size_t FieldIndex)
{
   switch(Version)
   {
   case MYSQL_323:  return ((mysql_field_323*)pField)[FieldIndex].flags;
   case MYSQL_40:   return ((mysql_field_40 *)pField)[FieldIndex].flags;
   case MYSQL_50:   return ((mysql_field_50 *)pField)[FieldIndex].flags;
   case MYSQL_60:   return ((mysql_field_60 *)pField)[FieldIndex].flags;
   default:
      COL_ERROR_STREAM(COL_T("No case for MySQL v") << VersionString, COLerror::PreCondition);
   }
}

int DBmySqlDll::fieldTypeRaw(MYSQL_FIELD* pField, size_t FieldIndex)
{
   switch(Version)
   {
   case MYSQL_323:  return ((mysql_field_323*)pField)[FieldIndex].type;
   case MYSQL_40:   return ((mysql_field_40 *)pField)[FieldIndex].type;
   case MYSQL_50:   return ((mysql_field_50 *)pField)[FieldIndex].type;
   case MYSQL_60:   return ((mysql_field_60 *)pField)[FieldIndex].type;
   default:
      COL_ERROR_STREAM(COL_T("No case for MySQL v") << VersionString, COLerror::PreCondition);
   }
}

bool DBmySqlDll::fieldIsBinary(MYSQL_FIELD* pField, size_t FieldIndex)
{
   switch(Version)
   {
   case MYSQL_323:  return false; //((mysql_field_323*)pField)[FieldIndex].flags & BINARY_FLAG;
   case MYSQL_40:   return false; //((mysql_field_40 *)pField)[FieldIndex].flags & BINARY_FLAG;
   case MYSQL_50:   return ((mysql_field_50 *)pField)[FieldIndex].charsetnr == 63;
   case MYSQL_60:   return ((mysql_field_60 *)pField)[FieldIndex].charsetnr == 63;
   default:
      COL_ERROR_STREAM(COL_T("No case for MySQL v") << VersionString, COLerror::PreCondition);
   }
}
