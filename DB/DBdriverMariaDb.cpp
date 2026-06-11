//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdriverMariaDB
//
// Description:
//
// Implementation
//
// Author: Julian Muir 
// Date:   Mon May 03 2021 
//
//---------------------------------------------------------------------------

#include "DBdriverMariaDb.h"
#include "DBdriverValidate.h"

#include <FIL/FILutils.h>

#include <COL/COLstring.h>
#include <COL/COLdll.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

DBdriverMariaDb::DBdriverMariaDb() {
   COL_METHOD(DBdriverMariaDb::DBdriverMariaDb);
   m_IsLoaded = false;
}

DBdriverMariaDb::~DBdriverMariaDb() {
   COL_METHOD(DBdriverMariaDb::~DBdriverMariaDb);
}

void DBdriverMariaDb::loadLibrary(const COLstring& LibraryPath) {
   COL_METHOD(DBdriverMariaDb::loadLibrary);
   COL_VAR(LibraryPath);
   m_LibraryPath = LibraryPath;

   if( !DBlibValidateAndLoad(&m_MariaDbDll, m_LibraryPath) ){ 
      m_IsLoaded = false;
      return;
   }

   try {
   COL_TRC("Getting function addresses");
   mariadb_get_client_info    = (const char* (*)())                                   m_MariaDbDll.getProcAddress("mysql_get_client_info");
   mariadb_get_client_version = (unsigned long (*)())                                 m_MariaDbDll.getProcAddress("mysql_get_client_version");
   mariadb_close              = (void (*)(MARIADB*))                                  m_MariaDbDll.getProcAddress("mysql_close");
   mariadb_data_seek          = (void (*)(MARIADB_RES*, my_ulonglong))                m_MariaDbDll.getProcAddress("mysql_data_seek");
   mariadb_error              = (const char* (*)(MARIADB*))                           m_MariaDbDll.getProcAddress("mysql_error");
   mariadb_errno              = (unsigned int (*)(MARIADB*))                          m_MariaDbDll.getProcAddress("mysql_errno");
   mariadb_fetch_fields       = (MARIADB_FIELD* (*)(MARIADB_RES*))                    m_MariaDbDll.getProcAddress("mysql_fetch_fields");
   mariadb_fetch_lengths      = (unsigned long* (*)(MARIADB_RES*))                    m_MariaDbDll.getProcAddress("mysql_fetch_lengths");
   mariadb_fetch_row          = (MARIADB_ROW (*)(MARIADB_RES*))                       m_MariaDbDll.getProcAddress("mysql_fetch_row");
   mariadb_free_result        = (void (*)(MARIADB_RES*))                              m_MariaDbDll.getProcAddress("mysql_free_result");
   mariadb_init               = (MARIADB* (*)(MARIADB*))                              m_MariaDbDll.getProcAddress("mysql_init");
   mariadb_list_dbs           = (MARIADB_RES* (*)(MARIADB*, const char*))             m_MariaDbDll.getProcAddress("mysql_list_dbs");
   mariadb_list_tables        = (MARIADB_RES* (*)(MARIADB*, const char*))             m_MariaDbDll.getProcAddress("mysql_list_tables");
   mariadb_num_fields         = (unsigned int (*)(MARIADB_RES*))                      m_MariaDbDll.getProcAddress("mysql_num_fields");
   mariadb_num_rows           = (my_ulonglong (*)(MARIADB_RES*))                      m_MariaDbDll.getProcAddress("mysql_num_rows");
   // mariadb_options            = (int (*)(MARIADB*, enum mariadb_option, const void*)) m_MariaDbDll.getProcAddress("mysql_options");
   mariadb_ping               = (int (*)(MARIADB*))                                   m_MariaDbDll.getProcAddress("mysql_ping");
   mariadb_query              = (int (*)(MARIADB*, const char*))                      m_MariaDbDll.getProcAddress("mysql_query");
   mariadb_store_result       = (MARIADB_RES* (*)(MARIADB*))                          m_MariaDbDll.getProcAddress("mysql_store_result");
   mariadb_server_init        = (int (*)(int, char**, char**))                        m_MariaDbDll.getProcAddress("mysql_server_init");
   mariadb_server_end         = (void (*)())                                          m_MariaDbDll.getProcAddress("mysql_server_end");
   mariadb_thread_init        = (my_bool (*)())                                       m_MariaDbDll.getProcAddress("mysql_thread_init");
   mariadb_thread_end         = (void (*)())                                          m_MariaDbDll.getProcAddress("mysql_thread_end");
   mariadb_commit             = (bool (*)(MARIADB*))                                  m_MariaDbDll.getProcAddress("mysql_commit");
   mariadb_rollback           = (bool (*)(MARIADB*))                                  m_MariaDbDll.getProcAddress("mysql_rollback");
   mariadb_autocommit         = (bool (*)(MARIADB*, bool))                            m_MariaDbDll.getProcAddress("mysql_autocommit");

   mariadb_list_fields        = (MARIADB_RES* (*)(MARIADB*, const char*, const char*))  m_MariaDbDll.getProcAddress("mysql_list_fields");
   mariadb_real_connect       = (MARIADB* (*)(MARIADB*, const char*, const char*, const char*, const char*, unsigned int, const char*, unsigned long)) m_MariaDbDll.getProcAddress("mysql_real_connect");
   } catch (COLerror Error) {
      COL_TRC("The file specified is not a valid MariaDb library. " << Error);
      COL_VAR(Error);
      m_IsLoaded = false;
      return;
   } catch (...) {
      COL_TRC("Something unexpected happened when trying to load the MariaDB library!");
      m_IsLoaded = false;
      return;
   }
   m_IsLoaded = true;
   COL_TRC("Loaded the Library!");

   m_ClientVersionString = COLstring(this->mariadb_get_client_info());
   m_ClientVersionUint   = (COLuint32) this->mariadb_get_client_version();
}