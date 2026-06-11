#ifndef __DB_DRIVER_ORACLE_OCI_H__
#define __DB_DRIVER_ORACLE_OCI_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdriverOracleOci
//
// Description:
//
// OracleOci client driver wrapper
////
// Author: Julian Muir
// Date:   Mon May 11 2021 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLdll.h>


#include <DB/OCIoracleInclude/oci.h>
#include <stdlib.h>

// JULIAN TODO the OCI & ODBC field types use different mappings 
// https://docs.oracle.com/cd/B19306_01/appdev.102/b14250/oci03typ.htm
// mapping from ocidfn.h (main2/DB/OCIoracleInclude/ocidfn.h)
enum oci_field_types {
   // Most types were tested using an Oracle table some numbers returned 
   // did not match Oracle docs and were changed to match the actual
   // numbers returned and marked (oracle docs invalid)
   _DUMMY_ZERO_UNUSED_,
   _SQLT_CHR,   _SQLT_NUM,   _SQLT_INT,
   _SQLT_FLT,   _SQLT_STR,   _SQLT_VNU,
   _SQLT_PDN,   _SQLT_LNG,   _SQLT_VCS,
   _SQLT_NON,   _SQLT_RID,   _SQLT_DAT,
   _SQLT_VBI    = 15,        _SQLT_BIN    = 23,
   _SQLT_LBI    = 24,        _SQLT_UIN    = 68,
   _SQLT_SLS    = 91,        _SQLT_LVC    = 94,
   _SQLT_LVB    = 95,        _SQLT_AFC    = 96,
   _SQLT_AVC    = 97,        _SQLT_CUR    = 102,
   _SQLT_RDD    = 104,       _SQLT_LAB    = 105,
   _SQLT_OSL    = 106,       _SQLT_NTY    = 108,
   _SQLT_REF    = 110,       _SQLT_CLOB   = 112,
   _SQLT_BLOB   = 113,       _SQLT_BFILEE = 114,
   _SQLT_CFILEE = 115,       _SQLT_RSET   = 116,
   _SQLT_NCO    = 122,       _SQLT_VST    = 155,
   _SQLT_ODT                              = 156,
   _SQLT_DATE                             = 184,
   _SQLT_TIME                             = 185,
   _SQLT_TIME_TZ                          = 186,
   _SQLT_TIMESTAMP                        = 187,
   _SQLT_TIMESTAMP_TZ                     = 188,
   _SQLT_INTERVAL_YM                      = 189,
   _SQLT_INTERVAL_DS                      = 190,
   _SQLT_TIMESTAMP_LTZ                    = 232,
   _SQLT_PNTY                             = 241,
   // on Oracle web page but not in the ocidfn.h header file :(
   _SQLT_BFLOAT                           = 100, // 21, (oracle docs invalid)
   _SQLT_BDOUBLE                          = 101  // 22  (oracle docs invalid)
};

// JULIAN TODO the ODBC field types are different mappings 
// no obvious source for this found yet... more research
enum oracle_odbc_field_types
{
};


class DBdriverOracleOci {
public:
   DBdriverOracleOci();
   ~DBdriverOracleOci();

   void loadLibrary(const COLstring& LibraryPath);
   bool isLoaded() { return m_IsLoaded; }
   COLstring getLibraryPath() { return m_LibraryPath; }
   COLstring getLibraryVersionString() { return m_ClientVersionString; } // JULIAN ???
   COLuint32 getLibraryVersionUint() { return m_ClientVersionUint; }     // JULIAN ???
   // JULIAN I DON'T UNDERSTAND WHAT THIS IS ABOUT...
   //this has to be loaded every time, as the thrown exception from the getFunctionAddress indicates that the oracle version is 10.1 or older
   sword oci_client_version(sword* pMajor, sword* pMinor, sword* pUpdate, sword* pPatch, sword* pPortUpdate){
      COLdllFcnPtr pDllFcn = m_OracleOciDll.getProcAddress("OCIClientVersion");
      return (*(sword (COL_C_DECL*)(sword*,sword*,sword*,sword*,sword*))pDllFcn)(pMajor, pMinor, pUpdate, pPatch, pPortUpdate);
   }

   sword (COL_C_DECL* oci_attr_get)          (const dvoid* pSourceHandle, ub4 SourceHandleType, dvoid* pAttribute, ub4* pSizeOfAttribute, ub4 AttributeType, OCIError* pErrorHandle );
   sword (COL_C_DECL* oci_attr_set)          (dvoid* pTargetHandle, ub4 TargetHandleType, dvoid* pAttribute, ub4 SizeOfAttribute, ub4 AttributeType, OCIError* pErrorHandle );
   sword (COL_C_DECL* oci_bind_by_name)      (OCIStmt* pStatementHandle, OCIBind** ppBindHandle, OCIError* pErrorHandle, const OraText* pBindName, sb4 SizeOfBindName, dvoid* pValueBuffer, sb4 SizeOfValueBuffer, ub2 OciDataType, dvoid* pIndicator, ub2* pActualLengthArray, ub2* pReturnCodeArray, ub4 MaximumCountOfType, ub4* pCountOfElement, ub4 Mode );
   // Use oci_bind_by_name2 when working with data types when lengths exceed UB2MAXVAL on the client. 
   // JULIAN for some reason it does not recognize the sb8 definition in oratypes.h 
   // sword (COL_C_DECL* oci_bind_by_name2)     (OCIStmt* pStatementHandle, OCIBind** ppBindHandle, OCIError* pErrorHandle, const OraText* pBindName, sb4 SizeOfBindName, dvoid* pValueBuffer, sb8 SizeOfValueBuffer, ub2 OciDataType, dvoid* pIndicator, ub4* pActualLengthArray, ub2* pReturnCodeArray, ub4 MaximumCountOfType, ub4* pCountOfElement, ub4 Mode );
   sword (COL_C_DECL* oci_define_by_pos)     (OCIStmt* pStatement, OCIDefine** ppDefine, OCIError* pErrorHandle, ub4 SelectListPosition, dvoid* pValueBuffer, sb4 SizeOfValueBuffer, ub2 OciDataType, dvoid* pIndicator, ub2* pLengthOfData, ub2* pReturnCode, ub4 Mode );
   sword (COL_C_DECL* oci_define_dynamic)    (OCIDefine* pDefine, OCIError* pErrorHandle, dvoid* pContext,  sb4 (*fpDynamicDefineCallback)(dvoid* pContext, OCIDefine* pDefine, ub4 RowIndex, dvoid** ppBuffer, ub4** ppSizeOfBuffer, ub1* pPieceValue, dvoid** ppIndicator, ub2** ppReturnCode) );
   sword (COL_C_DECL* oci_descriptor_alloc)  (const dvoid* pEnvironmentHandle, dvoid** ppDescriptor, ub4 DescriptorType, size_t SizeOfUserMemory, dvoid** ppUserMemory);
   sword (COL_C_DECL* oci_descriptor_free)   (dvoid *pDescriptor, ub4 DescriptorType );
   sword (COL_C_DECL* oci_env_nls_create)    (OCIEnv** pOutEnvHandle, ub4 Mode, dvoid* pContext, dvoid * (*pMalocFunc) (dvoid *pContext, size_t size), dvoid* (*pRalocFunc) (dvoid *pContext, dvoid *memptr, size_t newsize), void (*pMfreeFunc) (dvoid *pContext, dvoid *memptr), size_t ExtraMemSize, dvoid **pUserMem, ub2 CharSetId, ub2 NCharSetId);
   sword (COL_C_DECL* oci_error_get)         (dvoid* pHandle, ub4 RecordNumber, OraText* pSqlState, sb4* pErrorCode, OraText* pErrorBuffer, ub4 SizeOfErrorBuffer, ub4 HandleType );// "Not supported in release 8.x or later."
   sword (COL_C_DECL* oci_handle_alloc)      (const dvoid* pEnvironmentHandle, dvoid** ppHandle, ub4 HandleType, size_t SizeOfUserMemory, dvoid** ppUserMemory );
   sword (COL_C_DECL* oci_handle_free)       (dvoid* pHandle, ub4 HandleType );
   sword (COL_C_DECL* oci_lob_char_set_form) (OCIEnv*, OCIError*, const OCILobLocator*, ub1*);
   sword (COL_C_DECL* oci_lob_read_2)        (OCISvcCtx* pServiceContextHandle, OCIError* pErrorHandle, OCILobLocator* pLobLocator, ub4* pAmount, ub4 Offset, dvoid* pBuffer, ub4 SizeOfBuffer, dvoid* pContext, OCICallbackLobRead fpLobReadCallback, ub2 CharacterSetId, ub1 CharacterSetForm );
   sword (COL_C_DECL* oci_logoff)            (OCISvcCtx* pServiceContextHandle, OCIError* pErrorHandle );
   sword (COL_C_DECL* oci_logon)             (OCIEnv* pEnvironmentHandle, OCIError* pErrorHandle, OCISvcCtx** pServiceHandle, const OraText* pUserName, ub4 LengthOfUserName, const OraText* pPassword, ub4 LengthOfPassword, const OraText* pDataSourceName, ub4 LengthOfDataSourceName );
   sword (COL_C_DECL* oci_param_get)         (const dvoid* pSourceHandle, ub4 SourceHandleType, OCIError* pErrorHandle, dvoid** ppParameter, ub4 ParameterPostion );
   sword (COL_C_DECL* oci_stmt_execute)      (OCISvcCtx* pServiceContextHandle, OCIStmt* pStatementHandle, OCIError* pErrorHandle, ub4 CountOfIteration, ub4 IterationOffset, const OCISnapshot* pSnapshotIn, OCISnapshot* pSnapshotOut, ub4 Mode);
   sword (COL_C_DECL* oci_stmt_fetch_2)      (OCIStmt* pStatementHandle, OCIError* pErrorHandle, ub4 CountOfRow, ub2 FetchOrientation, sb4 ScrollOffset, ub4 Mode);
   sword (COL_C_DECL* oci_stmt_prepare_2)    (OCISvcCtx* pServiceContextHandle, OCIStmt** ppStatementHandle, OCIError* pErrorHandle, const OraText* pStatement, ub4 SizeOfStatement, const OraText* pCacheKey, ub4 SizeOfpCacheKey, ub4 Syntax, ub4 Mode);
   sword (COL_C_DECL* oci_trans_commit)      (OCISvcCtx* pServiceContextHandle, OCIError* pErrorHandle, ub4 Flag);
   sword (COL_C_DECL* oci_trans_rollback)    (OCISvcCtx* pServiceContextHandle, OCIError* pErrorHandle, ub4 Flag );
   // ------ never used ------
   // sword (COL_C_DECL* oci_describe_any)      (OCISvcCtx* pServiceContextHandle, OCIError* pErrorHandle, dvoid* pObject, ub4 SizeOfObjectName, ub1 ObjectType, ub1 InfoLevel, ub1 SchemaObjectType, OCIDescribe* pDescribe);
   // sword (COL_C_DECL* oci_server_attach)     (OCIServer* pServerHandle, OCIError* pErrorHandle, CONST OraText* pDataSourceName, sb4 SizeOfDataSourceName, ub4 Mode);
   // sword (COL_C_DECL* oci_session_begin)     (OCISvcCtx* pServiceContextHandle, OCIError* pErrorHandle, OCISession* pSessionHandle, ub4 CredentialsType, ub4 Mod );
   // sword (COL_C_DECL* oci_session_end)       (OCISvcCtx *pServiceContextHandle, OCIError* pErrorHandle, OCISession* pSessionHandle, ub4 Mode);
   // ------ never used + deprecated ------
   // sword (COL_C_DECL* oci_lob_get_length)    (OCISvcCtx* pServiceContextHandle, OCIError* pErrorHandle, OCILobLocator* pLobLocator, ub4* pLength);

   // ------ oci_initialize, oci_env_init deprecated - use oci_env_nls_create instead ------
   // sword (COL_C_DECL* oci_initialize)        (ub4 Mode, dvoid* pContext, dvoid* (*fpMemoryAllocation)(dvoid* pContext, size_t SizeOfMemory), dvoid* (*fpMemoryReallocation)(dvoid* pContext, dvoid* pMemory, size_t SizeOfNewMemory), void (*fpFreeMemory)(dvoid* pContext, dvoid* pMemory) );
   // sword (COL_C_DECL* oci_env_init)          (OCIEnv** ppEnvironmentHandle, ub4 Mode, size_t SizeOfUserMemory, dvoid** ppUserMemory );
   // ------ deprecated use <function>_2 instead ------
   // sword (COL_C_DECL* oci_lob_read)          (OCISvcCtx* pServiceContextHandle, OCIError* pErrorHandle, OCILobLocator* pLobLocator, ub4* pAmount, ub4 Offset, dvoid* pBuffer, ub4 SizeOfBuffer, dvoid* pContext, OCICallbackLobRead fpLobReadCallback, ub2 CharacterSetId, ub1 CharacterSetForm );
   sword (COL_C_DECL* oci_stmt_prepare)      (OCIStmt* pStatementHandle, OCIError* pErrorHandle, CONST OraText* pStatement, ub4 SizeOfStatement, ub4 Syntax, ub4 Mode);
private:
   COLstring   m_LibraryPath;
   COLdll      m_OracleOciDll;
   bool        m_IsLoaded;
   COLstring   m_ClientVersionString;
   COLuint32   m_ClientVersionUint;
};

#endif