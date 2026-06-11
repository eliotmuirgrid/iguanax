#ifndef __DB_DRIVER_MARIA_DB_H__
#define __DB_DRIVER_MARIA_DB_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdriverMariaDB
//
// Description:
//
// MariaDB client driver wrapper
//
// When adding support for another version of MariaDB
//
// Author: Julian Muir
// Date:   Mon May 03 2021 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLdll.h>


// These are structures defined in the MariaDb headers, but they change between
// versions, so we treat them as opaque outside of this wrapper.
#define MARIADB        void
#define MARIADB_RES    void
#define MARIADB_FIELD  void

// These should never change, so we define them here to help with the bindings.
typedef char      my_bool;
typedef char**    MARIADB_ROW;
typedef COLuint64 my_ulonglong;

#define NOT_NULL_FLAG  1
#define PRI_KEY_FLAG   2

// JULIAN Nic moved the MySQL equivalent to  DBmySqlField -- so reproduce this for MariaDB later
// enum DBmariaDbVersion
// {
//    MARIADB_323,
//    MARIADB_40,
//    MARIADB_50, // includes 4.1
//    MARIADB_60  // includes 5.1 to 5.7, Maria DB 10.0
// };

class DBdriverMariaDb {
public:
   DBdriverMariaDb();
   ~DBdriverMariaDb();

   void loadLibrary(const COLstring& LibraryPath);
   bool isLoaded() { return m_IsLoaded; }
   COLstring getLibraryPath() { return m_LibraryPath; }
   COLstring getLibraryVersionString() { return m_ClientVersionString; }
   COLuint32 getLibraryVersionUint() { return m_ClientVersionUint; }

   const char*    (COL_STD_CALL* mariadb_get_client_info)    (void);
   unsigned long  (COL_STD_CALL* mariadb_get_client_version) (void);
   void           (COL_STD_CALL* mariadb_close)              (MARIADB* mariadb);
   void           (COL_STD_CALL* mariadb_data_seek)          (MARIADB_RES* res, my_ulonglong offset);
   const char*    (COL_STD_CALL* mariadb_error)              (MARIADB* mariadb);
   unsigned int   (COL_STD_CALL* mariadb_errno)              (MARIADB* mariadb);
   MARIADB_FIELD* (COL_STD_CALL* mariadb_fetch_fields)       (MARIADB_RES* res);
   unsigned long* (COL_STD_CALL* mariadb_fetch_lengths)      (MARIADB_RES* res);
   MARIADB_ROW    (COL_STD_CALL* mariadb_fetch_row)          (MARIADB_RES* res);
   void           (COL_STD_CALL* mariadb_free_result)        (MARIADB_RES* res);
   MARIADB*       (COL_STD_CALL* mariadb_init)               (MARIADB* mariadb);
   int            (COL_STD_CALL* mariadb_library_init)       (int argc, char **argv, char **groups);
   MARIADB_RES*   (COL_STD_CALL* mariadb_list_dbs)           (MARIADB* mariadb, const char* wild);
   MARIADB_RES*   (COL_STD_CALL* mariadb_list_tables)        (MARIADB* mariadb, const char* wild);
   unsigned int   (COL_STD_CALL* mariadb_num_fields)         (MARIADB_RES* res);
   my_ulonglong   (COL_STD_CALL* mariadb_num_rows)           (MARIADB_RES* res);
   int            (COL_STD_CALL* mariadb_ping)               (MARIADB* mariadb);
   int            (COL_STD_CALL* mariadb_query)              (MARIADB* mariadb, const char* q);
   MARIADB_RES*   (COL_STD_CALL* mariadb_store_result)       (MARIADB* mariadb);
   int            (COL_STD_CALL* mariadb_server_init)        (int argc, char **argv, char **groups);
   void           (COL_STD_CALL* mariadb_server_end)         (void);
   my_bool        (COL_STD_CALL* mariadb_thread_init)        (void);
   void           (COL_STD_CALL* mariadb_thread_end)         (void);
   bool           (COL_STD_CALL* mariadb_commit)             (MARIADB *mariadb);
   bool           (COL_STD_CALL* mariadb_rollback)           (MARIADB *mariadb);
   bool           (COL_STD_CALL* mariadb_autocommit)         (MARIADB *mariadb, bool mode);

   MARIADB* (COL_STD_CALL* mariadb_real_connect) (MARIADB* mariadb, const char* host, const char* user, const char* passwd, const char* db, unsigned int port, const char* unix_socket, unsigned long clientflag);
   MARIADB_RES* (COL_STD_CALL* mariadb_list_fields) (MARIADB* mariadb, const char* table, const char* wild);

private:
   COLstring   m_LibraryPath;
   COLdll      m_MariaDbDll;
   bool        m_IsLoaded;
   COLstring   m_ClientVersionString;
   COLuint32   m_ClientVersionUint;
};


#endif