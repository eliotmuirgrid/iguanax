//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOciOracle
//
// Description:
//
// Implementation
//
// Author: Henry Tran
// Date:   May 2, 2004 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBdatabaseOciOracle.h"
#include "DBdatabaseOracle.h"
#include "DBresultSetPtr.h"
#include "DBminimumInclude.h"
#include "DBsqlInsert.h"
#include "DBresultSetRow.h"
#include "DBsqlSelect.h"
#include "DBsqlUpdate.h"
#include "DBvariant.h"

#include <COL/COLmutex.h>
#include <COL/COLauto.h>
#include <COL/COLdateTime.h>
#include <COL/COLdll.h>
#include <COL/COLlocker.h>
#include <COL/COLstring.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <FIL/FILutils.h>
#include <LINK/LINKconstants.h>

#ifdef _WIN32
   #include <REG/REGregistryWindows.h>
#endif

#include "OCIoracleInclude/oci.h"
#include "OCIoracleInclude/ocidem.h"
#include <stdlib.h>

static const int DB_OCI_ONE_THREADED_INIT = 1;

static const int DB_OCI_CHARSET_UTF8 = 871;

//be conservative
#define MAXBUFLEN 5028

#ifdef _WIN32
#  define FCN_CALL __cdecl
#else
#  define FCN_CALL
#endif

class DBdatabaseOciOracleDll{
public:
   DBdatabaseOciOracleDll(COLdll* pDll) : MajorVersion(0), MinorVersion(0)
   {
      COL_METHOD(DBdatabaseOciOracleDll::DBdatabaseOciOracleDll);
      m_pDll = pDll;  // pDll is already loaded from the DBdatabaseOciOracle::initialize logic, we just want to take ownership
      initializeFunctionPointers();
   }

   ~DBdatabaseOciOracleDll(){
      COL_METHOD(DBdatabaseOciOracleDll::~DBdatabaseOciOracleDll);
      delete m_pDll;
   }

   sword (FCN_CALL* oci_attr_get) ( CONST dvoid* pSourceHandle,
                       ub4 SourceHandleType,
                       dvoid* pAttribute,
                       ub4* pSizeOfAttribute,
                       ub4 AttributeType,
                       OCIError* pErrorHandle );

   sword (FCN_CALL* oci_attr_set) ( dvoid* pTargetHandle, 
                       ub4 TargetHandleType,
                       dvoid* pAttribute,
                       ub4 SizeOfAttribute, 
                       ub4 AttributeType, 
                       OCIError* pErrorHandle );
   
   sword (FCN_CALL* oci_bind_by_name) ( OCIStmt* pStatementHandle, 
                          OCIBind** ppBindHandle,
                          OCIError* pErrorHandle,
                          const text* pBindName,
                          sb4 SizeOfBindName,
                          dvoid* pValueBuffer,
                          sb4 SizeOfValueBuffer,
                          ub2 OciDataType,
                          dvoid* pIndicator,
                          ub2* pActualLengthArray,
                          ub2* pReturnCodeArray,
                          ub4 MaximumCountOfType,
                          ub4* pCountOfElement, 
                          ub4 Mode );

   //this has to be loaded every time, as the thrown exception from the getFunctionAddress indicates that
   //the oracle version is 10.1 or older
   sword oci_client_version(sword* pMajor, sword* pMinor, sword* pUpdate, sword* pPatch, sword* pPortUpdate){
      COLdllFcnPtr pDllFcn = m_pDll->getProcAddress("OCIClientVersion");
      return (*(sword (FCN_CALL*)(sword*,sword*,sword*,sword*,sword*))pDllFcn)(pMajor, pMinor, pUpdate, pPatch, pPortUpdate);
   }

   sword (FCN_CALL* oci_define_by_pos) ( OCIStmt* pStatement,
                            OCIDefine** ppDefine,
                            OCIError* pErrorHandle,
                            ub4 SelectListPosition, 
                            dvoid* pValueBuffer, 
                            sb4 SizeOfValueBuffer, 
                            ub2 OciDataType,
                            dvoid* pIndicator, 
                            ub2* pLengthOfData, 
                            ub2* pReturnCode, 
                            ub4 Mode );

   sword (FCN_CALL* oci_define_dynamic) ( OCIDefine* pDefine, 
                             OCIError* pErrorHandle, 
                             dvoid* pContext,
                             sb4 (*fpDynamicDefineCallback)(dvoid* pContext,
                                                            OCIDefine* pDefine,
                                                            ub4 RowIndex,
                                                            dvoid** ppBuffer,
                                                            ub4** ppSizeOfBuffer,
                                                            ub1* pPieceValue,
                                                            dvoid** ppIndicator,
                                                            ub2** ppReturnCode) );

   sword (FCN_CALL* oci_describe_any) ( OCISvcCtx* pServiceContextHandle,
                           OCIError* pErrorHandle,
                           dvoid* pObject,
                           ub4 SizeOfObjectName,
                           ub1 ObjectType,
                           ub1 InfoLevel,
                           ub1 SchemaObjectType,
                           OCIDescribe* pDescribe );

   sword (FCN_CALL* oci_descriptor_alloc) ( CONST dvoid* pEnvironmentHandle,
                               dvoid** ppDescriptor, 
                               ub4 DescriptorType,
                               size_t SizeOfUserMemory, 
                               dvoid** ppUserMemory );

   sword (FCN_CALL* oci_descriptor_free) ( dvoid *pDescriptor,
                              ub4 DescriptorType );

   sword (FCN_CALL* oci_env_init) ( OCIEnv** ppEnvironmentHandle,
                       ub4 Mode, 
                       size_t SizeOfUserMemory, 
                       dvoid** ppUserMemory );

   sword (FCN_CALL* oci_env_nls_create) (OCIEnv** pOutEnvHandle,
                      ub4           Mode,
                      dvoid         *pContext,
                      dvoid         *(*pMalocFunc) (dvoid *pContext, size_t size),
                      dvoid         *(*pRalocFunc) (dvoid *pContext, dvoid *memptr, size_t newsize),
                      void          (*pMfreeFunc) (dvoid *pContext, dvoid *memptr),
                      size_t        ExtraMemSize,
                      dvoid         **pUserMem,
                      ub2           CharSetId,
                      ub2           NCharSetId);

   sword (FCN_CALL* oci_error_get) ( dvoid* pHandle, 
                        ub4 RecordNumber,
                        text* pSqlState, // "Not supported in release 8.x or later."
                        sb4* pErrorCode, 
                        text* pErrorBuffer,
                        ub4 SizeOfErrorBuffer,
                        ub4 HandleType );

   sword (FCN_CALL* oci_handle_alloc) ( CONST dvoid* pEnvironmentHandle, 
                           dvoid** ppHandle, 
                           ub4 HandleType, 
                           size_t SizeOfUserMemory, 
                           dvoid** ppUserMemory );

   sword (FCN_CALL* oci_handle_free) ( dvoid* pHandle,
                          ub4 HandleType );

   sword (FCN_CALL* oci_initialize)( ub4    Mode, 
                         dvoid* pContext, 
                         dvoid* (*fpMemoryAllocation)(dvoid* pContext, size_t SizeOfMemory),
                         dvoid* (*fpMemoryReallocation)(dvoid* pContext, dvoid* pMemory, size_t SizeOfNewMemory),
                         void   (*fpFreeMemory)(dvoid* pContext, dvoid* pMemory) );

   sword (FCN_CALL* oci_lob_get_length) ( OCISvcCtx* pServiceContextHandle,
                             OCIError* pErrorHandle,
                             OCILobLocator* pLobLocator,
                             ub4* pLength );

   sword (FCN_CALL* oci_lob_char_set_form) (OCIEnv*, OCIError*, CONST OCILobLocator*, ub1*);

   sword (FCN_CALL* oci_lob_read) ( OCISvcCtx* pServiceContextHandle,
                       OCIError* pErrorHandle,
                       OCILobLocator* pLobLocator,
                       ub4* pAmount,
                       ub4 Offset,
                       dvoid* pBuffer,
                       ub4 SizeOfBuffer,
                       dvoid* pContext, 
                       OCICallbackLobRead fpLobReadCallback,
                       ub2 CharacterSetId,
                       ub1 CharacterSetForm );
   
   sword (FCN_CALL* oci_logoff) ( OCISvcCtx* pServiceConextHandle,
                     OCIError* pErrorHandle );

   sword (FCN_CALL* oci_logon) ( OCIEnv* pEnvironmentHandle,
                    OCIError* pErrorHandle,
                    OCISvcCtx** pServiceHandle,
                    CONST OraText* pUserName,
                    ub4 LengthOfUserName,
                    CONST OraText* pPassword,
                    ub4 LengthOfPassword,
                    CONST OraText* pDataSourceName,
                    ub4 LengthOfDataSourceName );

   sword (FCN_CALL* oci_param_get) ( CONST dvoid* pSourceHandle,
                        ub4 SourceHandleType,
                        OCIError* pErrorHandle,
                        dvoid** ppParameter,
                        ub4 ParameterPostion );

   sword (FCN_CALL* oci_server_attach) ( OCIServer* pServerHandle,
                            OCIError* pErrorHandle,
                            CONST text* pDataSourceName, 
                            sb4 SizeOfDataSourceName, 
                            ub4 Mode );

   sword (FCN_CALL* oci_session_begin) ( OCISvcCtx* pServiceContextHandle, 
                            OCIError* pErrorHandle, 
                            OCISession* pSessionHandle,
                            ub4 CredentialsType, 
                            ub4 Mode );

   sword (FCN_CALL* oci_session_end) ( OCISvcCtx *pServiceContextHandle,
                          OCIError* pErrorHandle,
                          OCISession* pSessionHandle,
                          ub4 Mode );

   sword (FCN_CALL* oci_stmt_execute) ( OCISvcCtx* pServiceContextHandle,
                           OCIStmt* pStatementHandle,
                           OCIError* pErrorHandle, 
                           ub4 CountOfIteration, 
                           ub4 IterationOffset,
                           CONST OCISnapshot* pSnapshotIn, 
                           OCISnapshot* pSnapshotOut,
                           ub4 Mode );

   sword (FCN_CALL* oci_stmt_fetch_2) ( OCIStmt* pStatementHandle,
                           OCIError* pErrorHandle,
                           ub4 CountOfRow, 
                           ub2 FetchOrientation, 
                           sb4 ScrollOffset, 
                           ub4 Mode );

   sword (FCN_CALL* oci_stmt_prepare) ( OCIStmt* pStatementHandle,
                           OCIError* pErrorHandle,
                           CONST text* pStatement,
                           ub4 SizeOfStatement,
                           ub4 Syntax,
                           ub4 Mode );

   sword (FCN_CALL* oci_trans_commit) ( OCISvcCtx* pServiceContextHandle,
                           OCIError* pErrorHandle,
                           ub4 Flag );

   sword (FCN_CALL* oci_trans_rollback) ( OCISvcCtx* pServiceContextHandle, 
                             OCIError* pErrorHandle,
                             ub4 Flag );


   static bool isLibrarySupported(COLdll&);

   const COLstring& versionString();
   COLint32 minorVersion();
   COLint32 majorVersion();
   
   const COLstring& libraryName() const{
      return m_pDll->libraryName();
   }

private:
   //templates make this a whole lot easier
   template<class T>
   void assign(T& Function, const char* FunctionName){
      Function = (T)m_pDll->getProcAddress(FunctionName);
   }

   void initVersion();

   void initializeFunctionPointers(){
      assign(oci_attr_get, "OCIAttrGet");
      assign(oci_attr_set, "OCIAttrSet");
      assign(oci_bind_by_name, "OCIBindByName");
      assign(oci_define_by_pos, "OCIDefineByPos");
      assign(oci_define_dynamic, "OCIDefineDynamic");
      assign(oci_describe_any, "OCIDescribeAny");
      assign(oci_descriptor_alloc, "OCIDescriptorAlloc");
      assign(oci_descriptor_free, "OCIDescriptorFree");
      assign(oci_env_init, "OCIEnvInit");
      assign(oci_error_get, "OCIErrorGet");
      assign(oci_handle_alloc, "OCIHandleAlloc");
      assign(oci_handle_free, "OCIHandleFree");
      assign(oci_initialize, "OCIInitialize");
      assign(oci_lob_get_length, "OCILobGetLength");
      assign(oci_lob_char_set_form, "OCILobCharSetForm");
      assign(oci_lob_read, "OCILobRead");
      assign(oci_logoff, "OCILogoff");
      assign(oci_logon, "OCILogon");
      assign(oci_param_get, "OCIParamGet");
      assign(oci_server_attach, "OCIServerAttach");
      assign(oci_session_begin, "OCISessionBegin");
      assign(oci_session_end, "OCISessionEnd");
      assign(oci_stmt_execute, "OCIStmtExecute");
      assign(oci_stmt_fetch_2, "OCIStmtFetch2");
      assign(oci_stmt_prepare, "OCIStmtPrepare");
      assign(oci_trans_commit, "OCITransCommit");
      assign(oci_trans_rollback, "OCITransRollback");
      assign(oci_env_nls_create, "OCIEnvNlsCreate");
   }

   COLstring VersionString;
   COLint32 MajorVersion;
   COLint32 MinorVersion;

   COLdll* m_pDll;
   DBdatabaseOciOracleDll& operator = (const DBdatabaseOciOracleDll&);
};

static DBdatabaseOciOracleDll* pLoadedOciOracleDll;  // Leaked on purpose (speed/simplicity).

bool DBdatabaseOciOracleDll::isLibrarySupported(COLdll& Dll){
   if( Dll.loaded() ){
      try{
         DBdatabaseOciOracleDll testOciOracleDll(&Dll);
         return true;
      } catch(const COLerror&){
         // We were unable to find a required function.
      }
   }

   return false;
}


bool DBdatabaseOciOracle::isLibrarySupported(const COLstring& LibraryPath){
   COLdll Dll;
   return (Dll.tryLoadLibrary(LibraryPath) == COL_DLL_LOAD_OK)
      && DBdatabaseOciOracleDll::isLibrarySupported(Dll);
}

bool DBdatabaseOciOracle::isValidLibraryPath(const COLstring& LibraryPath){
   return (FILfileExists(LibraryPath));
}

DBdatabaseOciOracleLoadResult DBdatabaseOciOracle::libraryLoadHasError(const COLstring& LibraryPath){
   COLdll Dll;
   COLdllLoadResult Result = Dll.tryLoadLibrary(LibraryPath);
   DBdatabaseOciOracleLoadResult ReturnValue;

   //can't just do a straight cast, interface might change
   switch(Result){
   case COL_DLL_LOAD_OK:
      ReturnValue = DB_OCI_LOAD_OK;
      break;
   case COL_DLL_LOAD_INVALID_LIBRARY:
      ReturnValue = DB_OCI_LOAD_INVALID_LIBRARY;
      break;
   case COL_DLL_LOAD_NOT_EXIST:
      ReturnValue = DB_OCI_LOAD_NOT_EXIST;
      break;
   case COL_DLL_LOAD_NO_PERMISSIONS:
      ReturnValue = DB_OCI_LOAD_NO_PERMISSIONS;
      break;
   default:
      ReturnValue = DB_OCI_LOAD_UNKNOWN_ERROR;
   }

   return ReturnValue;
}


// This class is unsafe for use with COLarray, as it does not copy
// its resources.  Use COLarray< COref<DBdatabaseOciOracleBuffer> >
// to be safe.
class DBdatabaseOciOracleBuffer : public COLrefCounted{
public:
   DBdatabaseOciOracleBuffer()
      : pDouble( NULL ),
        pInteger( NULL ),
        pOciDate( NULL ),
        pOciLobLocator( NULL ),
        pStringBuffer( NULL ) {}

   ~DBdatabaseOciOracleBuffer(){
      try{
         clearBuffer();
      } catch(const COLerror& Error) {
         DB2_LOG(Error);
      }
   } 

   void clearBuffer();

   COLfloat64* double_(){
      if ( pDouble == NULL ){
         clearBuffer();
         pDouble = new COLfloat64();
      }
      return pDouble;
   }

   COLint32* integer(){
      if ( pInteger == NULL ){
         clearBuffer();
         pInteger = new COLint32();
      }
      return pInteger;
   }

   OCIDate* ociDate( OCIEnv* pEnvironmentHandle ){
      if ( pOciDate == NULL ){
         clearBuffer();
         pLoadedOciOracleDll->oci_descriptor_alloc( pEnvironmentHandle,
                                                   (dvoid**) &pOciDate, 
                                                   OCI_DTYPE_DATE, 
                                                   0, 
                                                   NULL );
      }
      return pOciDate;
   }

   OCIDate* ociDate(){
      COLPRECONDITION( pOciDate != NULL )
      return pOciDate;
   }

   OCILobLocator** ociLobLocator( OCIEnv* pEnvironmentHandle ){
      if ( pOciLobLocator == NULL ){
         clearBuffer();
         pLoadedOciOracleDll->oci_descriptor_alloc( pEnvironmentHandle,
                                                   (dvoid**) &pOciLobLocator, 
                                                   OCI_DTYPE_LOB, 
                                                   0, 
                                                   NULL );
      }
      return &pOciLobLocator;
   }

   OCILobLocator** ociLobLocator(){
      COLPRECONDITION( pOciLobLocator != NULL )
      return &pOciLobLocator;
   }

   COLstring* stringBuffer(){
      if ( pStringBuffer == NULL ){
         clearBuffer();
         pStringBuffer = new COLstring();
         BufferHead = 0;
         BufferTail = 0;
      }
      return pStringBuffer;
   }

   size_t bufferSize() const {
      return BufferHead + BufferTail;
   }

   ub4 BufferHead;
   ub4 BufferTail;
   sb2 Indicator;

private:
   COLstring* pStringBuffer; // TODO why a pointer instead of a regular member?
   COLint32* pInteger;
   COLfloat64* pDouble;

   OCIDate* pOciDate;
   OCILobLocator* pOciLobLocator;
};

void DBdatabaseOciOracleBuffer::clearBuffer(){
   if ( pDouble != NULL ){
      delete pDouble;
      pDouble = NULL;
   }
   else if ( pInteger != NULL ){
      delete pInteger;
      pInteger = NULL;
   }
   else if ( pOciDate != NULL ){
      pLoadedOciOracleDll->oci_descriptor_free( pOciDate, OCI_DTYPE_DATE );
      pOciDate = NULL;
   }
   else if ( pOciLobLocator != NULL ){
      pLoadedOciOracleDll->oci_descriptor_free( pOciLobLocator, OCI_DTYPE_LOB );
      pOciLobLocator = NULL;
   }
   else if ( pStringBuffer != NULL ){
      delete pStringBuffer;
      pStringBuffer = NULL;
      BufferHead = 0;
      BufferTail = 0;
   }
}

class DBdatabaseOciOraclePrivate{
public:
   DBdatabaseOciOraclePrivate(DBdatabaseOciOracle* ipOwner) :
      pOwner(ipOwner), 
      AutoCommitFlag( true ),
      pEnvironmentHandle( NULL ),
      pServerHandle( NULL ),
      pServiceContextHandle( NULL ),
      pSessionHandle( NULL ),
      pErrorHandle( NULL ),
      UseUnicode(false)
   {
      COLPRECONDITION( DBdatabaseOciOracle::isInitialized() );
   }

   bool AutoCommitFlag;

   void checkForErrorWithSql(sword StatusCode, const COLstring& SqlStatementString);
   void checkForError(sword StatusCode);
   bool didStatementSucceed(sword StatusCode, COLstring& Error);

   DBdatabaseOciOracle* pOwner;
   OCIEnv* pEnvironmentHandle;
   OCIServer* pServerHandle;
   OCISvcCtx* pServiceContextHandle;
   OCISession* pSessionHandle;
   OCIError* pErrorHandle;

   bool UseUnicode;

   void createHandlesIfNeeded();

   void requireHandles();

   void fillResultSet( OCIStmt* pStatementHandle,
                       COLarray< COLref<DBdatabaseOciOracleBuffer> >& BindVector,
                       COLarray< DBdataType >& TreDataTypeVector,
                       COLarray< ub2 >& OciTypeVector,
                       DBresultSetPtr pResultSet,
                       const COLstring& SqlString,
                       COLuint32 ResultSetStart = 0,
                       COLuint32 ResultSetSize = 0);

   OCIDefine* defineColumn( OCIStmt* pStatementHandle, 
                          COLuint32 ColumnIndex,
                          ub2 OciDataType,
                          DBdatabaseOciOracleBuffer& DataBuffer, 
                          DBdataType TreDataType,
                          ub2 MaxDataSize,
                          const COLstring& SqlString);

   void disconnect();

   void executeSqlBind( const DBsqlInsert& SqlCommand, const COLstring& SqlString );
   void bindAllColumns( const DBsqlInsert& SqlCommand, 
                        OCIStmt* pStatementHandle,
                        COLarray<OCIBind*>& BindPtrVector,
                        COLarray< COLref<DBdatabaseOciOracleBuffer> >& DateTimeBufferVector,
                        const COLstring& SqlString);
   void throwBindError( const DBsqlInsert& SqlCommand, 
                        const COLstring& SqlString,
                        OCIError* pErrorHandle );
   COLstring getAllErrorMessages(OCIError* pErrorHandle); 
   // substitute the real values in the SQL query
   COLstring recreateSqlQuery( const DBsqlInsert& SqlCommand, const COLstring& SqlString );
   COLstring recreateSqlQuery( const DBsqlUpdate& SqlCommand, const COLstring& SqlString );
};

void DBdatabaseOciOraclePrivate::disconnect(){
   if ( pServiceContextHandle != NULL ){
      requireHandles();
      pLoadedOciOracleDll->oci_logoff( pServiceContextHandle,
                                         pErrorHandle );      

      pLoadedOciOracleDll->oci_handle_free( pServiceContextHandle, OCI_HTYPE_SVCCTX );
      pServiceContextHandle = NULL;
      pServerHandle = NULL; //deallocated because it is a child of the service context handle
   }
}

void DBdatabaseOciOraclePrivate::executeSqlBind(const DBsqlInsert& SqlCommand, const COLstring& SqlString){
   requireHandles();
   OCIStmt* pStatementHandle = NULL;
   try{
      checkForErrorWithSql( pLoadedOciOracleDll->oci_handle_alloc(pEnvironmentHandle, 
                                                          (dvoid**) &pStatementHandle,
                                                          OCI_HTYPE_STMT,
                                                          0, 
                                                          NULL), recreateSqlQuery(SqlCommand, SqlString) );   

      checkForErrorWithSql( pLoadedOciOracleDll->oci_stmt_prepare(pStatementHandle, 
                                                             pErrorHandle, 
                                                             (text*) SqlString.c_str(), 
                                                             SqlString.size(),
                                                             OCI_NTV_SYNTAX, 
                                                             OCI_DEFAULT), recreateSqlQuery(SqlCommand, SqlString) );

      COLarray< OCIBind* > BindPtrVector;

      // This resizing is okay since the entries are set to NULL in bindAllColumns
      // before an oci function is called - Eliot
      BindPtrVector.resize( SqlCommand.countOfColumn() );

      COLarray< COLref<DBdatabaseOciOracleBuffer> > DateTimeBufferVector;
      
      DateTimeBufferVector.resize( SqlCommand.countOfColumn() );
      for (int i = 0; i < DateTimeBufferVector.size(); i++){
         DateTimeBufferVector[i] = new DBdatabaseOciOracleBuffer();
      }
   
      bindAllColumns( SqlCommand,
                      pStatementHandle,
                      BindPtrVector,
                      DateTimeBufferVector,
                      SqlString );

      ub4 ExecuteMode;
      if ( AutoCommitFlag == true ){
         ExecuteMode = OCI_COMMIT_ON_SUCCESS;
      } else {
         ExecuteMode = OCI_DEFAULT;
      }

      sword ExecuteResult = pLoadedOciOracleDll->oci_stmt_execute(pServiceContextHandle, 
                                                                    pStatementHandle, 
                                                                    pErrorHandle, 
                                                                    1, 
                                                                    0,
                                                                    NULL, 
                                                                    NULL, 
                                                                    ExecuteMode);

      if ( ExecuteResult == OCI_ERROR ){
         throwBindError( SqlCommand, recreateSqlQuery(SqlCommand, SqlString), pErrorHandle );
      } else {
         checkForErrorWithSql( ExecuteResult, recreateSqlQuery(SqlCommand, SqlString) );
      }

      if ( pStatementHandle != NULL ){
         checkForErrorWithSql( pLoadedOciOracleDll->oci_handle_free(pStatementHandle, OCI_HTYPE_STMT), recreateSqlQuery(SqlCommand, SqlString) );
      }
   } catch ( COLerror& Error ) {
      if ( pStatementHandle != NULL ){
         checkForErrorWithSql( pLoadedOciOracleDll->oci_handle_free(pStatementHandle, OCI_HTYPE_STMT), recreateSqlQuery(SqlCommand, SqlString) );
      }
      throw Error;
   }
}

void DBdatabaseOciOraclePrivate::bindAllColumns
( 
   const DBsqlInsert& SqlCommand, 
   OCIStmt* pStatementHandle,
   COLarray<OCIBind*>& BindPtrVector,
   COLarray< COLref<DBdatabaseOciOracleBuffer> >& DateTimeBufferVector,
   const COLstring& SqlString
)
{
   for ( COLuint32 ColumnIndex = 0; ColumnIndex < SqlCommand.countOfColumn(); ColumnIndex++ )
   {
      COLstring ColumnName = SqlCommand.columnName( ColumnIndex );
      ColumnName.replace( " ", "_" );
      const DBvariant& ColumnValue = SqlCommand.columnValue( ColumnIndex );
      DBdataType ColumnType = ColumnValue.dataType();
      BindPtrVector[ColumnIndex] = NULL;

      switch (ColumnType){
      case DB_STRING:
         {
            checkForErrorWithSql( pLoadedOciOracleDll->oci_bind_by_name(pStatementHandle,
                                                                  &BindPtrVector[ColumnIndex],
                                                                  pErrorHandle,
                                                                  (const text*) ColumnName.c_str(),
                                                                  ColumnName.size(),
                                                                  (dvoid*) ColumnValue.string().c_str(),
                                                                  ColumnValue.string().size() + 1,
                                                                  STRING_TYPE,
                                                                  NULL,
                                                                  NULL,
                                                                  NULL,
                                                                  0,
                                                                  NULL,
                                                                  OCI_DEFAULT), SqlString );
         }
         break;
      case DB_DATETIME:
         {
            const COLdateTime& DateTimeCol = ColumnValue.dateTime();
            OCIDate* pDateTimeOci = DateTimeBufferVector[ColumnIndex]->ociDate(pEnvironmentHandle);

            pDateTimeOci->OCIDateYYYY = DateTimeCol.year();
            pDateTimeOci->OCIDateMM = DateTimeCol.month();
            pDateTimeOci->OCIDateDD = DateTimeCol.day();
            pDateTimeOci->OCIDateTime.OCITimeHH = DateTimeCol.hour();
            pDateTimeOci->OCIDateTime.OCITimeMI = DateTimeCol.minute();
            pDateTimeOci->OCIDateTime.OCITimeSS = DateTimeCol.second();      

            checkForErrorWithSql( pLoadedOciOracleDll->oci_bind_by_name(pStatementHandle,
                                                                  &BindPtrVector[ColumnIndex],
                                                                  pErrorHandle,
                                                                  (const text*) ColumnName.c_str(),
                                                                  ColumnName.size(),
                                                                  (dvoid*) pDateTimeOci,
                                                                  sizeof( *pDateTimeOci ),
                                                                  SQLT_ODT,
                                                                  NULL,
                                                                  NULL,
                                                                  NULL,
                                                                  0,
                                                                  NULL,
                                                                  OCI_DEFAULT), SqlString );
         }
         break;
      case DB_INTEGER:
         {
            checkForErrorWithSql( pLoadedOciOracleDll->oci_bind_by_name(pStatementHandle,
                                                                  &BindPtrVector[ColumnIndex],
                                                                  pErrorHandle,
                                                                  (const text*) ColumnName.c_str(),
                                                                  ColumnName.size(),
                                                                  (dvoid*) &((DBvariant&)ColumnValue).integer32(),
                                                                  sizeof( ColumnValue.integer32() ),
                                                                  INT_TYPE,
                                                                  NULL,
                                                                  NULL,
                                                                  NULL,
                                                                  0,
                                                                  NULL,
                                                                  OCI_DEFAULT), SqlString );
         }
         break;
         // DB_LARGE_INTEGER handled before this function is called since Oracle 9i
         // (and earlier versions) do not support 64-bit integers.
      /*else if ( ColumnType == TREvariantTypeEnum::eSmallInteger )
      {
         checkForError( OciOracleDllInstance().oci_bind_by_name(pStatementHandle,
                                                               &BindPtrVector[ColumnIndex],
                                                               pErrorHandle,
                                                               (const text*) ColumnName.c_str(),
                                                               ColumnName.size(),
                                                               (dvoid*) &ColumnValue.smallInteger(),
                                                               sizeof( ColumnValue.smallInteger() ),
                                                               INT_TYPE,
                                                               NULL,
                                                               NULL,
                                                               NULL,
                                                               0,
                                                               NULL,
                                                               OCI_DEFAULT) );
      }
      else if ( ColumnType == TREvariantTypeEnum::eTinyInteger )
      {
         checkForError( OciOracleDllInstance().oci_bind_by_name(pStatementHandle,
                                                               &BindPtrVector[ColumnIndex],
                                                               pErrorHandle,
                                                               (const text*) ColumnName.c_str(),
                                                               ColumnName.size(),
                                                               (dvoid*) &ColumnValue.tinyInteger(),
                                                               sizeof( ColumnValue.tinyInteger() ),
                                                               INT_TYPE,
                                                               NULL,
                                                               NULL,
                                                               NULL,
                                                               0,
                                                               NULL,
                                                               OCI_DEFAULT) );
      }*/
      case DB_LARGE_DOUBLE:
         {
            checkForErrorWithSql( pLoadedOciOracleDll->oci_bind_by_name(pStatementHandle,
                                                                  &BindPtrVector[ColumnIndex],
                                                                  pErrorHandle,
                                                                  (const text*) ColumnName.c_str(),
                                                                  ColumnName.size(),
                                                                  (dvoid*) &ColumnValue.float64(),
                                                                  sizeof( ColumnValue.float64() ),
                                                                  FLOAT_TYPE,
                                                                  NULL,
                                                                  NULL,
                                                                  NULL,
                                                                  0,
                                                                  NULL,
                                                                  OCI_DEFAULT), SqlString );
         }
         break;
      case DB_DOUBLE:
         {
            checkForErrorWithSql( pLoadedOciOracleDll->oci_bind_by_name(pStatementHandle,
                                                                  &BindPtrVector[ColumnIndex],
                                                                  pErrorHandle,
                                                                  (const text*) ColumnName.c_str(),
                                                                  ColumnName.size(),
                                                                  (dvoid*) &ColumnValue.float32(),
                                                                  sizeof( ColumnValue.float32() ),
                                                                  FLOAT_TYPE,
                                                                  NULL,
                                                                  NULL,
                                                                  NULL,
                                                                  0,
                                                                  NULL,
                                                                  OCI_DEFAULT), SqlString );
         }
         break;
      case DB_DATA_TYPE_NOT_DEFINED:
         // do nothing
         break;
      default:
         COL_ERROR_STREAM( COL_T("Attempted to bind '") << ColumnName << COL_T("' to an unsupported datatype: ") << ColumnType, COLerror::PreCondition );
         break;
      }
   }
}

void DBdatabaseOciOraclePrivate::throwBindError(const DBsqlInsert& SqlCommand, const COLstring& SqlString, OCIError* pErrorHandle){
   COLstring OracleErrorMessage;
   OracleErrorMessage = getAllErrorMessages( pErrorHandle );   
   
   COLstring ErrorMessage;
   COLostream ErrorStream( ErrorMessage );
   
   ErrorStream << COL_T("Oracle error occurred while trying to execute SQL statement:") << newline
               << newline
               << COL_T("Oracle Error Message:") << newline
               << OracleErrorMessage
               << COL_T("SQL Statement:") << newline
               << SqlString << newline
               << newline
               << COL_T("SQL Statement Column Information:") << newline
               << COL_T("NAME|TYPE|SIZE|VALUE") << newline
               << COL_T("==========") << newline;
   for ( COLuint32 ColumnIndex = 0; ColumnIndex < SqlCommand.countOfColumn(); ColumnIndex++ ){
      const DBvariant& ColumnValue = SqlCommand.columnValue( ColumnIndex );
      ErrorStream << SqlCommand.columnName( ColumnIndex ) 
                  << '|' << DBdataTypeName( ColumnValue.dataType())
                  << '|' << ColumnValue.binarySize()
                  << '|' << ColumnValue.toString() << newline
                  << COL_T("----------") << newline;
   }
   ErrorStream << newline
               << COL_T("Please compare the SQL statement column information with your database schema.");
   COL_ERROR_STREAM( ErrorMessage, COLerror::PreCondition )
}

DBdatabaseOciOracle::DBdatabaseOciOracle()
   : m_pEscapeFunction(&DBescapeSql)
   , m_BinaryOkay(false)
{
   pMember = new DBdatabaseOciOraclePrivate(this);
   
   
   if(!DB_OCI_ONE_THREADED_INIT){
      //#10218 - is this clever to do here? also - OCI_THREADED may be better
      // Initialize OCI environment in default mode
      pLoadedOciOracleDll->oci_initialize( OCI_DEFAULT,                              
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL );
   }
   
}

void DBdatabaseOciOraclePrivate::createHandlesIfNeeded(){
   if(pEnvironmentHandle){
      COLPRECONDITION(pErrorHandle);
      return;
   };

   // Initialize environment handle

   bool UseNls = UseUnicode;
#ifdef CHM_HPUX
   // HPUX doesn't seem to handle OCIEnvNlsCreate() -
   // hopefully setting NLS_LANG=AMERICAN_AMERICA.UTF8 will be sufficient for handling UTF8 correctly
   // at least, it won't simply refuse to work at all (for non-Unicode customers)
   UseNls = false;
#endif

   if(UseNls){
      sword Result = pLoadedOciOracleDll->oci_env_nls_create(&pEnvironmentHandle,
         OCI_DEFAULT, // Mode
         0, // pContext
         0, // pMalocFunc
         0, // pRalocFunc
         0, // pMfreeFunc
         0, // ExtraMemSize
         0, // pUsermem
         DB_OCI_CHARSET_UTF8, // CharSet
         DB_OCI_CHARSET_UTF8  // NCharSet
      );
      if (Result != OCI_SUCCESS)
         COL_ERROR_STREAM(COL_T("Failed to create environment handle in Unicode mode - OCIEnvNlsCreate() returned ") << Result, COLerror::PreCondition);
   } else {
      sword Result = pLoadedOciOracleDll->oci_env_init( &pEnvironmentHandle, 
                                        OCI_DEFAULT, 
                                        0, 
                                        NULL );
      if (Result != OCI_SUCCESS)
         COL_ERROR_STREAM(COL_T("Failed to create environment handle - OCIEnvInit() returned ") << Result, COLerror::PreCondition);
   }

   COLPRECONDITION( !pErrorHandle);
   
   // Allocate the error reporting handle
   sword Result = pLoadedOciOracleDll->oci_handle_alloc( pEnvironmentHandle, 
                                            (dvoid**) &pErrorHandle, 
                                            OCI_HTYPE_ERROR, 
                                            0, 
                                            NULL );
   if (Result != OCI_SUCCESS)
      COL_ERROR_STREAM(COL_T("Failed to create error handle - OCIHandleAlloc() returned ") << Result, COLerror::PreCondition);
};

void DBdatabaseOciOraclePrivate::requireHandles(){
   if(!pEnvironmentHandle){
      COL_ERROR_STREAM("Environment handle is NULL. This is probably due to invoking operations on a connection before connecting.", COLerror::PreCondition);
   };
   COLPRECONDITION(pErrorHandle); // having Environment handle, but not Error handle would be highly abnormal
};

DBdatabaseOciOracle::~DBdatabaseOciOracle(){
   disconnect();

   OCIError* pErrorHandle       = pMember->pErrorHandle;
   OCIEnv*   pEnvironmentHandle = pMember->pEnvironmentHandle;

   // This must be done before the environment handle is freed.
   delete pMember;

   pLoadedOciOracleDll->oci_handle_free(pErrorHandle, OCI_HTYPE_ERROR);
   pLoadedOciOracleDll->oci_handle_free(pEnvironmentHandle, OCI_HTYPE_ENV);
}


#ifndef WIN32
   COLarray<COLstring> DBfindSupportedOracleDlls(bool){
      COLarray<COLstring> None;
      return None;
   }
#else // if defined(WIN32)
   COLarray<COLstring> DBfindSupportedOracleDlls(bool IgnoreCache)
   {
      static COLmutex CriticalSection;
      COLlocker Lock(CriticalSection);

      static COLarray<COLstring> CachedResults;
      static bool CacheIsValid = false;

      if( IgnoreCache || !CacheIsValid )
      {
         CachedResults.clear();

         COLstring DefaultHome;
         COLarray<COLstring> CandidatePathKeys;

         try
         {
            REGregistry Registry(HKEY_LOCAL_MACHINE);
            Registry.openKey("SOFTWARE\\ORACLE\\ALL_HOMES", false);

            DefaultHome = Registry.readString("DEFAULT_HOME", "");

            for(COLstring Key; Registry.readEnumKey(Key); )
            {
               Key = Key.c_str();  // There's a funny story here.
               CandidatePathKeys.push_back() = "SOFTWARE\\ORACLE\\ALL_HOMES\\" + Key;
            }
         }
         catch (const COLerror&)
         {
            // Oracle doesn't seem to be installed (normally).
         }

         for(COLindex KeyIndex=0, MaxIndex=CandidatePathKeys.size(); KeyIndex < MaxIndex; ++KeyIndex)
         {
            try
            {
               REGregistry Registry(HKEY_LOCAL_MACHINE);
               Registry.openKey(CandidatePathKeys[KeyIndex], false);

               COLstring Name = Registry.readString("NAME", "");
               COLstring Path = Registry.readString("PATH", "");

               COLdll Dll;

               if( !Path.is_null()
                  && (Dll.tryLoadLibrary((Path += "\\bin\\oci.dll").c_str()) == COL_DLL_LOAD_OK)
                  && DBdatabaseOciOracleDll::isLibrarySupported(Dll) )
               {
                  if( !DefaultHome.is_null() && DefaultHome == Name )
                  {
                     CachedResults.insert(Path, 0);
                  }
                  else
                  {
                     CachedResults.push_back(Path);
                  }
               }
            }
            catch(const COLerror&)
            {
               // This key isn't in the registry, or something else bad happened.
            }
         }

         CacheIsValid = true;
      }

      return CachedResults;
   }

   static bool DBloadOracleDllViaRegistry(COLdll& Dll, int &ErrorCode)
   {
      COLarray<COLstring> SupportedDlls = DBfindSupportedOracleDlls(true);
      int CurrentErrorCode;

      for(COLindex DllIndex=0, MaxIndex=SupportedDlls.size(); DllIndex < MaxIndex; ++DllIndex)
      {
         if( (CurrentErrorCode = Dll.tryLoadLibrary(SupportedDlls[DllIndex].c_str())) == COL_DLL_LOAD_OK )
         {
            return true;
         }
         if((CurrentErrorCode == COL_DLL_LOAD_NO_PERMISSIONS) || (CurrentErrorCode == COL_DLL_LOAD_INVALID_LIBRARY))
         {
            ErrorCode = CurrentErrorCode;
         }
      }

      return false;
   }
#endif // defined(WIN32)


bool DBdatabaseOciOracle::initialize(COLostream& LogStream, const char* LibraryPath){
   COL_FUNCTION(DBdatabaseOciOracle::initialize);
   if ( !isInitialized() ){
      static COLmutex CriticalSection;
      COLlocker Lock(CriticalSection);
      COL_TRC("Trying to load Oracle");
      LogStream << COL_T("Trying to load Oracle... ");
      COLdll* pDll = new COLdll();
      int ErrorCode = COL_DLL_LOAD_OK;
      COLstring RawError;
      COL_TRC("Loading from library path: " << LibraryPath);
      ErrorCode = pDll->tryLoadLibrary(LibraryPath, true, &RawError);
      COL_VAR2(pDll->libraryName(), pDll->loaded());
      if(!pDll->loaded()){
         COL_TRC("NO (Oracle OCI support not present)");
         LogStream << COL_T("NO (Oracle OCI support not present)") << newline;
         if(ErrorCode == COL_DLL_LOAD_NO_PERMISSIONS) {
            COL_TRC("Not enough permissions to load OCI.");
            LogStream << COL_T("Not enough permissions to load OCI.") << newline;
            LogStream << COL_T("For assistance, check the documentation ") << LINKdatabaseSettings << " " << newline;
            LogStream << COL_T("Raw error: " ) << newline << RawError << newline;
         }
         else if(ErrorCode == COL_DLL_LOAD_INVALID_LIBRARY){
            COL_TRC("The OCI library that was found in an invalid library.");
            LogStream << COL_T("The OCI library that was found in an invalid library.") << newline;
            LogStream << COL_T("For assistance, check the documentation ") << LINKdatabaseSettings << " " << newline;
            LogStream << COL_T("Raw error: " ) << newline << RawError << newline;
         }
      } else {         
         try{
            COLauto<DBdatabaseOciOracleDll> pTmpDll = new DBdatabaseOciOracleDll(pDll);
            LogStream << pTmpDll->libraryName() << ' ';
            //#14983 - we only support up to a certain version of oracle
            if (pTmpDll->majorVersion() > 19){
               LogStream << 'v' << pTmpDll->versionString() << COL_T(" is not supported. Contact iNTERFACEWARE for an updated version of this software.") << newline;
            } else {
               pLoadedOciOracleDll = pTmpDll.release();
               LogStream << 'v' << pLoadedOciOracleDll->versionString() << COL_T(" OK (Oracle OCI support enabled)") << newline;
            }
         } catch(...) {
            LogStream << COL_T(" NO (not supported)") << newline;
         }
      }
   } else {
      COL_TRC("Already loaded Oracle.");
      LogStream
         << COL_T("Already loaded Oracle: ") << pLoadedOciOracleDll->libraryName()
         << COL_T(" v") << pLoadedOciOracleDll->versionString()
         << COL_T(" OK (Oracle OCI support enabled)") << newline;
   }

   if(isInitialized() && DB_OCI_ONE_THREADED_INIT){
      // Initialize OCI environment in default mode
      pLoadedOciOracleDll->oci_initialize( OCI_THREADED,                              
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL );
   }

   return isInitialized();
}

bool DBdatabaseOciOracle::readyToReinitialize(){
   // I'm not sure if we're doing something wrong, but we can't seem
   // to switch to a different OCI Oracle library after using one.
   //
   return (pLoadedOciOracleDll == NULL);
}

bool DBdatabaseOciOracle::reinitialize(COLostream& LogStream, const char* LibraryPath){
   // Technically we can't reinitialize OCI Oracle because the DLLs don't like to co-exist;
   // and even if a DLL us unloaded, loading another doesn't seem to work.  But we provide
   // this function in hopes that we figure out how to do it in the future.

   static COLmutex CriticalSection;
   COLlocker Lock(CriticalSection);

   COLPRECONDITION( readyToReinitialize() );

   return initialize(LogStream, LibraryPath);
}


bool DBdatabaseOciOracle::isInitialized(){
   return (pLoadedOciOracleDll != NULL);
}

static COLmutex s_OCILogonMutex;

void DBdatabaseOciOracle::connect(const char* pDataSourceName, const char* pUserName, const char* pPassword){
   pMember->createHandlesIfNeeded();

   if ( !isConnected()
        || cachedDataSourceName() != pDataSourceName
        || cachedUserName() != pUserName
        || cachedPassword() != pPassword ){
      disconnect();

      // Note that OCI returns undefined values if an error occurs,
      // so we must use temp local variables
      OCISvcCtx* pTempServiceContext = 0;
      sword LogonResult = 0;
      {
         COLlocker Lock(s_OCILogonMutex); // #11073
         LogonResult = pLoadedOciOracleDll->oci_logon(pMember->pEnvironmentHandle,
                                        pMember->pErrorHandle,
                                        &pTempServiceContext,
                                        (CONST OraText*) pUserName,
                                        strlen(pUserName),
                                        (CONST OraText*) pPassword,
                                        strlen(pPassword),
                                        (CONST OraText*) pDataSourceName,
                                        strlen(pDataSourceName));
      }

      pMember->checkForError(LogonResult); 
      //success, set value
      pMember->pServiceContextHandle = pTempServiceContext;
      setCachedDataSourceName( pDataSourceName );
      setCachedUserName( pUserName );
      setCachedPassword( pPassword );
     }

   /* Old Rishi code. Might be required in the future.

   // Allocate the server handle
   checkForError( OciOracleDllInstance().oci_handle_alloc(pMember->pEnvironmentHandle, 
                                                          (dvoid**) &pMember->pServerHandle, 
                                                          OCI_HTYPE_SERVER,
                                                          0, 
                                                          NULL) );

   // Attach server to the data source 
   checkForError( OciOracleDllInstance().oci_server_attach(pMember->pServerHandle, 
                                                           pMember->pErrorHandle, 
                                                           (text*) pDataSourceName, 
                                                           strlen(pDataSourceName), 
                                                           0) );

   // Allocate the service context
   checkForError( OciOracleDllInstance().oci_handle_alloc(pMember->pEnvironmentHandle, 
                                                          (dvoid**) &pMember->pServiceContextHandle, 
                                                          OCI_HTYPE_SVCCTX,
                                                          0, 
                                                          NULL) );

   // Set server attribute for the service context
   checkForError( OciOracleDllInstance().oci_attr_set(pMember->pServiceContextHandle, 
                                                      OCI_HTYPE_SVCCTX, 
                                                      pMember->pServerHandle, 
                                                      0, 
                                                      OCI_ATTR_SERVER, 
                                                      pMember->pErrorHandle) );

   // Allocate the session handle   
   checkForError( OciOracleDllInstance().oci_handle_alloc(pMember->pEnvironmentHandle, 
                                                          (dvoid**) &pMember->pSessionHandle,
                                                          OCI_HTYPE_SESSION, 
                                                          0, 
                                                          NULL) );

   // Set user name attribute for the session
   checkForError( OciOracleDllInstance().oci_attr_set(pMember->pSessionHandle, 
                                                      OCI_HTYPE_SESSION,
                                                      (dvoid*) pUserName, 
                                                      strlen(pUserName),
                                                      OCI_ATTR_USERNAME, 
                                                      pMember->pErrorHandle) );

   // Set password attribute for the session
   checkForError( OciOracleDllInstance().oci_attr_set(pMember->pSessionHandle, 
                                                      OCI_HTYPE_SESSION,
                                                      (dvoid*) pPassword, 
                                                      strlen(pPassword),
                                                      OCI_ATTR_PASSWORD,
                                                      pMember->pErrorHandle) );

   // Begin the session
   checkForError( OciOracleDllInstance().oci_session_begin(pMember->pServiceContextHandle,  
                                                           pMember->pErrorHandle, 
                                                           pMember->pSessionHandle, 
                                                           OCI_CRED_RDBMS, 
                                                           OCI_DEFAULT) );

   // Set the session attribute for the service context
   checkForError( OciOracleDllInstance().oci_attr_set(pMember->pServiceContextHandle, 
                                                      OCI_HTYPE_SVCCTX,
                                                      (dvoid*) pMember->pSessionHandle, 
                                                      0,
                                                      OCI_ATTR_SESSION, 
                                                      pMember->pErrorHandle) );*/   
         
}

void DBdatabaseOciOracle::disconnect(){
   pMember->disconnect();

   DBdatabase::endTransaction();

   /* Old Rishi code. Might be required in the future.
   OciOracleDllInstance().oci_session_end( pMember->pServiceContextHandle,
                                                 pMember->pErrorHandle,
                                                 pMember->pSessionHandle,
                                                 OCI_DEFAULT );
   
   // "The OCI will deallocate a child handle if the parent is deallocated."
   OciOracleDllInstance().oci_handle_free( pMember->pEnvironmentHandle, OCI_HTYPE_ENV );
   pMember->pServiceContextHandle = NULL;
   pMember->pServerHandle = NULL;
   pMember->pSessionHandle = NULL;*/
}

static bool isBinaryType(ub2 OciDataType){
   switch(OciDataType) {
   case SQLT_BLOB:  // BLOB (duh!)
   case SQLT_BIN:   // RAW
   case SQLT_LBI:   // LONG RAW
      return true;
   }
   return false;
}

static DBdataType dbDataType( ub2 OciDataType, bool BinOkay ){
   switch ( OciDataType ){
      case SQLT_CHR: case SQLT_STR:
      case SQLT_LNG:  // LONG (a long string)
      case SQLT_AFC:  // CHAR, NCHAR
      case SQLT_RDD:  // ROWID
      case SQLT_INTERVAL_DS:  // INTERVAL DAY TO SECOND
      case SQLT_INTERVAL_YM:  // INTERVAL YEAR TO MONTH
         return DB_STRING;
      case SQLT_CLOB:
      case SQLT_BLOB:  // This is binary: DB_TEXT just means LOB.
         return DB_TEXT;
      case SQLT_DAT: case SQLT_DATE: case SQLT_TIMESTAMP:
      case SQLT_TIMESTAMP_TZ:   // TIMESTAMP WITH TIME ZONE
      case SQLT_TIMESTAMP_LTZ:  // TIMESTAMP WITH LOCAL TIME ZONE
         return DB_DATETIME;
      case SQLT_INT: case SQLT_UIN:
         return DB_INTEGER;
      case SQLT_NUM:  // This is actually handled elsewhere.
      case SQLT_FLT:
      case 100:  // BINARY_FLOAT
      case 101:  // BINARY_DOUBLE
         return DB_DOUBLE;      
      default:
         // Binary types not handled above (like BLOB) are DB_BINARY.
         if(isBinaryType(OciDataType)) {
            return BinOkay ? DB_BINARY : DB_STRING;
         }
         COL_ERROR_STREAM( COL_T("OCI data type: ") << OciDataType << newline << COL_T("not supported."), COLerror::SystemError );
   }
}

static sb4 DB2onLobRead
(
   dvoid* pContext, 
   const dvoid* pBuf,
   ub4 len,
   ub1 piece
)
{
   COLstring* pString = (COLstring*)pContext;
   
   pString->write(pBuf,len);
   return OCI_CONTINUE;
}

void DBdatabaseOciOraclePrivate::fillResultSet
( 
   OCIStmt* pStatementHandle,
   COLarray< COLref<DBdatabaseOciOracleBuffer> >& BindVector,
   COLarray< DBdataType >& DbDataTypeVector,
   COLarray< ub2 >& OciTypeVector,
   DBresultSetPtr pResultSet,
   const COLstring& SqlString,
   COLuint32 ipResultSetStart,
   COLuint32 ipResultSetSize
)
{
   sword FetchStatusCode;

   // Note that scrollable cursors are not used here. The issue is unlike most database implementations where
   // cursors take the place of LIMIT X,Y in the sql statements because they are faster, OCI scrollable cursors
   // seem to be implemented over their non scrollable implementations. Which means, doing  a jump to the middle
   // of a result set with cursors is the same as iterating through the result set to the same location without
   // scrolling. 
   //
   // So, if the result set is 10000 rows, and ipResultSetSize is 5000, be prepared to wait until we get to 
   // row 5000, because we have to do the iteration, and its very slow.
   //
   // a workaround is to use ROWNUM as provided in every result set, and use it like LIMIT in other database
   // implementations, and never rely on the start/size parameters of this call.
   //
   // - Ron Cheong

   //move to first row
   COLuint32 CurrentRow = 0;
   while (CurrentRow < ipResultSetStart &&
          (OCI_NO_DATA != (FetchStatusCode = pLoadedOciOracleDll->oci_stmt_fetch_2( pStatementHandle,
                                                          pErrorHandle, 
                                                          1,
                                                          OCI_FETCH_NEXT,
                                                          0,
                                                          OCI_DEFAULT )))
         ){
      CurrentRow++;
      // We have to clear the buffers after each fetch to avoid a pileup.
      for(COLuint32 i = 0; i < pResultSet->countOfColumn(); i++) {
         switch(DbDataTypeVector[i]) {
            case DB_STRING: 
            case DB_BINARY: 
               BindVector[i]->clearBuffer();
            default: 
               break;
         }
      }
   }

   COLuint32 TerminalSize = 0;
   if (!ipResultSetSize) { TerminalSize = (COLuint32)-1; }
   else                  { TerminalSize = ipResultSetStart+ipResultSetSize; }

   for(;  (CurrentRow < TerminalSize) &&
          (OCI_NO_DATA != (FetchStatusCode = pLoadedOciOracleDll->oci_stmt_fetch_2( pStatementHandle,
                                                          pErrorHandle, 
                                                          1,
                                                          OCI_FETCH_NEXT,
                                                          0,
                                                          OCI_DEFAULT )));
          CurrentRow++){
      checkForErrorWithSql( FetchStatusCode, SqlString );
      DBresultSetRow& ResultSetRow = pResultSet->addRow();

      for ( COLuint32 ColumnIndex = 0; ColumnIndex < pResultSet->countOfColumn(); ColumnIndex++ ){
         DBdatabaseOciOracleBuffer& Binding = *BindVector[ColumnIndex];
         if ( Binding.Indicator != -1 ){
            switch(DbDataTypeVector[ColumnIndex]) {
            case DB_STRING:
            {
               const char* pText = (char*)Binding.stringBuffer()->data();
               size_t      Size  =        Binding.bufferSize();
               ResultSetRow.setColumnValue(ColumnIndex, COLstring());
               COLstring& Output = const_cast<COLstring&>(ResultSetRow.columnValue(ColumnIndex).string());
               Output.append(pText,Size);
               Binding.clearBuffer();
               break;
            }
            case DB_BINARY:
            {
               COLstring& Value = *Binding.stringBuffer();
               Value.setCapacity(Binding.bufferSize());
               Value.setSize(Binding.bufferSize());
               ResultSetRow.setColumnValue(ColumnIndex, Value);
               Binding.clearBuffer();
               break;
            }
            case DB_TEXT:
            {
               OCILobLocator* pLobLocator = *Binding.ociLobLocator();
               COLstring String;
               ub1 CharsetForm, ReadBuffer[MAXBUFLEN];
               ub4 AmountRead = 0;
               checkForErrorWithSql( pLoadedOciOracleDll->oci_lob_char_set_form
                  (pEnvironmentHandle, pErrorHandle, pLobLocator, &CharsetForm), SqlString);
               checkForErrorWithSql( pLoadedOciOracleDll->oci_lob_read(pServiceContextHandle,
                                                                  pErrorHandle,
                                                                  pLobLocator,
                                                                  &AmountRead,
                                                                  1,
                                                                  ReadBuffer,
                                                                  sizeof(ReadBuffer),
                                                                  &String,
                                                                  &DB2onLobRead,
                                                                  0,
                                                                  CharsetForm), SqlString);
               if( !pOwner->supportBinaryType() || !isBinaryType(OciTypeVector[ColumnIndex]) ) {
                  ResultSetRow.setColumnValue(ColumnIndex, String);
               } else {
                  ResultSetRow.setColumnValue(ColumnIndex, COLstring());
                  const_cast<DBvariant&>(ResultSetRow.columnValue(ColumnIndex))
                     .binary().write(String.c_str(), String.size());
               }
               break;
            }
            case DB_INTEGER:
            {
               COLint32* pInteger = (COLint32*)Binding.integer();
               ResultSetRow.setColumnValue( ColumnIndex, *pInteger );
               break;
            }
            case DB_DOUBLE:
            {
               COLfloat64* pFloat = (COLfloat64*)Binding.double_();
               ResultSetRow.setColumnValue( ColumnIndex, *pFloat );
               break;
            }
            case DB_DATETIME:
            {
               OCIDate* pOciDate = Binding.ociDate(pEnvironmentHandle);
               COLdateTime DateTimeCOL(pOciDate->OCIDateYYYY, 
                                       pOciDate->OCIDateMM, 
                                       pOciDate->OCIDateDD,
                                       pOciDate->OCIDateTime.OCITimeHH, 
                                       pOciDate->OCIDateTime.OCITimeMI, 
                                       pOciDate->OCIDateTime.OCITimeSS);
               ResultSetRow.setColumnValue( ColumnIndex, DateTimeCOL );
               break;
            }
            default:
               ;
            }
         }
      }      
   }
}

static sb4 dynamicFetchCallback
( 
   dvoid* pContext,
   OCIDefine* pDefine,
   ub4 RowIndex,
   dvoid** ppBuffer,
   ub4** ppBufferSize,
   ub1* pPieceValue,
   dvoid** ppIndicator,
   ub2** ppReturnCode
)
{
   DBdatabaseOciOracleBuffer* pDataBuffer = (DBdatabaseOciOracleBuffer*)pContext;
   COLstring* pCurrentBuffer = pDataBuffer->stringBuffer();
   char *pBuf;

   size_t OldChunk = pDataBuffer->bufferSize();
   size_t NewChunk = OldChunk > 0 ? OldChunk : 1024;
   pCurrentBuffer->setCapacity(OldChunk+NewChunk);
   pCurrentBuffer->setSize(OldChunk+NewChunk);
   pBuf = (char*)pCurrentBuffer->data();
   ::memset(pBuf + OldChunk, 0, NewChunk);

   pDataBuffer->BufferHead = pDataBuffer->bufferSize();
   //pDataBuffer->BufferTail = 0;

   *ppBuffer     = pBuf + OldChunk;
   *ppBufferSize = &(pDataBuffer->BufferTail = NewChunk);
   *ppIndicator  = &(pDataBuffer->Indicator = 0);

   return OCI_CONTINUE;
}

OCIDefine* DBdatabaseOciOraclePrivate::defineColumn
( 
   OCIStmt* pStatementHandle,
   COLuint32 ColumnIndex, 
   ub2 OciDataType,
   DBdatabaseOciOracleBuffer& DataBuffer, 
   DBdataType DbDataType,
   ub2 MaxDataSize,
   const COLstring& SqlString
)
{
   OCIDefine* pDefine = NULL;

   switch(DbDataType) {
   case DB_STRING:
   case DB_BINARY:
      checkForErrorWithSql( pLoadedOciOracleDll->oci_define_by_pos(pStatementHandle,
                                                              &pDefine, 
                                                              pErrorHandle,
                                                              ColumnIndex + 1,
                                                              NULL,
                                                              1024 * 1024, // How many bytes we will accept.
                                                              isBinaryType(OciDataType) ? SQLT_BIN : SQLT_CHR,
                                                              &DataBuffer.Indicator,
                                                              NULL,
                                                              NULL,
                                                              OCI_DYNAMIC_FETCH),
                                                              SqlString );

      checkForErrorWithSql( pLoadedOciOracleDll->oci_define_dynamic(pDefine, 
                                                               pErrorHandle,
                                                               &DataBuffer,
                                                               dynamicFetchCallback),
                                                               SqlString );
      break;
   case DB_TEXT:
      checkForErrorWithSql( pLoadedOciOracleDll->oci_define_by_pos(pStatementHandle,
                                                              &pDefine, 
                                                              pErrorHandle,
                                                              ColumnIndex + 1,
                                                              DataBuffer.ociLobLocator(pEnvironmentHandle),
                                                              sizeof(DataBuffer.ociLobLocator()),
                                                              isBinaryType(OciDataType) ? SQLT_BLOB : SQLT_CLOB,
                                                              &DataBuffer.Indicator,
                                                              NULL,
                                                              NULL,
                                                              OCI_DEFAULT),
                                                              SqlString );
      break;
   case DB_DATETIME:
      checkForErrorWithSql( pLoadedOciOracleDll->oci_define_by_pos(pStatementHandle,
                                                              &pDefine, 
                                                              pErrorHandle,
                                                              ColumnIndex + 1,
                                                              DataBuffer.ociDate(pEnvironmentHandle),
                                                              sizeof(*DataBuffer.ociDate()),
                                                              SQLT_ODT,
                                                              &DataBuffer.Indicator,
                                                              NULL,
                                                              NULL,
                                                              OCI_DEFAULT),
                                                              SqlString );
      break;
   case DB_INTEGER:
      checkForErrorWithSql( pLoadedOciOracleDll->oci_define_by_pos(pStatementHandle,
                                                              &pDefine, 
                                                              pErrorHandle,
                                                              ColumnIndex + 1,
                                                              DataBuffer.integer(),
                                                              sizeof(*DataBuffer.integer()),
                                                              SQLT_INT,
                                                              &DataBuffer.Indicator,
                                                              NULL,
                                                              NULL,
                                                              OCI_DEFAULT),
                                                              SqlString);
      break;
   case DB_DOUBLE:
      checkForErrorWithSql( pLoadedOciOracleDll->oci_define_by_pos(pStatementHandle,
                                                              &pDefine, 
                                                              pErrorHandle,
                                                              ColumnIndex + 1,
                                                              DataBuffer.double_(),
                                                              sizeof(*DataBuffer.double_()),
                                                              SQLT_FLT,
                                                              &DataBuffer.Indicator,
                                                              NULL,
                                                              NULL,
                                                              OCI_DEFAULT),
                                                              SqlString );
      break;
   default:
      COL_ERROR_STREAM( COL_T("DB data type: ") << DbDataType << newline << COL_T("not supported."), COLerror::SystemError );
   }

   return pDefine;
}

class DBdatabaseOciOracleParamHolder{
public:
   DBdatabaseOciOracleParamHolder(DBdatabaseOciOracleDll& Dll) 
      : pHandle(NULL),
        pDll(&Dll)
   {
   }
   ~DBdatabaseOciOracleParamHolder()
   {
      freeHandle();
   }

   sword getStatementParam(CONST dvoid* pStatementHandle,OCIError* pErrorHandle, ub4 ParamIndex);

   dvoid* handle(){return pHandle;}

private:
   void freeHandle()
   {
      if (pHandle != NULL)
      {
         pDll->oci_descriptor_free(pHandle, OCI_DTYPE_PARAM);
         pHandle = NULL;
      }
   }
   DBdatabaseOciOracleDll* pDll;
   dvoid* pHandle; 
};

sword DBdatabaseOciOracleParamHolder::getStatementParam(CONST dvoid* pStatementHandle,OCIError* pErrorHandle, ub4 ParamIndex){
   freeHandle();
   sword Result = pDll->oci_param_get(pStatementHandle, OCI_HTYPE_STMT, pErrorHandle,&pHandle,ParamIndex);
   if (Result != OCI_SUCCESS){
      COLPRECONDITION(pHandle == NULL);
   }
   return Result;
}



DBresultSetPtr DBdatabaseOciOracle::executeSqlString
(
   const COLstring& SqlStringInput,
   COLuint32 ResultSetStart,
   COLuint32 ResultSetSize,
   COLarray<DBresultSetPtr> *pAllResultSets
) const
{
   COL_METHOD(executeSqlString);

   pMember->requireHandles();
   OCIStmt* pStatementHandle = NULL; // local statement handle 

   try{
      pMember->checkForErrorWithSql( pLoadedOciOracleDll->oci_handle_alloc(pMember->pEnvironmentHandle, 
                                                             (dvoid**) &pStatementHandle,
                                                             OCI_HTYPE_STMT,
                                                             0, 
                                                             NULL), SqlStringInput );   

      pMember->checkForErrorWithSql( pLoadedOciOracleDll->oci_stmt_prepare(pStatementHandle, 
                                                             pMember->pErrorHandle, 
                                                             (text*) SqlStringInput.c_str(), 
                                                             SqlStringInput.size(),
                                                             OCI_NTV_SYNTAX, 
                                                             OCI_DEFAULT), SqlStringInput );

      ub2 StatementType;
      pMember->checkForErrorWithSql( pLoadedOciOracleDll->oci_attr_get(pStatementHandle,
                                                                  OCI_HTYPE_STMT,
                                                                  &StatementType,
                                                                  NULL,
                                                                  OCI_ATTR_STMT_TYPE,
                                                                  pMember->pErrorHandle), SqlStringInput );

      if ( StatementType == OCI_STMT_SELECT ){   
         COLarray< COLref<DBdatabaseOciOracleBuffer> > DefineVector;
         COLarray< DBdataType > DbDataTypeVector;
         COLarray< ub2 > OciTypeVector;

         //first execute is to retrieve column names
         pMember->checkForErrorWithSql(pLoadedOciOracleDll->oci_stmt_execute(pMember->pServiceContextHandle, 
                                                                pStatementHandle, 
                                                                pMember->pErrorHandle, 
                                                                0, 
                                                                0,
                                                                NULL, 
                                                                NULL, 
                                                                OCI_DEFAULT), SqlStringInput);//OCI_STMT_SCROLLABLE_READONLY), SqlStringInput ); 
         

         COLuint32 ColumnIndex = 1;    // OCI starts counting select-list columns from 1
   
         text* pColumnName;
         ub2 OciDataType;
         ub2 MaxDataSize;
         
         ub4 SizeOfColumnName;

         DBresultSetPtr pResultSet = new DBresultSet();

         COLarray< ub2 > MaxDataSizeVector; 

         DBdatabaseOciOracleParamHolder ColumnParameter(*pLoadedOciOracleDll);
         while ( OCI_SUCCESS == ColumnParameter.getStatementParam(pStatementHandle,pMember->pErrorHandle,ColumnIndex++)){
            // Retrieve the length of the column name attribute 
            pMember->checkForErrorWithSql( pLoadedOciOracleDll->oci_attr_get(ColumnParameter.handle(), 
                                                               OCI_DTYPE_PARAM, 
                                                               (dvoid**) &pColumnName,
                                                               &SizeOfColumnName, 
                                                               OCI_ATTR_NAME, 
                                                               pMember->pErrorHandle), SqlStringInput );

            // Retrieve the data type attribute 
            pMember->checkForErrorWithSql( pLoadedOciOracleDll->oci_attr_get(ColumnParameter.handle(), 
                                                               OCI_DTYPE_PARAM, 
                                                               (dvoid*) &OciDataType,
                                                               NULL, 
                                                               OCI_ATTR_DATA_TYPE, 
                                                               pMember->pErrorHandle), SqlStringInput );

            // Retrieve maximum data size attribute 
            pMember->checkForErrorWithSql( pLoadedOciOracleDll->oci_attr_get(ColumnParameter.handle(), 
                                                               OCI_DTYPE_PARAM, 
                                                               (dvoid*) &MaxDataSize,
                                                               NULL, 
                                                               OCI_ATTR_DATA_SIZE, 
                                                               pMember->pErrorHandle), SqlStringInput );

            OciTypeVector.push_back(OciDataType);
            if ( OciDataType != SQLT_NUM ) {
               DbDataTypeVector.push_back( dbDataType(OciDataType, supportBinaryType()) );
            } else {
               sb2 OciPrecision;
               sb1 OciScale;
               ub4 SizeOfPrecision = sizeof(OciPrecision);
               
               // Retrieve the length of the column name attribute 
               pMember->checkForErrorWithSql( pLoadedOciOracleDll->oci_attr_get(ColumnParameter.handle(), 
                                                                  OCI_DTYPE_PARAM, 
                                                                  (dvoid*) &OciPrecision,
                                                                  &SizeOfPrecision, 
                                                                  OCI_ATTR_PRECISION, 
                                                                  pMember->pErrorHandle), SqlStringInput );

               if(SizeOfPrecision != sizeof(OciPrecision)){
                  // #10338 - this appears to be happening, and correlates to memory corruption or crashes
                  COL_ERROR_STREAM("OCIAttrGet(OCI_ATTR_PRECISION) size mismatch:"
                     << COL_T(" expected ") << sizeof(OciPrecision) << COL_T(", returned ") << SizeOfPrecision,
                     COLerror::PreCondition);
               }
               

               // Retrieve the data type attribute 
               pMember->checkForErrorWithSql( pLoadedOciOracleDll->oci_attr_get(ColumnParameter.handle(), 
                                                                  OCI_DTYPE_PARAM, 
                                                                  (dvoid*) &OciScale,
                                                                  NULL, 
                                                                  OCI_ATTR_SCALE, 
                                                                  pMember->pErrorHandle),
                                                                  SqlStringInput );
               
               COL_VAR2(OciPrecision, OciScale);
               if (OciPrecision != 0 && OciScale == 0) // NUMBER(?,0)
               {
                  DbDataTypeVector.push_back( DB_INTEGER );
               }
               else  // NUMBER, NUMBER(?,>0), BINARY_FLOAT or BINARY_DOUBLE
               {
                  // Note: for "NUMBER", OciPrecision == 0, OciScale == -127
                  DbDataTypeVector.push_back( DB_DOUBLE );
               }
            }

            // store the datatype and column name
            MaxDataSizeVector.push_back( MaxDataSize );

            COLstring ColumnName = (char*) pColumnName;
            DBdataType DbDataType =  DbDataTypeVector.back();
            if(DbDataType == DB_TEXT && supportBinaryType()) {
               DbDataType = isBinaryType(OciDataType) ? DB_BINARY : DB_TEXT;
            }
            pResultSet->addColumn( ColumnName.substr(0, SizeOfColumnName), DbDataType );   
         }   

         DefineVector.resize( pResultSet->countOfColumn() );
         for ( ColumnIndex = 0; ColumnIndex < pResultSet->countOfColumn(); ColumnIndex++ ){
            DefineVector[ColumnIndex] = new DBdatabaseOciOracleBuffer();
            pMember->defineColumn( pStatementHandle, 
                        ColumnIndex, 
                        OciTypeVector[ColumnIndex],
                        *DefineVector[ColumnIndex], 
                        DbDataTypeVector[ColumnIndex],
                        MaxDataSizeVector[ColumnIndex],
                        SqlStringInput);
         }
      
         pMember->fillResultSet( pStatementHandle, DefineVector, DbDataTypeVector,
            OciTypeVector, pResultSet, SqlStringInput, ResultSetStart, ResultSetSize );
         
         pMember->checkForErrorWithSql( pLoadedOciOracleDll->oci_handle_free(pStatementHandle, OCI_HTYPE_STMT), SqlStringInput );
         return pResultSet;
      } else {
         ub4 ExecuteMode;
         if ( pMember->AutoCommitFlag == true ){ ExecuteMode = OCI_COMMIT_ON_SUCCESS; }
         else                                  { ExecuteMode = OCI_DEFAULT; }

         pMember->checkForErrorWithSql( pLoadedOciOracleDll->oci_stmt_execute(pMember->pServiceContextHandle, 
                                                                pStatementHandle, 
                                                                pMember->pErrorHandle, 
                                                                1, 
                                                                0,
                                                                NULL, 
                                                                NULL, 
                                                                ExecuteMode), SqlStringInput );

         pMember->checkForErrorWithSql( pLoadedOciOracleDll->oci_handle_free(pStatementHandle, OCI_HTYPE_STMT), SqlStringInput );
         return NULL;
      }
   } catch ( COLerror& Error ) {
      pLoadedOciOracleDll->oci_handle_free( pStatementHandle, OCI_HTYPE_STMT );
      throw Error;
   }
}

bool DBdatabaseOciOraclePrivate::didStatementSucceed(sword StatusCode, COLstring& Error){
   switch (StatusCode){
   case OCI_SUCCESS:
   case OCI_SUCCESS_WITH_INFO:
      return true;
      break;         
   case OCI_NEED_DATA:
      Error = "Error - OCI_NEED_DATA";
      return false;
      break;
   case OCI_ERROR:
      Error = getAllErrorMessages(pErrorHandle);
      return false;
      break;
   case OCI_INVALID_HANDLE:
      Error = "Error - OCI_INVALID_HANDLE";
      return false;
      break;
   case OCI_STILL_EXECUTING:
      Error = "Error - OCI_STILL_EXECUTE";
      return false;
      break;
   default:
      Error = "Unrecognized return code.";
      return false;
      break;
   }
}

void DBdatabaseOciOraclePrivate::checkForErrorWithSql(sword StatusCode, const COLstring& SqlStatementString){
   COLstring ErrorString;
   if (!didStatementSucceed(StatusCode, ErrorString)){
      COLostream ErrorStream(ErrorString);    
      ErrorStream << newline << COL_T("Problem in statement: ") << SqlStatementString;
      throw COLerror(ErrorString, COLerror::SystemError);
   }
}  

void DBdatabaseOciOraclePrivate::checkForError( sword StatusCode){
   COLstring ErrorString;
   if (!didStatementSucceed(StatusCode, ErrorString)){
      throw COLerror(ErrorString, COLerror::SystemError);
   }
}

DBresultSetPtr DBdatabaseOciOracle::fetchDatabaseTables() const{
   DBsqlSelect SelectTableNameStatement;
   SelectTableNameStatement.addTable("USER_TABLES");
   SelectTableNameStatement.addColumn("TABLE_NAME");
   return executeSqlCommand(SelectTableNameStatement);
}

DBresultSetPtr DBdatabaseOciOracle::fetchNonSystemDatabaseTables() const{
   // fetchDatabaseTables already returns only user tables
   return fetchDatabaseTables();
}

DBresultSetPtr DBdatabaseOciOracle::fetchDatabaseTableColumns( const char* TableName ) const{
   return NULL;
}

bool DBdatabaseOciOracle::isSystemTable( const char* TableName ) const{
   return false;
}

void DBdatabaseOciOracle::setAutocommitFlag( bool Flag ) const{
   pMember->AutoCommitFlag = Flag;
}

void DBdatabaseOciOracle::commitTransaction(){
   try {
      // TODO - Eliot - bit redundant creating an empty COLstring here - would be better to have a different helper function for non SQL associated errors
      pMember->checkForError(pLoadedOciOracleDll->oci_trans_commit(
         pMember->pServiceContextHandle, pMember->pErrorHandle,OCI_DEFAULT));

      DBdatabase::endTransaction();
   } catch (...) {
      DBdatabase::endTransaction();
      throw;
   }
}

void DBdatabaseOciOracle::rollbackTransaction(){
   try {
      // TODO - Eliot - bit redundant creating an empty COLstring here - would be better to have a different helper function for non SQL associated errors      
      pMember->checkForError(pLoadedOciOracleDll->oci_trans_rollback(
         pMember->pServiceContextHandle, pMember->pErrorHandle, OCI_DEFAULT));

      DBdatabase::endTransaction();
   } catch (...) {
      DBdatabase::endTransaction();
      throw;
   }
}

bool DBdatabaseOciOracle::isConnected(){
   if (!pMember->pServiceContextHandle){
      return false;
   } else {
      //try to issue a sql statement, so our check on the server handle actually means something.
      //this is a very ghetto ping unlike mysql_ping.
      try{
         // previous code decreases database performance (#3347)
         // below is much better solution: 
         executeSqlString("SELECT 0 FROM DUAL");
      } catch(const COLerror&) {
         //ignore the execution error
      }

      OCIServer* pServerHandle = 0;

      try{
         //get server handle
         pMember->checkForErrorWithSql( pLoadedOciOracleDll->oci_attr_get(pMember->pServiceContextHandle,
                                                                     OCI_HTYPE_SVCCTX,
                                                                     &pServerHandle,
                                                                     NULL,
                                                                     OCI_ATTR_SERVER,
                                                                     pMember->pErrorHandle), "SELECT 0 FROM DUAL" );
      } catch(const COLerror&) {
         disconnect(); //force disconnect
         return false;
      }

      if (!pServerHandle){//no server handle, just return false
         disconnect();
         return false;
      }
      //check the connection status
      ub4 ServerStatus = 0;
      pMember->checkForErrorWithSql( pLoadedOciOracleDll->oci_attr_get((dvoid*)pServerHandle, 
                                                         OCI_HTYPE_SERVER, 
                                                         (dvoid*) &ServerStatus,
                                                         (ub4 *)0, 
                                                         OCI_ATTR_SERVER_STATUS, 
                                                         pMember->pErrorHandle), "SELECT 0 FROM DUAL" );
      if(ServerStatus != OCI_SERVER_NORMAL){ //disconnect since the handle is pretty useless at this point.
         disconnect();
         return false;
      }
      return true;
   }
}

DBresultSetPtr DBdatabaseOciOracle::executeSqlInsert( const DBsqlInsert& SqlInsertCommand ) const{
   pMember->requireHandles();
   COLstring SqlString;
   COLostream SqlStream( SqlString );
   createSqlInsert( SqlInsertCommand, SqlStream );
   pMember->executeSqlBind( SqlInsertCommand, SqlString );
   return NULL;
}

DBresultSetPtr DBdatabaseOciOracle::executeSqlUpdate( const DBsqlUpdate& SqlUpdateCommand ) const{
   pMember->requireHandles();
   COLstring SqlString;
   COLostream SqlStream( SqlString );
   createSqlUpdate( SqlUpdateCommand, SqlStream );
   pMember->executeSqlBind( SqlUpdateCommand, SqlString );
   return NULL;
}

COLstring DBdatabaseOciOraclePrivate::getAllErrorMessages(OCIError* pErrorHandle){
   const ub4 SIZE_OF_BUFFER = 512;
   text ErrorBuffer[SIZE_OF_BUFFER];
   // Eliot - let's be paranoid and clear the buffer before we use it
   memset(ErrorBuffer,0,SIZE_OF_BUFFER);
   sb4 ErrorCode = 0;
   COLstring ErrorMessage;      
   ub4 ErrorNumber = 1;

   while (pLoadedOciOracleDll->oci_error_get( pErrorHandle, 
                                                ErrorNumber, 
                                                NULL, 
                                                &ErrorCode, 
                                                ErrorBuffer, 
                                                SIZE_OF_BUFFER, 
                                                OCI_HTYPE_ERROR ) != OCI_NO_DATA){
      ErrorMessage += (char*) ErrorBuffer;
      ErrorMessage += "\n";
      memset(ErrorBuffer,0,SIZE_OF_BUFFER);
      ErrorNumber++;
   }

   return ErrorMessage;
}

void DBdatabaseOciOracle::streamInsertUpdateValue(COLostream& Stream, const DBsqlInsert& SqlCommand, COLuint32 ColumnIndex) const{
   if ( SqlCommand.columnValueIsColumnFlag(ColumnIndex) == false ){
      switch(SqlCommand.columnValue(ColumnIndex).dataType()){
      case DB_DATA_TYPE_NOT_DEFINED:
         streamNullValue(Stream);
         break;
         // As of Oracle 9i, 64-bit integers are not supported, therefore we can't do binding for them.
      case DB_LARGE_INTEGER:
         Stream << SqlCommand.columnValue(ColumnIndex).toString();
         break;
      default:
         {
            COLstring EscapedColumnName = SqlCommand.columnName( ColumnIndex );
            EscapedColumnName.replace( " ", "_" );
            Stream << ':' << EscapedColumnName;
         }
      }
   } else {
      streamName( Stream, SqlCommand.columnValue(ColumnIndex).toString(), SqlCommand.quoteColumnName(ColumnIndex) ); 
   }
}

const COLstring& DBdatabaseOciOracle::dbDoubleType() const{ 
   return DBdatabaseOracle::dbDoubleType();
}

const COLstring& DBdatabaseOciOracle::dbDateTimeType() const{
   return DBdatabaseOracle::dbDateTimeType();
}

const COLstring& DBdatabaseOciOracle::dbTextType() const{
   return DBdatabaseOracle::dbTextType();
}

void DBdatabaseOciOracle::streamCreateTableForeignKeySuffix(COLostream& Stream, const DBsqlCreateTable& SqlCreateTableCommand, COLuint32 ColumnIndex) const{
   DBdatabaseOracle::streamCreateTableForeignKeySuffix( Stream, SqlCreateTableCommand, ColumnIndex );
}

void DBdatabaseOciOracle::streamDateTime( COLostream& Stream, const COLdateTime& DateTime ) const{
   DBdatabaseOracle::streamDateTime( Stream, DateTime );
}

void DBdatabaseOciOracle::streamNullValue( COLostream& Stream ) const{
   DBdatabaseOracle::streamNullValue( Stream );
}

void DBdatabaseOciOracle::createSqlCommandWithoutBinding( const DBsqlCommand& SqlCommand, COLostream& OutputSql ) const{
   COLstring SqlString;
   COLostream StringStream(SqlString);
   switch (SqlCommand.commandType()){
   case DB_SELECT_CMD:
      createSqlSelect( (DBsqlSelect&) SqlCommand, OutputSql );
      break;
   case DB_SELECT_UNION_CMD:
      createSqlSelectUnion( (DBsqlSelectUnion&) SqlCommand, OutputSql );
      break;
   case DB_UPDATE_CMD:
      createSqlUpdate( (DBsqlUpdate&) SqlCommand, StringStream);
      SqlString = pMember->recreateSqlQuery((DBsqlUpdate&)SqlCommand, SqlString);
      OutputSql << SqlString;
      break;
   case DB_INSERT_CMD:
      createSqlInsert((DBsqlInsert&)SqlCommand, StringStream);
      SqlString = pMember->recreateSqlQuery((DBsqlInsert&)SqlCommand, SqlString);
      OutputSql << SqlString;
      break;
   case DB_DELETE_CMD:
      createSqlDelete( (DBsqlDelete&) SqlCommand, OutputSql );
      break;
   case DB_DROP_CMD:
      createSqlDropTable( (DBsqlDropTable&) SqlCommand, OutputSql );
      break;
   case DB_CREATE_TABLE_CMD:
      createSqlCreateTable( (DBsqlCreateTable&) SqlCommand, OutputSql );
      break;
   default:
      throw COLerror( "Specified SQL command not supported.", COLerror::PreCondition );
   }
}

COLstring DBdatabaseOciOraclePrivate::recreateSqlQuery( const DBsqlInsert& SqlCommand, const COLstring& SqlString ){
   COLstring SqlQuery = SqlString;
   for ( COLuint32 ColumnIndex = 0; ColumnIndex < SqlCommand.countOfColumn(); ColumnIndex++ ){
      COLstring ColumnValue;
      COLostream Stream(ColumnValue);
      const DBvariant& VariantValue = SqlCommand.columnValue(ColumnIndex);
      switch(VariantValue.dataType()){
      case DB_STRING:
         pOwner->addEscapedStringToStream( Stream, VariantValue.string() );
         break;
      case DB_DATETIME:
         pOwner->streamDateTime( Stream, VariantValue.dateTime() );
         break;
      default:
         ColumnValue = VariantValue.toString();
      }
      COLstring ColumnName  = SqlCommand.columnName(ColumnIndex);
      ColumnName.prepend(":");
      COLindex Position = SqlQuery.find(ColumnName);
      if (Position != npos){
         SqlQuery.remove(Position, ColumnName.size());
         SqlQuery.insert(Position, ColumnValue);
      }
   }
   return SqlQuery;
}

COLstring DBdatabaseOciOraclePrivate::recreateSqlQuery( const DBsqlUpdate& SqlCommand, const COLstring& SqlString ){
   COLstring SqlQuery = SqlString;
   for ( COLuint32 ColumnIndex = 0; ColumnIndex < SqlCommand.countOfColumn(); ColumnIndex++ ){
      COLstring ColumnValue;
      COLostream Stream(ColumnValue);
      const DBvariant& VariantValue = SqlCommand.columnValue(ColumnIndex);
      switch(VariantValue.dataType()){
      case DB_STRING:
         pOwner->addEscapedStringToStream( Stream, VariantValue.string() );
         break;
      case DB_DATETIME:
         pOwner->streamDateTime( Stream, VariantValue.dateTime() );
         break;
      default:
         ColumnValue = VariantValue.toString();
      }
      COLstring ColumnName  = SqlCommand.columnName(ColumnIndex);
      ColumnName.prepend(":");
      COLindex Position = SqlQuery.find(ColumnName);
      if (Position != npos){
         SqlQuery.remove(Position, ColumnName.size());
         SqlQuery.insert(Position, ColumnValue);
      }
   }
   return SqlQuery;
}

void DBdatabaseOciOracleDll::initVersion(){
   if (!MajorVersion && m_pDll->loaded()){
      if (!m_pDll->canLoadProcAddress("OCIClientVersion")){
         VersionString =  "10.1 or older"; // OCIClientVersion was introduced in 10g Release 2.
         MajorVersion = 10;
         MinorVersion = 1;
      } else {
         sword Major, Minor, Update, Patch, PortUpdate;
         oci_client_version(&Major, &Minor, &Update, &Patch, &PortUpdate);

         MajorVersion = Major;
         MinorVersion = Minor;

         // I don't know where the patch and port-update values would fit.
         COLostream(VersionString) << Major << '.' << Minor << '.' << Update;
      }
   }
}

COLint32 DBdatabaseOciOracleDll::majorVersion(){
   initVersion();
   return MajorVersion;
}

COLint32 DBdatabaseOciOracleDll::minorVersion(){
   initVersion();
   return MinorVersion;
}

const COLstring& DBdatabaseOciOracleDll::versionString(){
   initVersion();
   return VersionString;
}

const COLstring& DBdatabaseOciOracle::loadedApiLibraryPath(){
   COLPRECONDITION(pLoadedOciOracleDll != NULL);
   return pLoadedOciOracleDll->libraryName();
}

const COLstring& DBdatabaseOciOracle::loadedApiVersion(){
   COLPRECONDITION(pLoadedOciOracleDll != NULL);
   return pLoadedOciOracleDll->versionString();
}

void DBdatabaseOciOracle::setParameter(const COLstring& Name, const COLstring& Value){
   if(Name == "UseUnicode"){
      if(Value == "yes"){
         pMember->UseUnicode = true;
         m_pEscapeFunction = &DBescapeSqlWithUnicode;
         return;
      }
      else if(Value == "no" || Value.is_null()){
         pMember->UseUnicode = false;
         m_pEscapeFunction = &DBescapeSql;
         return;
      }
   }

   // otherwise (invalid values and all other cases)
   // this will fall through to the base implementation,
   // which will throw an "unsupported" message
   DBdatabase::setParameter(Name, Value);
}

bool DBdatabaseOciOracle::useUnicode() const{
   return pMember->UseUnicode;
};

static const COLstring DB_DATATYPE_ORACLE_NVARCHAR2 = "NVARCHAR2";
static const COLstring DB_DATATYPE_ORACLE_VARCHAR = "VARCHAR";

const COLstring& DBdatabaseOciOracle::dbStringType() const{
   // Oracle OCI seems to work with Unicode either way,
   // but OCI 9.2 corrupts extended characters when returning data from NVARCHAR2
   // so keeping this at VARCHAR
   return DB_DATATYPE_ORACLE_VARCHAR;
}


COLostream& DBdatabaseOciOracle::addEscapedStringToStream(COLostream& Stream, const COLstring& UnescapedString) const{
   return (*m_pEscapeFunction)(Stream, UnescapedString);
}
