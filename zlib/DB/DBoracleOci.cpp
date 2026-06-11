//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBoracleOci
//
// Description:
//
// Implementation
//
// Author: Julian Muir
// Date:   May 13, 2021
//
//---------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
// JULIAN COMMENTS ABOUT THE ORACLE DATA IMPORTING/MAPPING WORKING
// - WARNING: The Oracle recommended mappings to external data types do not always give the expected results
//   - If I was being harsh I might even say the are (appear to be) "WRONG"
//     (or it could be my inexperience showing...)
// - Need to look carefully at how to instantiate the variable values for each one
//   - For example might need to set an array to NULL to get the string NULL delimited so it is recognized 
//     correctly (as a null delimited string) by COLstring or COLvar
//     NOTE: This can probably be handled better by (someone with a better understanding of the) correct use 
//           of COLvar etc??? BUT it still needs care and attention to get it right as the Oracle handling
//           is very low-level
//------------------------------------------------------------------------------------------------------------

#include "DBoracleOci.h"
#include "DBdriverOracleOci.h"
#include "DBresultSetPtr.h"
#include "DBoracleType.h"

#include <COL/COLstring.h>
#include <COL/COLvar.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
#include <COL/COLutils.h>
COL_LOG_MODULE;

// JULIAN possibly these 3...

#include <DB/OCIoracleInclude/oci.h>
#include <DB/OCIoracleInclude/ocidem.h>
#include <stdlib.h>

// JULIAN TODO REMOVE LATER -- just testing out string mapping
// #include<string>  
// #include <iostream>

static const int DB_OCI_ONE_THREADED_INIT = 1;
static const int DB_OCI_CHARSET_UTF8 = 871;

//be conservative
#define MAXBUFLEN 5028

DBoracleOci::DBoracleOci() {
   COL_METHOD(DBoracleOci::DBoracleOci);
}

DBoracleOci::~DBoracleOci(){
   COL_METHOD(DBoracleOci::~DBoracleOci);
}

bool DBoracleOci::setDriver(const COLstring& LibraryPath) {
   COL_METHOD(DBoracleOci::setDriver);
   COL_VAR(LibraryPath);

   m_Driver.loadLibrary(LibraryPath);
   if (m_Driver.isLoaded()) {
      COL_VAR2(m_Driver.getLibraryVersionString(), m_Driver.getLibraryVersionUint());
      return true;
   } else {
      COL_ERR("Could not load Oracle OCI Driver!");
      return false;
   }
}

bool DBoracleOci::connect(const COLvar& ConnectionInfo) {
   COL_METHOD(DBoracleOci::connect);
   COL_VAR(ConnectionInfo);

   if (!m_Driver.isLoaded()) {
      COL_TRC("Oracle OCI Driver has not been loaded");
      return false;
   } else {
      COL_TRC("Oracle OCI Driver has been loaded");
   }

   sword Result = m_Driver.oci_env_nls_create(
      &m_pEnvironmentHandle,
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
   COL_VAR((int) Result);
   
   if (OCI_SUCCESS != Result) { 
      COL_TRC("oci_env_nls_create failed"); 
      return false;
   } else { 
      COL_TRC("m_pEnvironmentHandle initialized (oci_env_nls_create succeeded)");
      COL_VAR(m_pEnvironmentHandle); 
   }

   COL_VAR(ConnectionInfo);

   const char* Host     = ConnectionInfo["host"].str().c_str();
   const char* User     = ConnectionInfo["user"].str().c_str();
   const char* Password = ConnectionInfo["password"].str().c_str();

   COL_VAR(Host);
   COL_VAR(User);
   COL_VAR(Password);

   // Note that OCI returns undefined values if an error occurs,
   // so we must use temp local variables
   sword      LogonResult         = 0;
   OCISvcCtx* pTempServiceContext = nullptr;

   // need to initialize m_pErrorHandle for oci_logon 
   if (!m_pErrorHandle){
      m_Driver.oci_handle_alloc(m_pEnvironmentHandle, (dvoid **) &m_pErrorHandle, (ub4) OCI_HTYPE_ERROR, 0, nullptr);
   }
   LogonResult = 999;                      // clear result before using
   LogonResult = m_Driver.oci_logon(
      m_pEnvironmentHandle,
      m_pErrorHandle,
      &pTempServiceContext,                // OCILogon will initialize
      (CONST OraText*) User,
      strlen( User),
      (CONST OraText*) Password,
      strlen( Password),
      (CONST OraText*) Host,               // JULIAN can use a TNS alias name (or connection string)
      strlen( Host)
   );
   COL_VAR(this->getErrorMessages(m_pErrorHandle));

   // Note that OCI returns undefined values if an error occurs,
   // so we must use temp local variables
   m_pServiceContextHandle = pTempServiceContext; // JULIAN TODO needs to be tested as valid before assigning
                                                  // see the old code in DBdatabaseOciOracle::connect
   COL_VAR(LogonResult);
   COL_VAR(m_pServiceContextHandle); // check if pointer initialized
   COL_VAR(pTempServiceContext);     // check if pointer initialized

   if (OCI_SUCCESS != LogonResult) {
      COL_VAR(this->getErrorMessages(m_pErrorHandle));
      return false;
   } 
   // COL_TRC("============================================================================");
   // COLsleep(20000); // JULIAN PUT IN A 20 SECOND DELAY TO TURN OFF SERVER FOR TESTING
   // COL_TRC("============================================================================");
   // JULIAN = FAIL still returns as connected = need to add the query... damn...
   if (isConnected()){
      return true;
   } else {
      return false;
   }
}

// this is the equivalent of MySQL ping function
bool DBoracleOci::isConnected() {
   COL_METHOD(DBoracleOci::isConnected);

   // need to call DBoracleOci::setDriver() first to initialize the driver
   if (!m_Driver.isLoaded()) {
      COL_TRC("hasConnectionByGuid called without loading Oracle driver");
      return false;
   }

   // need to call DBoracleOci::setDriver() first to initialize the context handle
   if (!m_pServiceContextHandle){                               
      COL_TRC("hasConnectionByGuid called without initializing OCI Service Context handle");
      return false;
   } 

   // JULIAN TODO do we add a SQL query like the old code in DBdatabaseOracleOci::hasConnectionByGuid
   // yes it looks like we need to do this as oci_attr_get returns OCI_SERVER_NORMAL
   // but need to write DBoracleOci::execute#SqlString() first 

   // possibly another call rather than a query might also force the status refresh...

   OCIServer* pServerHandleTemp = nullptr;
   //get server handle
   this->m_Driver.oci_attr_get(
      m_pServiceContextHandle,
      OCI_HTYPE_SVCCTX,         // IN Service Handle type
      &pServerHandleTemp,
      nullptr,
      OCI_ATTR_SERVER,          // OUT Server Handle attr
      m_pErrorHandle
   );

   //check the connection status
   ub4 ServerStatus = 0;
   this->m_Driver.oci_attr_get(
      pServerHandleTemp,        // IN server handle on object
      OCI_HTYPE_SERVER,         // use server handle
      (dvoid*) &ServerStatus,
      (ub4 *)0,
      OCI_ATTR_SERVER_STATUS,   // to get server handle status
      m_pErrorHandle            // OUT
   );

   if(OCI_SERVER_NORMAL != ServerStatus) { 
         COLstring Error = getErrorMessages(m_pErrorHandle);
         COL_TRC("Oracle OCI Server handle is invalid - ServerStatus != OCI_SERVER_NORMAL"  << Error);
      return false;
   } else {
      COL_TRC("Oracle OCI Server handle is valid - ServerStatus == OCI_SERVER_NORMAL");
      return true;
   }
}

COLstring DBoracleOci::getErrorMessages(OCIError* pErrorHandle){
   COL_METHOD(DBoracleOci::getErrorMessages);

   if (!pErrorHandle){ 
      COL_TRC("m_pErrorHandle is nullptr (no value was assigned) - check for failed OCI function call");
      return "m_pErrorHandle is nullptr (no value assigned)"; 
   }

   const ub4 SIZE_OF_BUFFER = 512;
   text ErrorBuffer[SIZE_OF_BUFFER];
   // Eliot - let's be paranoid and clear the buffer before we use it
   memset(ErrorBuffer,0,SIZE_OF_BUFFER);
   sb4 ErrorCode = 0;
   COLstring ErrorMessage = "";      
   ub4 ErrorNumber = 1;

   int cnt = 1;
   sword Result = 666; // init to value oraccle does not use
   while (Result != OCI_NO_DATA){
      Result = m_Driver.oci_error_get(m_pErrorHandle, ErrorNumber, nullptr, &ErrorCode, ErrorBuffer, SIZE_OF_BUFFER, OCI_HTYPE_ERROR );
      COL_VAR(Result);
      ErrorMessage += (char*) &ErrorCode;   // include error code
      ErrorMessage += ": ";                 // include error code
      ErrorMessage += (char*) ErrorBuffer;
      ErrorMessage += "\n";
      memset(ErrorBuffer,0,SIZE_OF_BUFFER);
      ErrorNumber++;
      cnt++ ;
      if (cnt == 10) break;                 // max of 10 errors 
   }
   COL_TRC("Error Messages: " << ErrorMessage);
   return ErrorMessage;
}

/*
COLvar ResultSet
ResultSet[ColumnName][<column-index>]
ResultSet[ColumnType][<column-index>]
ResultSet[<row-index>][<column-index]
*/
bool DBoracleOci::executeSqlString(const COLstring& SqlString, COLvar* pResultSet, COLvar* pResultInfo){
   COL_METHOD(DBoracleOci::executeSqlString);

   if (!m_Driver.isLoaded()) {
      COL_TRC("Oracle OCI Driver has not been loaded");
      return false;
   }

   if (!m_pEnvironmentHandle) {
      COL_ERR("Oracle OCI environment handle not initialized ");
      return false;
   }

   if (!m_pServiceContextHandle) {
      COL_ERR("Oracle OCI service context handle not initialized ");
      return false;
   }

//-----------------------------------------------------------------------------
// Steps to execute an SQL statement
// 1 - PREPARE the SQL and get a statement handle
// 2 - (opt) BIND paramter place holders (implement later for speed)
// 3 - EXECUTE the SQL statement
// 4 - DESCRIBE the statement - read meta data (field names and type)
// 5 - DEFINE the output variables (this step links each fields to a variable)
// 6 - FETCH the data (we will do it in blocks of ??? records)
//-----------------------------------------------------------------------------

   OCIStmt* pStatementHandle = nullptr; // local statement handle 
   sword Result;
   Result = m_Driver.oci_handle_alloc(m_pEnvironmentHandle,(dvoid**) &pStatementHandle, OCI_HTYPE_STMT, 0, nullptr);
   if (Result != OCI_SUCCESS){
      COL_ERROR_STREAM(COL_T("Failed to create statement handle - using oci_handle_alloc()") << Result, COLerror::PreCondition);
   }
   
   Result = m_Driver.oci_stmt_prepare_2(m_pServiceContextHandle, &pStatementHandle, m_pErrorHandle, (OraText*) SqlString.c_str(), SqlString.size(), nullptr, 0, OCI_NTV_SYNTAX, OCI_DEFAULT);
   if (Result != OCI_SUCCESS){
      COL_ERROR_STREAM(COL_T("Failed to prepare statement - using oci_stmt_prepare_2() ") << Result, COLerror::PreCondition);
   }

   // JULIAN I thought that oci_stmt_prepare_2 was a problem and this was working - but oci_stmt_prepare_2 is fine now...
   // Result = m_Driver.oci_stmt_prepare(pStatementHandle, m_pErrorHandle, (OraText*) SqlString.c_str(), SqlString.size(), OCI_NTV_SYNTAX, OCI_DEFAULT);
   // if (Result != OCI_SUCCESS){
   //    COL_ERROR_STREAM(COL_T("Failed to prepare statement - using oci_stmt_prepare_2() ") << Result, COLerror::PreCondition);
   // }

   ub2 StatementType;
   m_Driver.oci_attr_get(pStatementHandle, OCI_HTYPE_STMT, &StatementType, nullptr, OCI_ATTR_STMT_TYPE, m_pErrorHandle);
   if (OCI_STMT_SELECT != StatementType){   
      return false;
   } else {
      // gets data and column info
      m_Driver.oci_stmt_execute(m_pServiceContextHandle, pStatementHandle, m_pErrorHandle, 0, 0, nullptr, nullptr, OCI_DEFAULT); 
      // JULIAN trying to return 5 rows does not work it does not like it (ORA-24338: statement handle not executed)
      // m_Driver.oci_stmt_execute(m_pServiceContextHandle, pStatementHandle, m_pErrorHandle, 5, 0, nullptr, nullptr, OCI_DEFAULT); 

      COLuint32  ColumnIndex = 1;        // OCI starts counting select-list columns from 1   
      OraText*   pColumnName;            // resolves to char*
      ub2        OciDataType;
      ub2        MaxDataSize;            // JULIAN Nic thought not needed??? (COLstring will automatically find size of OraText*/char*)
      ub4        SizeOfColumnName;       // JULIAN maybe also not needed???  (COLstring will automatically find size of OraText*/char*)
      sb4        ParamStatus;
      dvoid*     pParamDescriptorHandle;

      COLvar     ResultInfo;
      ResultInfo.setArrayType();
      ParamStatus = m_Driver.oci_param_get(pStatementHandle, OCI_HTYPE_STMT, m_pErrorHandle, &pParamDescriptorHandle, ColumnIndex);
      COL_VAR(ParamStatus); 
      COL_VAR(getErrorMessages(m_pErrorHandle));

      // extract column meta data
      while (OCI_SUCCESS==ParamStatus){
         COL_TRC("============================get column data type===============================");
         // get column data type
         m_Driver.oci_attr_get(pParamDescriptorHandle, OCI_DTYPE_PARAM, &OciDataType, nullptr, OCI_ATTR_DATA_TYPE, m_pErrorHandle);
         COL_TRC("============================get column name & size===============================");
         // get column name & size
         m_Driver.oci_attr_get(pParamDescriptorHandle, OCI_DTYPE_PARAM, &pColumnName, &SizeOfColumnName, OCI_ATTR_NAME, m_pErrorHandle);

         COL_TRC("============================get column name & type===============================");
         COL_TRC(OciDataType); 
         COL_TRC((char*) pColumnName); 

         COLstring OciConstant;
         ResultInfo["ColumnName"][ColumnIndex-1] = (char*) pColumnName;
         ResultInfo["ColumnType"][ColumnIndex-1] = DBoracleOciTypeAsString((oci_field_types)OciDataType, OciConstant);
         COL_TRC("=================================");
         ColumnIndex++;
         ParamStatus = m_Driver.oci_param_get(pStatementHandle, OCI_HTYPE_STMT, m_pErrorHandle, &pParamDescriptorHandle, ColumnIndex);
         COL_TRC("===========================================================");
      }
      COL_VAR(ResultInfo);

      *pResultInfo = ResultInfo;
      COL_VAR(*pResultInfo);


      //  JULIAN NEED TO ADD THE DATA RETRIEVAL HERE AS WELL - after the field info
      COLvar ResultSet;

      // Define the select list items 

      // try with fixed array first and then use data returned
      // JULIAN had to initialize the arrays as the oci_stmt_fetch_2 just writes data and does not terminate the string...
      //        and COL_VAR looks for a terminator...
      char country_id[100][3];
      char country_name[100][41];
      // unsigned char country_region[100][22] = {NULL, NULL, NULL, NULL, NULL}; // using types from "Program Variable" column in Oracle external data types docs table
      char country_region[100][22]; // using types from "Program Variable" column in Oracle external data types docs table
      COL_VAR(*country_id);
      COL_VAR(*country_name);
      COL_TRC_DUMP("country_region", *country_region, 100);
      COL_TRC("==========================111111111111=================================");

      // Fetched data indicators, lengths and codes
      sb2 country_id_ind[100],  country_name_ind[100],  country_region_ind[100];
      ub2 country_id_len[100],  country_name_len[100],  country_region_len[100];
      ub2 country_id_code[100], country_name_code[100], country_region_code[100];

      OCIDefine* pDefineHandle = nullptr;
      m_Driver.oci_define_by_pos(pStatementHandle, &pDefineHandle,       m_pErrorHandle,    1,              (void*)country_id,      3,              SQLT_AFC, (void*)country_id_ind,     country_id_len,     country_id_code,     OCI_DEFAULT);
      m_Driver.oci_define_by_pos(pStatementHandle, &pDefineHandle,       m_pErrorHandle,    2,              (void*)country_name,   41,              SQLT_CHR, (void*)country_name_ind,   country_name_len,   country_name_code,   OCI_DEFAULT);
      // JULIAN datatype reported Oracle does not work (SQLT_NUM) used SQLT_LNG by accident and it works...
      m_Driver.oci_define_by_pos(pStatementHandle, &pDefineHandle,       m_pErrorHandle,    3,              (void*)country_region, 22,              SQLT_CHR, (void*)country_region_ind, country_region_len, country_region_code, OCI_DEFAULT);
      // m_Driver.oci_define_by_pos(pStatementHandle, &pDefineHandle,       m_pErrorHandle,    3,              (void*)country_region, 21,              SQLT_NUM, (void*)country_region_ind, country_region_len, country_region_code, OCI_DEFAULT);

// sword OCIDefineByPos (        OCIStmt *stmtp, 1 OCIDefine **defnpp,2  OCIError *errhp,3  ub4 position,4  void  *valuep,5       sb4 value_sz,6    ub2 dty,7 void *indp,8                ub2 *rlenp,9        ub2 *rcodep,10      ub4 mode11 );
// rc = OCIDefineByPos(                   stmtp,              &defnpp,             errhp,   1,              (void*)owner,         31,               SQLT_STR, (void*)owner_ind,	        owner_len,          owner_code,         OCI_DEFAULT);
 
m_Driver.oci_stmt_fetch_2( pStatementHandle, m_pErrorHandle,          5,    OCI_DEFAULT,      0,                OCI_DEFAULT );
// sword OCIStmtFetch2   ( OCIStmt  *stmthp, OCIError *errhp, ub4 nrows,    ub2 orientation,  sb4  fetchOffset, ub4 mode );

    // rc = OCIStmtFetch2(stmtp,            errhp,                  100,    OCI_DEFAULT,      0,                OCI_DEFAULT);

      COL_TRC_DUMP("country_id", country_id, 100);
      COL_TRC_DUMP("country_region", *country_region, 100);
      COL_TRC_DUMP("country_name", *country_name, 100);
      COL_TRC("==========================AAAAAAAAAAAAAAAA=================================");
      COL_VAR(country_id[0]);
      COL_VAR(country_name[0]);
      COL_VAR(country_region[0]);          // JULIAN displays the pointer address
      COL_VAR(*country_region[0]);         // JULIAN need to convert to string

      COL_VAR(country_region_ind[0]);
      COL_VAR(country_region_len[0]);
      COL_VAR(country_region_code[0]);

      COL_VAR(country_id_ind[0]);
      COL_VAR(country_id_len[0]);
      COL_VAR(country_id_code[0]);
      COL_TRC("==========================AAAAAAAAAAAAAAAA=================================");
      COL_VAR(country_id[1]);
      COL_VAR(country_name[1]);
      COL_VAR(country_region[1]);          // JULIAN displays the pointer address
      COL_VAR(*country_region[1]);         // JULIAN need to convert to string

      COL_VAR(country_region_ind[1]);
      COL_VAR(country_region_len[1]);
      COL_VAR(country_region_code[1]);

      COL_VAR(country_id_ind[1]);
      COL_VAR(country_id_len[1]);
      COL_VAR(country_id_code[1]);



      COL_TRC("==========================AAAAAAAAAAAAAAAA=================================");
      COL_VAR(country_id[2]);
      COL_VAR(country_name[2]);
      COL_VAR(country_region[2]);
      COL_VAR(*country_region[2]);         // JULIAN need to convert to string

      COL_VAR(country_region_ind[2]);
      COL_VAR(country_region_len[2]);
      COL_VAR(country_region_code[2]);

      COL_VAR(country_id_ind[2]);
      COL_VAR(country_id_len[2]);
      COL_VAR(country_id_code[2]);

      COL_TRC("==========================AAAAAAAAAAAAAAAA=================================");
      COL_VAR(country_id[3]);
      COL_VAR(country_name[3]);
      COL_VAR(country_region[3]);
      COL_VAR(*country_region[3]);         // JULIAN need to convert to string

      COL_VAR(country_region_ind[3]);
      COL_VAR(country_region_len[3]);
      COL_VAR(country_region_code[3]);

      COL_VAR(country_id_ind[3]);
      COL_VAR(country_id_len[3]);
      COL_VAR(country_id_code[3]);

      COL_TRC("==========================AAAAAAAAAAAAAAAA=================================");
      COL_VAR(country_id[4]);
      COL_VAR(country_name[4]);
      COL_VAR(country_region[4]);
      COL_VAR(*country_region[4]);         // JULIAN need to convert to string

      COL_VAR(country_region_ind[4]);
      COL_VAR(country_region_len[4]);
      COL_VAR(country_region_code[4]);

      COL_VAR(country_id_ind[4]);
      COL_VAR(country_id_len[4]);
      COL_VAR(country_id_code[4]);

      COL_TRC("==========================AAAAAAAAAAAAAAAA=================================");
      COL_VAR(country_id[5]);
      COL_VAR(country_name[5]);
      COL_VAR(country_region[5]);
      COL_VAR(*country_region[5]);         // JULIAN need to convert to string

      COL_VAR(country_region_ind[5]);
      COL_VAR(country_region_len[5]);
      COL_VAR(country_region_code[5]);

      COL_VAR(country_id_ind[5]);
      COL_VAR(country_id_len[5]);
      COL_VAR(country_id_code[5]);



      *pResultSet = ResultSet;
      *pResultInfo = ResultInfo;
      COL_VAR(*pResultInfo);
      return true;
   }

      // OCIParam     *mypard;
      // ub4          counter;        // ColumnIndex
      // ub2          dtype;
      // text         *col_name;   
      // ub4          col_name_len;
      // sb4          parm_status;    // ParamStatus
      // ...
      // /* Request a parameter descriptor for position 1 in the select-list */
      // counter = 1;
      // parm_status = OCIParamGet(stmthp, OCI_HTYPE_STMT, errhp, &mypard, (ub4) counter);
      // /* Loop only if a descriptor was successfully retrieved for
      //    current position, starting at 1 */
      // while (parm_status==OCI_SUCCESS) {
      // /* Retrieve the data type attribute */
      // checkerr(errhp, OCIAttrGet((dvoid*) mypard, (ub4) OCI_DTYPE_PARAM, 
      //                (dvoid*) &dtype,(ub4 *) 0, (ub4) OCI_ATTR_DATA_TYPE, 
      //                (OCIError *) errhp  ));
      // /* Retrieve the column name attribute */
      // checkerr(errhp, OCIAttrGet((dvoid*) mypard, (ub4) OCI_DTYPE_PARAM, 
      //       (dvoid**) &col_name,(ub4 *) &col_name_len, (ub4) OCI_ATTR_NAME, 
      //       (OCIError *) errhp ));
      // printf("column=%s  datatype=%d\n\n", col_name, dtype);
      // fflush(stdout);
      // /* increment counter and get next descriptor, if there is one */
      // counter++;
      // parm_status = OCIParamGet(stmthp, OCI_HTYPE_STMT, errhp, &mypard,
      //             (ub4) counter);
      // }




      // get the column types -> put into the pResultInfo
      // loop through the fields

      // get the data -> put into the pResultSet
      // loop through the rows
         // loop through the fields 








////  pMember->requireHandles(); // JULIAN not needed 
////  OCIStmt* pStatementHandle = N#ULL; // local statement handle 
////  try{
////       pMember->checkSql( pOciDll->oci_handle_alloc(pMember->pEnvironmentHandle,(dvoid**) &pStatementHandle, OCI_HTYPE_STMT, 0, N#ULL), SqlStringInput );   
////       pMember->checkSql( pOciDll->oci#_stmt_prepare(pStatementHandle, pMember->pErrorHandle, (OraText*) SqlStringInput.c_str(), 
////                                                              SqlStringInput.size(), OCI_NTV_SYNTAX,OCI_DEFAULT), SqlStringInput );

////       ub2 StatementType;
////       pMember->checkSql( pOciDll->oci_attr_get(pStatementHandle,OCI_HTYPE_STMT,&StatementType,N#ULL,OCI_ATTR_STMT_TYPE,pMember->pErrorHandle), SqlStringInput );

///////////////////////////////// ============================================
//       if ( StatementType == OCI_STMT_SELECT ){   
//          COLarray< COLref<DBdatabaseOracleOciBuffer> > DefineVector;
//          COLarray< DBdataType > DbDataTypeVector;
//          COLarray< ub2 > OciTypeVector;
//          //first execute is to retrieve column names
//          pMember->checkSql(pOciDll->oci_stmt_execute(pMember->pServiceContextHandle, pStatementHandle,pMember->pErrorHandle, 0,0,N#ULL,N#ULL,OCI_DEFAULT), SqlStringInput);//OCI_STMT_SCROLLABLE_READONLY), SqlStringInput ); 
//          COLuint32 ColumnIndex = 1;    // OCI starts counting select-list columns from 1   
//          OraText* pColumnName;
//          ub2 OciDataType;
//          ub2 MaxDataSize;
//          ub4 SizeOfColumnName;
//          DBresultSetPtr pResultSet = new DBresultSet();
//          COLarray< ub2 > MaxDataSizeVector; 
//          DBdatabaseOracleOciParamHolder ColumnParameter(*pOciDll);
///////////////////////////////// ============================================



//          while ( OCI_SUCCESS == ColumnParameter.getStatementParam(pStatementHandle,pMember->pErrorHandle,ColumnIndex++)){
//             // Retrieve the length of the column name attribute 
//             pMember->checkSql( pOciDll->oci_attr_get(ColumnParameter.handle(),OCI_DTYPE_PARAM,(dvoid**) &pColumnName,&SizeOfColumnName,OCI_ATTR_NAME,pMember->pErrorHandle), SqlStringInput);
//             // Retrieve the data type attribute 
//             pMember->checkSql( pOciDll->oci_attr_get(ColumnParameter.handle(),OCI_DTYPE_PARAM,(dvoid*) &OciDataType,N#ULL,OCI_ATTR_DATA_TYPE, pMember->pErrorHandle), SqlStringInput);
//             // Retrieve maximum data size attribute 
//             pMember->checkSql( pOciDll->oci_attr_get(ColumnParameter.handle(),OCI_DTYPE_PARAM, (dvoid*) &MaxDataSize,N#ULL,OCI_ATTR_DATA_SIZE, pMember->pErrorHandle), SqlStringInput);

//             OciTypeVector.push_back(OciDataType);
//             if ( OciDataType != SQLT_NUM ){
//                DbDataTypeVector.push_back( dbDataType(OciDataType, supportBinaryType()) );
//             } else {
//                sb2 OciPrecision;
//                sb1 OciScale;
//                ub4 SizeOfPrecision = sizeof(OciPrecision);
               
//                // Retrieve the length of the column name attribute 
//                pMember->checkSql( pOciDll->oci_attr_get(ColumnParameter.handle(), OCI_DTYPE_PARAM,(dvoid*) &OciPrecision,&SizeOfPrecision, OCI_ATTR_PRECISION, pMember->pErrorHandle), SqlStringInput );
//                if(SizeOfPrecision != sizeof(OciPrecision)){    // #10338 - this appears to be happening, and correlates to memory corruption or crashes
//                   COL_ERROR_STREAM("OCIAttrGet(OCI_ATTR_PRECISION) size mismatch:" << COL_T(" expected ") << sizeof(OciPrecision) << COL_T(", returned ") << SizeOfPrecision,  COLerror::PreCondition);
//                }
//                // Retrieve the data type attribute 
//                pMember->checkSql( pOciDll->oci_attr_get(ColumnParameter.handle(),OCI_DTYPE_PARAM,(dvoid*) &OciScale,N#ULL,OCI_ATTR_SCALE,pMember->pErrorHandle),SqlStringInput);
//                COL_VAR2(OciPrecision, OciScale);
//                if (OciPrecision != 0 && OciScale == 0){ // NUMBER(?,0)
//                   DbDataTypeVector.push_back( DB_INTEGER );
//                } else { // NUMBER, NUMBER(?,>0), BINARY_FLOAT or BINARY_DOUBLE
//                   // Note: for "NUMBER", OciPrecision == 0, OciScale == -127
//                   DbDataTypeVector.push_back( DB_DOUBLE );
//                }
//             }
//             // store the datatype and column name
//             MaxDataSizeVector.push_back( MaxDataSize );
//             COLstring ColumnName = (char*) pColumnName;
//             DBdataType DbDataType =  DbDataTypeVector.back();
//             if(DbDataType == DB_TEXT && supportBinaryType()) {
//                DbDataType = isBinaryType(OciDataType) ? DB_BINARY : DB_TEXT;
//             }
//             pResultSet->addColumn( ColumnName.substr(0, SizeOfColumnName), DbDataType );   
//          }   
//          DefineVector.resize( pResultSet->countOfColumn() );
//          for ( ColumnIndex = 0; ColumnIndex < pResultSet->countOfColumn(); ColumnIndex++ ) {
//             DefineVector[ColumnIndex] = new DBdatabaseOracleOciBuffer();
//             pMember->defineColumn( pStatementHandle,ColumnIndex,OciTypeVector[ColumnIndex],*DefineVector[ColumnIndex],DbDataTypeVector[ColumnIndex],MaxDataSizeVector[ColumnIndex],SqlStringInput);
//          }
//          pMember->fillResultSet( pStatementHandle, DefineVector, DbDataTypeVector, OciTypeVector, pResultSet, SqlStringInput, ResultSetStart, ResultSetSize );
//          pMember->checkSql( pOciDll->oci_handle_free(pStatementHandle, OCI_HTYPE_STMT), SqlStringInput );
//          return pResultSet;
//       } else {
//          ub4 ExecuteMode;
//          if ( pMember->AutoCommitFlag == true ) {
//             ExecuteMode = OCI_COMMIT_ON_SUCCESS;
//          } else {
//             ExecuteMode = OCI_DEFAULT;
//          }
//          pMember->checkSql( pOciDll->oci_stmt_execute(pMember->pServiceContextHandle,pStatementHandle,pMember->pErrorHandle,1,0,N#ULL,N#ULL,ExecuteMode), SqlStringInput );
//          pMember->checkSql( pOciDll->oci_handle_free(pStatementHandle, OCI_HTYPE_STMT), SqlStringInput );
//          return N#ULL;
//       }
//    } catch ( COLerror& Error ){
//       pOciDll->oci_handle_free( pStatementHandle, OCI_HTYPE_STMT );
//       throw Error;
//    }
}

// DBresultSetPtr DBdatabaseOracleOci::executeSqlString(const COLstring& SqlStringInput, COLuint32 ResultSetStart, COLuint32 ResultSetSize, COLarray<DBresultSetPtr> *pAllResultSets) const{
//    COL_METHOD(DBoracleOci::executeSqlString);
//    pMember->requireHandles();
//    OCIStmt* pStatementHandle = N#ULL; // local statement handle 
//    try{
//       pMember->checkSql( pOciDll->oci_handle_alloc(pMember->pEnvironmentHandle,(dvoid**) &pStatementHandle, OCI_HTYPE_STMT, 0, N#ULL), SqlStringInput );   
//       pMember->checkSql( pOciDll->oci#_stmt_prepare(pStatementHandle, pMember->pErrorHandle, (OraText*) SqlStringInput.c_str(), 
//                                                              SqlStringInput.size(), OCI_NTV_SYNTAX,OCI_DEFAULT), SqlStringInput );

//       ub2 StatementType;
//       pMember->checkSql( pOciDll->oci_attr_get(pStatementHandle,OCI_HTYPE_STMT,&StatementType,N#ULL,OCI_ATTR_STMT_TYPE,pMember->pErrorHandle), SqlStringInput );

//       if ( StatementType == OCI_STMT_SELECT ){   
//          COLarray< COLref<DBdatabaseOracleOciBuffer> > DefineVector;
//          COLarray< DBdataType > DbDataTypeVector;
//          COLarray< ub2 > OciTypeVector;
//          //first execute is to retrieve column names
//          pMember->checkSql(pOciDll->oci_stmt_execute(pMember->pServiceContextHandle, pStatementHandle,pMember->pErrorHandle, 0,0,N#ULL,N#ULL,OCI_DEFAULT), SqlStringInput);//OCI_STMT_SCROLLABLE_READONLY), SqlStringInput ); 
//          COLuint32 ColumnIndex = 1;    // OCI starts counting select-list columns from 1   
//          OraText* pColumnName;
//          ub2 OciDataType;
//          ub2 MaxDataSize;
//          ub4 SizeOfColumnName;
//          DBresultSetPtr pResultSet = new DBresultSet();
//          COLarray< ub2 > MaxDataSizeVector; 
//          DBdatabaseOracleOciParamHolder ColumnParameter(*pOciDll);
//          while ( OCI_SUCCESS == ColumnParameter.getStatementParam(pStatementHandle,pMember->pErrorHandle,ColumnIndex++)){
//             // Retrieve the length of the column name attribute 
//             pMember->checkSql( pOciDll->oci_attr_get(ColumnParameter.handle(),OCI_DTYPE_PARAM,(dvoid**) &pColumnName,&SizeOfColumnName,OCI_ATTR_NAME,pMember->pErrorHandle), SqlStringInput);
//             // Retrieve the data type attribute 
//             pMember->checkSql( pOciDll->oci_attr_get(ColumnParameter.handle(),OCI_DTYPE_PARAM,(dvoid*) &OciDataType,N#ULL,OCI_ATTR_DATA_TYPE, pMember->pErrorHandle), SqlStringInput);
//             // Retrieve maximum data size attribute 
//             pMember->checkSql( pOciDll->oci_attr_get(ColumnParameter.handle(),OCI_DTYPE_PARAM, (dvoid*) &MaxDataSize,N#ULL,OCI_ATTR_DATA_SIZE, pMember->pErrorHandle), SqlStringInput);

//             OciTypeVector.push_back(OciDataType);
//             if ( OciDataType != SQLT_NUM ){
//                DbDataTypeVector.push_back( dbDataType(OciDataType, supportBinaryType()) );
//             } else {
//                sb2 OciPrecision;
//                sb1 OciScale;
//                ub4 SizeOfPrecision = sizeof(OciPrecision);
               
//                // Retrieve the length of the column name attribute 
//                pMember->checkSql( pOciDll->oci_attr_get(ColumnParameter.handle(), OCI_DTYPE_PARAM,(dvoid*) &OciPrecision,&SizeOfPrecision, OCI_ATTR_PRECISION, pMember->pErrorHandle), SqlStringInput );
//                if(SizeOfPrecision != sizeof(OciPrecision)){    // #10338 - this appears to be happening, and correlates to memory corruption or crashes
//                   COL_ERROR_STREAM("OCIAttrGet(OCI_ATTR_PRECISION) size mismatch:" << COL_T(" expected ") << sizeof(OciPrecision) << COL_T(", returned ") << SizeOfPrecision,  COLerror::PreCondition);
//                }
//                // Retrieve the data type attribute 
//                pMember->checkSql( pOciDll->oci_attr_get(ColumnParameter.handle(),OCI_DTYPE_PARAM,(dvoid*) &OciScale,N#ULL,OCI_ATTR_SCALE,pMember->pErrorHandle),SqlStringInput);
//                COL_VAR2(OciPrecision, OciScale);
//                if (OciPrecision != 0 && OciScale == 0){ // NUMBER(?,0)
//                   DbDataTypeVector.push_back( DB_INTEGER );
//                } else { // NUMBER, NUMBER(?,>0), BINARY_FLOAT or BINARY_DOUBLE
//                   // Note: for "NUMBER", OciPrecision == 0, OciScale == -127
//                   DbDataTypeVector.push_back( DB_DOUBLE );
//                }
//             }
//             // store the datatype and column name
//             MaxDataSizeVector.push_back( MaxDataSize );
//             COLstring ColumnName = (char*) pColumnName;
//             DBdataType DbDataType =  DbDataTypeVector.back();
//             if(DbDataType == DB_TEXT && supportBinaryType()) {
//                DbDataType = isBinaryType(OciDataType) ? DB_BINARY : DB_TEXT;
//             }
//             pResultSet->addColumn( ColumnName.substr(0, SizeOfColumnName), DbDataType );   
//          }   
//          DefineVector.resize( pResultSet->countOfColumn() );
//          for ( ColumnIndex = 0; ColumnIndex < pResultSet->countOfColumn(); ColumnIndex++ ) {
//             DefineVector[ColumnIndex] = new DBdatabaseOracleOciBuffer();
//             pMember->defineColumn( pStatementHandle,ColumnIndex,OciTypeVector[ColumnIndex],*DefineVector[ColumnIndex],DbDataTypeVector[ColumnIndex],MaxDataSizeVector[ColumnIndex],SqlStringInput);
//          }
//          pMember->fillResultSet( pStatementHandle, DefineVector, DbDataTypeVector, OciTypeVector, pResultSet, SqlStringInput, ResultSetStart, ResultSetSize );
//          pMember->checkSql( pOciDll->oci_handle_free(pStatementHandle, OCI_HTYPE_STMT), SqlStringInput );
//          return pResultSet;
//       } else {
//          ub4 ExecuteMode;
//          if ( pMember->AutoCommitFlag == true ) {
//             ExecuteMode = OCI_COMMIT_ON_SUCCESS;
//          } else {
//             ExecuteMode = OCI_DEFAULT;
//          }
//          pMember->checkSql( pOciDll->oci_stmt_execute(pMember->pServiceContextHandle,pStatementHandle,pMember->pErrorHandle,1,0,N#ULL,N#ULL,ExecuteMode), SqlStringInput );
//          pMember->checkSql( pOciDll->oci_handle_free(pStatementHandle, OCI_HTYPE_STMT), SqlStringInput );
//          return N#ULL;
//       }
//    } catch ( COLerror& Error ){
//       pOciDll->oci_handle_free( pStatementHandle, OCI_HTYPE_STMT );
//       throw Error;
//    }
// }

