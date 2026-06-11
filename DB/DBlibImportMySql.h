#ifndef __DB_LIB_IMPORT_MY_SQL_H__
#define __DB_LIB_IMPORT_MY_SQL_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBlibImportMySql
//
// Description:
//
// A thin wrapper for the MySQL API, customized for each supported shared
// library where necessary.
//
// When adding support for another version of MySQL, pay particular attention
// to the areas tagged TODO_NEW_MYSQL.
//
// Author: Eric Mulvaney
// Date:   March 26, 2008 
//
//---------------------------------------------------------------------------

// JUlIAN wait for Nic's changes to replace the import mechanism

#include "DBminimumInclude.h"

#include "DBdriverMySql.h"

#include <COL/COLdll.h>
#include <COL/COLerror.h>

class COLmutex;

// These are structures defined in the MySQL headers, but they change between
// versions, so we treat them as opaque outside of this wrapper.
//
#define MYSQL        void
#define MYSQL_RES    void
#define MYSQL_FIELD  void

// These should never change, so we define them here to help with the bindings.
//
// TODO_NEW_MYSQL:  Verify that these don't change.
//
typedef char      my_bool;
typedef char**    MYSQL_ROW;
typedef COLuint64 my_ulonglong;

#define NOT_NULL_FLAG  1
#define PRI_KEY_FLAG   2


// JULIAN comment out to prevent redefinition compile error
// enum DBmySqlVersion
// {
//    //
//    // TODO_NEW_MYSQL:  Add new values for each major revision of MySQL.
//    //

//    MYSQL_323,
//    MYSQL_40,
//    MYSQL_50, // includes 4.1
//    MYSQL_60 // includes 5.1 to 5.7, Maria DB 10.0
// };

// JULIAN comment out to prevent redefinition compile error
// enum mysql_option {
//   MYSQL_OPT_CONNECT_TIMEOUT,
//   MYSQL_OPT_COMPRESS,
//   MYSQL_OPT_NAMED_PIPE,
//   MYSQL_INIT_COMMAND,
//   MYSQL_READ_DEFAULT_FILE,
//   MYSQL_READ_DEFAULT_GROUP,
//   MYSQL_SET_CHARSET_DIR,
//   MYSQL_SET_CHARSET_NAME,
//   MYSQL_OPT_LOCAL_INFILE,
//   MYSQL_OPT_PROTOCOL,
//   MYSQL_SHARED_MEMORY_BASE_NAME,
//   MYSQL_OPT_READ_TIMEOUT,
//   MYSQL_OPT_WRITE_TIMEOUT,
//   MYSQL_OPT_USE_RESULT,
//   MYSQL_REPORT_DATA_TRUNCATION,
//   MYSQL_OPT_RECONNECT,
//   MYSQL_PLUGIN_DIR,
//   MYSQL_DEFAULT_AUTH,
//   MYSQL_OPT_BIND,
//   MYSQL_OPT_SSL_KEY,
//   MYSQL_OPT_SSL_CERT,
//   MYSQL_OPT_SSL_CA,
//   MYSQL_OPT_SSL_CAPATH,
//   MYSQL_OPT_SSL_CIPHER,
//   MYSQL_OPT_SSL_CRL,
//   MYSQL_OPT_SSL_CRLPATH,
//   MYSQL_OPT_CONNECT_ATTR_RESET,
//   MYSQL_OPT_CONNECT_ATTR_ADD,
//   MYSQL_OPT_CONNECT_ATTR_DELETE,
//   MYSQL_SERVER_PUBLIC_KEY,
//   MYSQL_ENABLE_CLEARTEXT_PLUGIN,
//   MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS,
//   MYSQL_OPT_MAX_ALLOWED_PACKET,
//   MYSQL_OPT_NET_BUFFER_LENGTH,
//   MYSQL_OPT_TLS_VERSION,
//   MYSQL_OPT_SSL_MODE,
//   MYSQL_OPT_GET_SERVER_PUBLIC_KEY,
//   MYSQL_OPT_RETRY_COUNT,
//   MYSQL_OPT_OPTIONAL_RESULTSET_METADATA,
//   MYSQL_OPT_SSL_FIPS_MODE,
//   MYSQL_OPT_TLS_CIPHERSUITES,
//   MYSQL_OPT_COMPRESSION_ALGORITHMS,
//   MYSQL_OPT_ZSTD_COMPRESSION_LEVEL
// };

// JULIAN comment out to prevent redefinition compile error
// enum mysql_ssl_mode {
//   SSL_MODE_DISABLED = 1,
//   SSL_MODE_PREFERRED,
//   SSL_MODE_REQUIRED,
//   SSL_MODE_VERIFY_CA,
//   SSL_MODE_VERIFY_IDENTITY
// };

class DBmySqlDll
{
private:
   COLdll Dll;

   //templates make this a whole lot easier
   template<class T>
   void assign(T& Function, const char* FunctionName)
   {
      try
      {
         Function = (T)Dll.getProcAddress(FunctionName);
      }
      catch(const COLerror&)
      {
         COL_ERROR_STREAM_PLAIN(COL_T("Could not assign the function '") << FunctionName << COL_T("' from DLL."), COLerror::FileError);
      }
   }

   // DBmySqlVersion Version;
   COLstring      VersionString;

   my_bool (COL_STD_CALL* commit)     (MYSQL* mysql);
   my_bool (COL_STD_CALL* rollback)   (MYSQL* mysql);
   my_bool (COL_STD_CALL* autocommit) (MYSQL* mysql, my_bool auto_mode);

   void initFunctionPointers();
public:

   DBmySqlDll(COLdll& iDll);
   ~DBmySqlDll();

   const COLstring& libraryPath()   const { return Dll.libraryName(); }
   const COLstring& versionString() const { return VersionString; }

   //
   // TODO_NEW_MYSQL:  Verify that these member definitions match up with the function
   // TODO_NEW_MYSQL:  definitions in new MySQL headers.
   //

   static COLmutex& dllSection();

   void         (COL_STD_CALL* mysql_close)           (MYSQL* mysql);
   void         (COL_STD_CALL* mysql_data_seek)       (MYSQL_RES* res, my_ulonglong offset);
   const char*  (COL_STD_CALL* mysql_error)           (MYSQL* mysql);
   unsigned int (COL_STD_CALL* mysql_errno)           (MYSQL* mysql);
   MYSQL_FIELD* (COL_STD_CALL* mysql_fetch_fields)    (MYSQL_RES* res);
   unsigned long*(COL_STD_CALL* mysql_fetch_lengths)  (MYSQL_RES* res);
   MYSQL_ROW    (COL_STD_CALL* mysql_fetch_row)       (MYSQL_RES* res);
   void         (COL_STD_CALL* mysql_free_result)     (MYSQL_RES* res);
   MYSQL*       (COL_STD_CALL* mysql_init)            (MYSQL* mysql);
   MYSQL_RES*   (COL_STD_CALL* mysql_list_dbs)        (MYSQL* mysql, const char* wild);
   MYSQL_RES*   (COL_STD_CALL* mysql_list_tables)     (MYSQL* mysql, const char* wild);
   unsigned int (COL_STD_CALL* mysql_num_fields)      (MYSQL_RES* res);
   my_ulonglong (COL_STD_CALL* mysql_num_rows)        (MYSQL_RES* res);
   // int          (COL_STD_CALL* mysql_options)         (MYSQL *mysql, enum mysql_option option, const void *arg);
   MYSQL_RES*   (COL_STD_CALL* mysql_ping)            (MYSQL* mysql);
   int          (COL_STD_CALL* mysql_query)           (MYSQL* mysql, const char* q);
   MYSQL_RES*   (COL_STD_CALL* mysql_store_result)    (MYSQL* mysql);
   int          (COL_STD_CALL* mysql_server_init)     (int argc, char **argv, char **groups);
   void         (COL_STD_CALL* mysql_server_end)      (void);
   my_bool      (COL_STD_CALL* mysql_thread_init)     (void);
   void         (COL_STD_CALL* mysql_thread_end)      (void);

   MYSQL* (COL_STD_CALL* mysql_real_connect) (MYSQL* mysql, const char* host, const char* user, const char* passwd, const char* db, unsigned int port, const char* unix_socket, unsigned long clientflag);
   MYSQL_RES* (COL_STD_CALL* mysql_list_fields) (MYSQL* mysql, const char* table, const char* wild);
   
   //
   // Transactions are supported in MySQL 4.1 (aka 5.0 beta).
   //

   bool supportsTransactions() const { return commit != NULL; }

   my_bool mysql_commit(MYSQL* mysql)
   {
      COLPRECONDITION(commit != NULL);
      return commit(mysql);
   }

   my_bool mysql_rollback(MYSQL* mysql)
   {
      COLPRECONDITION(rollback != NULL);
      return rollback(mysql);
   }

   my_bool mysql_autocommit(MYSQL* mysql, my_bool auto_mode)
   {
      COLPRECONDITION(autocommit != NULL);
      return autocommit(mysql, auto_mode);
   }

   //
   // These are helpers.
   //

   DBdataType fieldType(MYSQL_FIELD* pField, size_t FieldIndex, bool BinOkay);

   // The brilliant MySQL designers didn't just append new fields to their structures,
   // they reorganized them and changed their types now and again, so these have to
   // be specialized for each version.
   //
   const char* fieldName  (MYSQL_FIELD* pField, size_t FieldIndex=0);
   const char* fieldDef   (MYSQL_FIELD* pField, size_t FieldIndex=0);
   COLuint32   fieldLength(MYSQL_FIELD* pField, size_t FieldIndex=0);
   COLuint32   fieldFlags (MYSQL_FIELD* pField, size_t FieldIndex=0);

private:
   // int  fieldTypeRaw (MYSQL_FIELD* pField, size_t FieldIndex=0);
   bool fieldIsBinary(MYSQL_FIELD* pField, size_t FieldIndex=0);
};


#endif // end of defensive include
