#ifndef __DB_DRIVER_MY_SQL_H__
#define __DB_DRIVER_MY_SQL_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdriverMySql
//
// Description:
//
// MySQL client driver wrapper
//
// Author: Nicolas Lehman
// Date:   Mon Apr 26 2021 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLdll.h>


// These are structures defined in the MySQL headers, but they change between
// versions, so we treat them as opaque outside of this wrapper.
#define MYSQL        void
#define MYSQL_RES    void

enum mysql_field_types
{

   MYSQL_TYPE_DECIMAL,  MYSQL_TYPE_TINY,
   MYSQL_TYPE_SHORT,    MYSQL_TYPE_LONG,
   MYSQL_TYPE_FLOAT,    MYSQL_TYPE_DOUBLE,
   MYSQL_TYPE_NULL,     MYSQL_TYPE_TIMESTAMP,
   MYSQL_TYPE_LONGLONG, MYSQL_TYPE_INT24,
   MYSQL_TYPE_DATE,     MYSQL_TYPE_TIME,
   MYSQL_TYPE_DATETIME, MYSQL_TYPE_YEAR,
   MYSQL_TYPE_NEWDATE,  MYSQL_TYPE_VARCHAR,
   MYSQL_TYPE_BIT,

   MYSQL_TYPE_NEWDECIMAL =  246,
   MYSQL_TYPE_ENUM =        247,
   MYSQL_TYPE_SET =         248,
   MYSQL_TYPE_TINY_BLOB =   249,
   MYSQL_TYPE_MEDIUM_BLOB = 250,
   MYSQL_TYPE_LONG_BLOB =   251,
   MYSQL_TYPE_BLOB =        252,
   MYSQL_TYPE_VAR_STRING =  253,
   MYSQL_TYPE_STRING =      254,
   MYSQL_TYPE_GEOMETRY =    255
};

// JULIAN TODO the ODBC field types are different mappings 
enum mysql_odbc_field_types
{
};


//Definition below should work for 5.1 onward
//If a newer version of MySQL is misbehaving check their docs to see if this is the right format
struct MYSQL_FIELD {
  char          *name;
  char          *org_name;
  char          *table;
  char          *org_table;
  char          *db;
  char          *catalog;
  char          *def;
  unsigned long length;
  unsigned long max_length;
  unsigned int  name_length;
  unsigned int  org_name_length;
  unsigned int  table_length;
  unsigned int  org_table_length;
  unsigned int  db_length;
  unsigned int  catalog_length;
  unsigned int  def_length;
  unsigned int  flags;
  unsigned int  decimals;
  unsigned int  charsetnr;
  enum mysql_field_types type;
  void *extension;
};

// These should never change, so we define them here to help with the bindings.
typedef char      my_bool;
typedef char**    MYSQL_ROW;
typedef COLuint64 my_ulonglong;

#define NOT_NULL_FLAG  1
#define PRI_KEY_FLAG   2

class DBdriverMySql {
public:
   DBdriverMySql();
   ~DBdriverMySql();

   void loadLibrary(const COLstring& LibraryPath);
   bool isLoaded() { return m_IsLoaded; }
   COLstring getLibraryPath() { return m_LibraryPath; }
   COLstring getLibraryVersionString() { return m_ClientVersionString; }
   COLuint32 getLibraryVersionUint() { return m_ClientVersionUint; }

   const char*    (COL_STD_CALL* mysql_get_client_info)    (void);
   unsigned long  (COL_STD_CALL* mysql_get_client_version) (void);
   void           (COL_STD_CALL* mysql_close)              (MYSQL* mysql);
   void           (COL_STD_CALL* mysql_data_seek)          (MYSQL_RES* res, my_ulonglong offset);
   const char*    (COL_STD_CALL* mysql_error)              (MYSQL* mysql);
   unsigned int   (COL_STD_CALL* mysql_errno)              (MYSQL* mysql);
   MYSQL_FIELD*   (COL_STD_CALL* mysql_fetch_fields)       (MYSQL_RES* res);
   unsigned long* (COL_STD_CALL* mysql_fetch_lengths)      (MYSQL_RES* res);
   MYSQL_ROW      (COL_STD_CALL* mysql_fetch_row)          (MYSQL_RES* res);
   void           (COL_STD_CALL* mysql_free_result)        (MYSQL_RES* res);
   MYSQL*         (COL_STD_CALL* mysql_init)               (MYSQL* mysql);
   MYSQL_RES*     (COL_STD_CALL* mysql_list_dbs)           (MYSQL* mysql, const char* wild);
   MYSQL_RES*     (COL_STD_CALL* mysql_list_tables)        (MYSQL* mysql, const char* wild);
   unsigned int   (COL_STD_CALL* mysql_num_fields)         (MYSQL_RES* res);
   my_ulonglong   (COL_STD_CALL* mysql_num_rows)           (MYSQL_RES* res);
   int            (COL_STD_CALL* mysql_ping)               (MYSQL* mysql);
   int            (COL_STD_CALL* mysql_query)              (MYSQL* mysql, const char* q);
   MYSQL_RES*     (COL_STD_CALL* mysql_store_result)       (MYSQL* mysql);
   int            (COL_STD_CALL* mysql_server_init)        (int argc, char **argv, char **groups);
   void           (COL_STD_CALL* mysql_server_end)         (void);
   my_bool        (COL_STD_CALL* mysql_thread_init)        (void);
   void           (COL_STD_CALL* mysql_thread_end)         (void);
   bool           (COL_STD_CALL* mysql_commit)             (MYSQL *mysql);
   bool           (COL_STD_CALL* mysql_rollback)           (MYSQL *mysql);
   bool           (COL_STD_CALL* mysql_autocommit)         (MYSQL *mysql, bool mode);
   MYSQL_RES*     (COL_STD_CALL* mysql_list_fields)        (MYSQL* mysql, const char* table, const char* wild);

   MYSQL* (COL_STD_CALL* mysql_real_connect) (MYSQL* mysql, const char* host, const char* user, const char* passwd, const char* db, unsigned int port, const char* unix_socket, unsigned long clientflag);

private:
   COLstring   m_LibraryPath;
   bool        m_IsLoaded;
   COLdll      m_MysqlDll;
   COLstring   m_ClientVersionString;
   COLuint32   m_ClientVersionUint;
};


#endif