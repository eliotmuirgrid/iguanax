#ifndef __DB_LIB_IMPORT_MARIA_DB_H__
#define __DB_LIB_IMPORT_MARIA_DB_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBlibImportMariaDb
//
// Description:
//
// A thin wrapper for the MariaDB API, customized for each supported shared
// library where necessary.
//
// When adding support for another version of MariaDB, pay particular attention
// to the areas tagged TODO_NEW_MARIADB.
//
// Author: Julian Muir
// Date:   April 27, 2021 
//
//---------------------------------------------------------------------------

// JUlIAN wait for Nic's changes to replace the import mechanism

// JULIAN NO LONGER NEEDED - this is replaced by Nic's driver modules = DBdriverMariaDb.h

// JULIAN this can safely be deleted as a group of 4 files
//        1 - main2/DB/DBlibImportMariaDb.cpp (this file)
//        2 - main2/DB/DBlibImportMariaDb.h   (header for this file)
//        3 - main2/DB/DBdatabaseMariaDb.cpp  (includes DBlibImportMariaDb.h)
//        4 - main2/DB/DBdatabaseMariaDb.h


#include "DBminimumInclude.h"
#include "DBdriverMariaDb.h"


#include <COL/COLdll.h>
#include <COL/COLerror.h>

class COLmutex;

// These are structures defined in the MariaDB headers, but they change between
// versions, so we treat them as opaque outside of this wrapper.
//
#define MARIADB        void
#define MARIADB_RES    void
#define MARIADB_FIELD  void

// These should never change, so we define them here to help with the bindings.
//
// TODO_NEW_MARIADB  Verify that these don't change.
//
typedef char      my_bool;
typedef char**    MARIADB_ROW;
typedef COLuint64 my_ulonglong;

#define NOT_NULL_FLAG  1
#define PRI_KEY_FLAG   2


class DBunsupportedMariaDbError : public COLerror
{
   const COLstring VersionString;

public:
   DBunsupportedMariaDbError(const COLstring& iVersionString)
      : COLerror("Loaded MariaDB library has unrecognized version.", COLerror::PreCondition)
      , VersionString(iVersionString)
   { }

   const COLstring& versionString() const { return VersionString; }
};


// JULIAN comment out to prevent redefinition compile error
// enum DBmariaDbVersion
// {
//    //
//    // TODO_NEW_MARIADB:  Add new values for each major revision of MariaDB.
//    //

//    MARIADB_323,
//    MARIADB_40,
//    MARIADB_50, // includes 4.1
//    MARIADB_60 // includes 5.1 to 5.7, Maria DB 10.0
// };

// JULIAN comment out to prevent redefinition compile error
// enum mariadb_option {  
//   MARIADB_OPT_CONNECT_TIMEOUT,
//   MARIADB_OPT_COMPRESS,
//   MARIADB_OPT_NAMED_PIPE,
//   MARIADB_INIT_COMMAND,
//   MARIADB_READ_DEFAULT_FILE,
//   MARIADB_READ_DEFAULT_GROUP,
//   MARIADB_SET_CHARSET_DIR,
//   MARIADB_SET_CHARSET_NAME,
//   MARIADB_OPT_LOCAL_INFILE,
//   MARIADB_OPT_PROTOCOL,
//   MARIADB_SHARED_MEMORY_BASE_NAME,
//   MARIADB_OPT_READ_TIMEOUT,
//   MARIADB_OPT_WRITE_TIMEOUT,
//   MARIADB_OPT_USE_RESULT,
//   MARIADB_REPORT_DATA_TRUNCATION,
//   MARIADB_OPT_RECONNECT,
//   MARIADB_PLUGIN_DIR,
//   MARIADB_DEFAULT_AUTH,
//   MARIADB_OPT_BIND,
//   MARIADB_OPT_SSL_KEY,
//   MARIADB_OPT_SSL_CERT,
//   MARIADB_OPT_SSL_CA,
//   MARIADB_OPT_SSL_CAPATH,
//   MARIADB_OPT_SSL_CIPHER,
//   MARIADB_OPT_SSL_CRL,
//   MARIADB_OPT_SSL_CRLPATH,
//   MARIADB_OPT_CONNECT_ATTR_RESET,
//   MARIADB_OPT_CONNECT_ATTR_ADD,
//   MARIADB_OPT_CONNECT_ATTR_DELETE,
//   MARIADB_SERVER_PUBLIC_KEY,
//   MARIADB_ENABLE_CLEARTEXT_PLUGIN,
//   MARIADB_OPT_CAN_HANDLE_EXPIRED_PASSWORDS,
//   MARIADB_OPT_MAX_ALLOWED_PACKET,
//   MARIADB_OPT_NET_BUFFER_LENGTH,
//   MARIADB_OPT_TLS_VERSION,
//   MARIADB_OPT_SSL_MODE,
//   MARIADB_OPT_GET_SERVER_PUBLIC_KEY,
//   MARIADB_OPT_RETRY_COUNT,
//   MARIADB_OPT_OPTIONAL_RESULTSET_METADATA,
//   MARIADB_OPT_SSL_FIPS_MODE,
//   MARIADB_OPT_TLS_CIPHERSUITES,
//   MARIADB_OPT_COMPRESSION_ALGORITHMS,
//   MARIADB_OPT_ZSTD_COMPRESSION_LEVEL
// };

// JULIAN caused build error - so commented out (we are probably losing this stuff anyway)
// enum mariadb_ssl_mode {
//   SSL_MODE_#DISABLED = 1,
//   SSL_MODE_PREFERRED,
//   SSL_MODE_REQUIRED,
//   SSL_MODE_VERIFY_CA,
//   SSL_MODE_VERIFY_IDENTITY
// };

class DBmariaDbDll
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

   //DBmariaDbVersion Version;
   COLstring      VersionString;

   my_bool (COL_STD_CALL* commit)     (MARIADB* mariadb);
   my_bool (COL_STD_CALL* rollback)   (MARIADB* mariadb);
   my_bool (COL_STD_CALL* autocommit) (MARIADB* mariadb, my_bool auto_mode);

   void initFunctionPointers();
public:

   DBmariaDbDll(COLdll& iDll);
   ~DBmariaDbDll();

   const COLstring& libraryPath()   const { return Dll.libraryName(); }
   const COLstring& versionString() const { return VersionString; }

   //
   // TODO_NEW_MARIADB:  Verify that these member definitions match up with the function
   // TODO_NEW_MARIADB:  definitions in new MariaDB headers.
   // https://mariadb.com/kb/en/mariadb-connectorc-api-functions/
   //

   static COLmutex& dllSection();

   void           (COL_STD_CALL* mysql_close)         (MARIADB* mariadb);
   void           (COL_STD_CALL* mysql_data_seek)     (MARIADB_RES* res, my_ulonglong offset);
   const char*    (COL_STD_CALL* mysql_error)         (MARIADB* mariadb);
   unsigned int   (COL_STD_CALL* mysql_errno)         (MARIADB* mariadb);
   MARIADB_FIELD* (COL_STD_CALL* mysql_fetch_fields)  (MARIADB_RES* res);
   unsigned long* (COL_STD_CALL* mysql_fetch_lengths) (MARIADB_RES* res);
   MARIADB_ROW    (COL_STD_CALL* mysql_fetch_row)     (MARIADB_RES* res);
   void           (COL_STD_CALL* mysql_free_result)   (MARIADB_RES* res);
   MARIADB*       (COL_STD_CALL* mysql_init)          (MARIADB* mariadb);
   MARIADB_RES*   (COL_STD_CALL* mysql_list_dbs)      (MARIADB* mariadb, const char* wild);
   MARIADB_RES*   (COL_STD_CALL* mysql_list_tables)   (MARIADB* mariadb, const char* wild);
   unsigned int   (COL_STD_CALL* mysql_num_fields)    (MARIADB_RES* res);
   my_ulonglong   (COL_STD_CALL* mysql_num_rows)      (MARIADB_RES* res);
   MARIADB_RES*   (COL_STD_CALL* mysql_ping)          (MARIADB* mariadb);
   int            (COL_STD_CALL* mysql_query)         (MARIADB* mariadb, const char* q);
   MARIADB_RES*   (COL_STD_CALL* mysql_store_result)  (MARIADB* mariadb);
   int            (COL_STD_CALL* mysql_server_init)   (int argc, char **argv, char **groups);
   void           (COL_STD_CALL* mysql_server_end)    (void);
   my_bool        (COL_STD_CALL* mysql_thread_init)   (void);
   void           (COL_STD_CALL* mysql_thread_end)    (void);

   MARIADB*     (COL_STD_CALL* mysql_real_connect) (MARIADB* mariadb, const char* host, const char* user, const char* passwd, const char* db, unsigned int port, const char* unix_socket, unsigned long clientflag);
   MARIADB_RES* (COL_STD_CALL* mysql_list_fields)  (MARIADB* mariadb, const char* table, const char* wild);
   
   //
   // Transactions are supported in MariaDB 4.1 (aka 5.0 beta).
   //

   bool supportsTransactions() const { return commit != NULL; }

   my_bool mysql_commit(MARIADB* mariadb)
   {
      COLPRECONDITION(commit != NULL);
      return commit(mariadb);
   }

   my_bool mysql_rollback(MARIADB* mariadb)
   {
      COLPRECONDITION(rollback != NULL);
      return rollback(mariadb);
   }

   my_bool mysql_autocommit(MARIADB* mariadb, my_bool auto_mode)
   {
      COLPRECONDITION(autocommit != NULL);
      return autocommit(mariadb, auto_mode);
   }

   //
   // These are helpers.
   //

   DBdataType fieldType(MARIADB_FIELD* pField, size_t FieldIndex, bool BinOkay);

   // The brilliant MariaDB designers didn't just append new fields to their structures,
   // they reorganized them and changed their types now and again, so these have to
   // be specialized for each version.
   //
   const char* fieldName  (MARIADB_FIELD* pField, size_t FieldIndex=0);
   const char* fieldDef   (MARIADB_FIELD* pField, size_t FieldIndex=0);
   COLuint32   fieldLength(MARIADB_FIELD* pField, size_t FieldIndex=0);
   COLuint32   fieldFlags (MARIADB_FIELD* pField, size_t FieldIndex=0);

private:
   int  fieldTypeRaw (MARIADB_FIELD* pField, size_t FieldIndex=0);
   bool fieldIsBinary(MARIADB_FIELD* pField, size_t FieldIndex=0);
};


#endif // end of defensive include
