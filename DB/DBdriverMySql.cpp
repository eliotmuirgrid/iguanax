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
//
// Author: Nicolas Lehman
// Date:   Mon Apr 26 2021 
//
//---------------------------------------------------------------------------

#include "DBdriverMySql.h"
#include "DBdriverValidate.h"

#include <FIL/FILutils.h>

#include <COL/COLstring.h>
#include <COL/COLdll.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

DBdriverMySql::DBdriverMySql() : m_LibraryPath(""), m_IsLoaded(false), m_ClientVersionString(""), m_ClientVersionUint(0) {
   COL_METHOD(DBdriverMySql::DBdriverMySql);
}

DBdriverMySql::~DBdriverMySql() {
   COL_METHOD(DBdriverMySql::~DBdriverMySql);
}

void DBdriverMySql::loadLibrary(const COLstring& LibraryPath) {
   COL_METHOD(DBdriverMySql::loadLibrary);
   COL_VAR(LibraryPath);
   m_LibraryPath = LibraryPath;

   if( !DBlibValidateAndLoad(&m_MysqlDll, m_LibraryPath) ){ 
      m_IsLoaded = false;
      return;
   }

   try {
   COL_TRC("Getting function addresses");
   mysql_get_client_info    = (const char* (*)())                                 m_MysqlDll.getProcAddress("mysql_get_client_info");
   mysql_get_client_version = (unsigned long (*)())                               m_MysqlDll.getProcAddress("mysql_get_client_version");
   mysql_close              = (void (*)(MYSQL*))                                  m_MysqlDll.getProcAddress("mysql_close");
   mysql_data_seek          = (void (*)(MYSQL_RES*, my_ulonglong))                m_MysqlDll.getProcAddress("mysql_data_seek");
   mysql_error              = (const char* (*)(MYSQL*))                           m_MysqlDll.getProcAddress("mysql_error");
   mysql_errno              = (unsigned int (*)(MYSQL*))                          m_MysqlDll.getProcAddress("mysql_errno");
   mysql_fetch_fields       = (MYSQL_FIELD* (*)(MYSQL_RES*))                      m_MysqlDll.getProcAddress("mysql_fetch_fields");
   mysql_fetch_lengths      = (unsigned long* (*)(MYSQL_RES*))                    m_MysqlDll.getProcAddress("mysql_fetch_lengths");
   mysql_fetch_row          = (MYSQL_ROW (*)(MYSQL_RES*))                         m_MysqlDll.getProcAddress("mysql_fetch_row");
   mysql_free_result        = (void (*)(MYSQL_RES*))                              m_MysqlDll.getProcAddress("mysql_free_result");
   mysql_init               = (MYSQL* (*)(MYSQL*))                                m_MysqlDll.getProcAddress("mysql_init");
   mysql_list_dbs           = (MYSQL_RES* (*)(MYSQL*, const char*))               m_MysqlDll.getProcAddress("mysql_list_dbs");
   mysql_list_tables        = (MYSQL_RES* (*)(MYSQL*, const char*))               m_MysqlDll.getProcAddress("mysql_list_tables");
   mysql_num_fields         = (unsigned int (*)(MYSQL_RES*))                      m_MysqlDll.getProcAddress("mysql_num_fields");
   mysql_num_rows           = (my_ulonglong (*)(MYSQL_RES*))                      m_MysqlDll.getProcAddress("mysql_num_rows");
   mysql_ping               = (int (*)(MYSQL*))                                   m_MysqlDll.getProcAddress("mysql_ping");
   mysql_query              = (int (*)(MYSQL*, const char*))                      m_MysqlDll.getProcAddress("mysql_query");
   mysql_store_result       = (MYSQL_RES* (*)(MYSQL*))                            m_MysqlDll.getProcAddress("mysql_store_result");
   mysql_server_init        = (int (*)(int, char**, char**))                      m_MysqlDll.getProcAddress("mysql_server_init");
   mysql_server_end         = (void (*)())                                        m_MysqlDll.getProcAddress("mysql_server_end");
   mysql_thread_init        = (my_bool (*)())                                     m_MysqlDll.getProcAddress("mysql_thread_init");
   mysql_thread_end         = (void (*)())                                        m_MysqlDll.getProcAddress("mysql_thread_end");
   mysql_commit             = (bool (*)(MYSQL*))                                  m_MysqlDll.getProcAddress("mysql_commit");
   mysql_rollback           = (bool (*)(MYSQL*))                                  m_MysqlDll.getProcAddress("mysql_rollback");
   mysql_autocommit         = (bool (*)(MYSQL*, bool))                            m_MysqlDll.getProcAddress("mysql_autocommit");

   mysql_list_fields        = (MYSQL_RES* (*)(MYSQL*, const char*, const char*))  m_MysqlDll.getProcAddress("mysql_list_fields");
   mysql_real_connect       = (MYSQL* (*)(MYSQL*, const char*, const char*, const char*, const char*, unsigned int, const char*, unsigned long)) m_MysqlDll.getProcAddress("mysql_real_connect");
   } catch (COLerror Error) {
   COL_TRC("The file specified is a valid MySQL library." << Error);
   COL_VAR(Error);
      m_IsLoaded = false;
      return;
   } catch(...){
      COL_TRC("Something unexpected happened when trying to load the MySQL library!");
      return;
   }
   m_IsLoaded = true;
   COL_TRC("Loaded the Library!");

   m_ClientVersionString = COLstring(this->mysql_get_client_info());
   m_ClientVersionUint   = (COLuint32) this->mysql_get_client_version();
}
