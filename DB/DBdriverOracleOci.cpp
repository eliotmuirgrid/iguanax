//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdriverOracleOci
//
// Description:
//
// Implementation
//
// Author: Julian Muir 
// Date:   Mon May 11 2021 
//
//---------------------------------------------------------------------------

#include "DBdriverOracleOci.h"
#include "DBdriverValidate.h"

#include <FIL/FILutils.h>

#include <COL/COLstring.h>
#include <COL/COLdll.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

DBdriverOracleOci::DBdriverOracleOci() {
   COL_METHOD(DBdriverOracleOci::DBdriverOracleOci);
   m_IsLoaded = false;
}

DBdriverOracleOci::~DBdriverOracleOci() {
   COL_METHOD(DBdriverOracleOci::~DBdriverOracleOci);
}

void DBdriverOracleOci::loadLibrary(const COLstring& LibraryPath) {
   COL_METHOD(DBdriverOracleOci::loadLibrary);
   COL_VAR(LibraryPath);
   m_LibraryPath = LibraryPath;

   if( !DBlibValidateAndLoad(&m_OracleOciDll, m_LibraryPath) ){ 
      m_IsLoaded = false;
      return;
   }

   try {
   COL_TRC("Getting function addresses");
   oci_attr_get          = (sword (*) (const dvoid*, ub4, dvoid*, ub4*, ub4, OCIError*))                                                                                                     m_OracleOciDll.getProcAddress("OCIAttrGet");
   oci_attr_set          = (sword (*) (dvoid*, ub4, dvoid*, ub4, ub4, OCIError*))                                                                                                            m_OracleOciDll.getProcAddress("OCIAttrSet");
   oci_bind_by_name      = (sword (*) (OCIStmt*, OCIBind**, OCIError*, const OraText*, sb4, dvoid*, sb4, ub2, dvoid*, ub2*, ub2*, ub4, ub4*, ub4))                                           m_OracleOciDll.getProcAddress("OCIBindByName");
   // JULIAN for some reason it does not recognize the sb8 definition in oratypes.h 
   // oci_bind_by_name2     = (sword (*) (OCIStmt*, OCIBind**, OCIError*, const OraText*, sb4, dvoid*, sb8, ub2, dvoid*, ub4*, ub2*, ub4, ub4*, ub4))                                           m_OracleOciDll.getProcAddress("OCIBindByName2");
   oci_define_by_pos     = (sword (*) (OCIStmt*, OCIDefine**, OCIError*, ub4, dvoid*, sb4, ub2, dvoid*, ub2*, ub2*, ub4))                                                                    m_OracleOciDll.getProcAddress("OCIDefineByPos");
   oci_define_dynamic    = (sword (*) (OCIDefine*, OCIError*, dvoid*, sb4 (*)(dvoid*, OCIDefine*, ub4, dvoid**, ub4**, ub1*, dvoid**, ub2**)))                                               m_OracleOciDll.getProcAddress("OCIDefineDynamic");
   oci_descriptor_alloc  = (sword (*) (const dvoid*, dvoid**, ub4, size_t, dvoid**))                                                                                                         m_OracleOciDll.getProcAddress("OCIDescriptorAlloc");
   oci_descriptor_free   = (sword (*) (dvoid *, ub4))                                                                                                                                        m_OracleOciDll.getProcAddress("OCIDescriptorFree");
   oci_env_nls_create    = (sword (*) (OCIEnv**, ub4, dvoid*, dvoid * (*)(dvoid *, size_t ), dvoid* (*)(dvoid *, dvoid *, size_t ), void (*)(dvoid *, dvoid *), size_t, dvoid **, ub2, ub2)) m_OracleOciDll.getProcAddress("OCIEnvNlsCreate");
   oci_error_get         = (sword (*) (dvoid*, ub4, OraText*, sb4*, OraText*, ub4, ub4))                                                                                                     m_OracleOciDll.getProcAddress("OCIErrorGet");
   oci_handle_alloc      = (sword (*) (const dvoid*, dvoid**, ub4, size_t, dvoid**))                                                                                                         m_OracleOciDll.getProcAddress("OCIHandleAlloc");
   oci_handle_free       = (sword (*) (dvoid*, ub4))                                                                                                                                         m_OracleOciDll.getProcAddress("OCIHandleFree");
   oci_lob_char_set_form = (sword (*) (OCIEnv*, OCIError*, const OCILobLocator*, ub1*))                                                                                                      m_OracleOciDll.getProcAddress("OCILobCharSetForm");
   oci_lob_read_2        = (sword (*) (OCISvcCtx*, OCIError*, OCILobLocator*, ub4*, ub4, dvoid*, ub4, dvoid*, OCICallbackLobRead, ub2, ub1))                                                 m_OracleOciDll.getProcAddress("OCILobRead2");
   oci_logoff            = (sword (*) (OCISvcCtx*, OCIError*))                                                                                                                               m_OracleOciDll.getProcAddress("OCILogoff");
   oci_logon             = (sword (*) (OCIEnv*, OCIError*, OCISvcCtx**, const OraText*, ub4, const OraText*, ub4, const OraText*, ub4))                                                      m_OracleOciDll.getProcAddress("OCILogon");
   oci_param_get         = (sword (*) (const dvoid*, ub4, OCIError*, dvoid**, ub4))                                                                                                          m_OracleOciDll.getProcAddress("OCIParamGet");
   oci_stmt_execute      = (sword (*) (OCISvcCtx*, OCIStmt*, OCIError*, ub4, ub4, const OCISnapshot*, OCISnapshot*, ub4))                                                                    m_OracleOciDll.getProcAddress("OCIStmtExecute");
   oci_stmt_fetch_2      = (sword (*) (OCIStmt*, OCIError*, ub4, ub2, sb4, ub4))                                                                                                             m_OracleOciDll.getProcAddress("OCIStmtFetch2");
   oci_stmt_prepare_2    = (sword (*) (OCISvcCtx*, OCIStmt**, OCIError*, const OraText*, ub4, const OraText*, ub4, ub4, ub4))                                                                m_OracleOciDll.getProcAddress("OCIStmtPrepare2");
   oci_trans_commit      = (sword (*) (OCISvcCtx*, OCIError*, ub4))                                                                                                                          m_OracleOciDll.getProcAddress("OCITransCommit");
   oci_trans_rollback    = (sword (*) (OCISvcCtx*, OCIError*, ub4))                                                                                                                          m_OracleOciDll.getProcAddress("OCITransRollback");
   // ------ never used ------
   // oci_describe_any      = (sword (*) (OCISvcCtx*, OCIError*, dvoid*, ub4, ub1, ub1, ub1, OCIDescribe*))                                                                                     m_OracleOciDll.getProcAddress("OCIDescribeAny");
   // oci_server_attach     = (sword (*) (OCIServer*, OCIError*, CONST OraText*, sb4, ub4))                                                                                                     m_OracleOciDll.getProcAddress("OCIServerAttach");
   // oci_session_begin     = (sword (*) (OCISvcCtx*, OCIError*, OCISession*, ub4, ub4))                                                                                                        m_OracleOciDll.getProcAddress("OCISessionBegin");
   // oci_session_end       = (sword (*) (OCISvcCtx *, OCIError*, OCISession*, ub4))                                                                                                            m_OracleOciDll.getProcAddress("OCISessionEnd");
   // ------ never used + deprecated ------
   // oci_lob_get_length    = (sword (*) (OCISvcCtx*, OCIError*, OCILobLocator*, ub4*))                                                                                                         m_OracleOciDll.getProcAddress("OCILobGetLength");

   // ------ oci_initialize, oci_env_init deprecated - use oci_env_nls_create instead ------
   // oci_initialize        = (sword (*) (ub4, dvoid*, dvoid* (*)(dvoid*, size_t ), dvoid* (*)(dvoid*, dvoid*, size_t ), void (*)(dvoid*, dvoid*)))                                             m_OracleOciDll.getProcAddress("OCIInitialize");
   // oci_env_init          = (sword (*) (OCIEnv**, ub4, size_t, dvoid**))                                                                                                                      m_OracleOciDll.getProcAddress("OCIEnvInit");
   // ------ deprecated use <function>_2 instead ------
   // oci_lob_read          = (sword (*) (OCISvcCtx*, OCIError*, OCILobLocator*, ub4*, ub4, dvoid*, ub4, dvoid*, OCICallbackLobRead, ub2, ub1))                                                 m_OracleOciDll.getProcAddress("OCILobRead");
   oci_stmt_prepare      = (sword (*) (OCIStmt*, OCIError*, CONST OraText*, ub4, ub4, ub4))                                                                                                  m_OracleOciDll.getProcAddress("OCIStmtPrepare");
} catch (COLerror Error) {
      COL_TRC("The file specified is not a valid Oracle Oci library. " << Error);
      COL_VAR(Error);
      m_IsLoaded = false;
      return;
   } catch (...) {
      COL_TRC("Something unexpected happened when trying to load the Oracle Oci library!");
      m_IsLoaded = false;
      return;
   }
   m_IsLoaded = true;
   COL_TRC("Loaded the Oracle Oci Library!");

   sword Return = this->oci_client_version(NULL, NULL, NULL, NULL, NULL);
   COL_VAR(Return);
   m_ClientVersionString = COLstring(Return, 5);
   m_ClientVersionUint   = (COLuint32) Return;
}