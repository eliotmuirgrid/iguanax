//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: IWEBAutils.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <FIL/FILutils.h>
#include <IWEBA/IWEBAutils.h>
#include <SCKS/SCKSsslContext.h>
COL_LOG_MODULE;

bool IWEBtestSslFileValid(const COLstring& SslFilePath, COLstring* pError){
   COL_FUNCTION(IWEBtestSslFileValid);
   if(SslFilePath.is_null()){
      *pError = "Filepath cannot be empty.";
      COL_VAR(*pError);
      return false;
   }
   if (!FILfileExists(SslFilePath)){
      *pError = "File does not exist: '" + SslFilePath + '\'';
      COL_VAR(*pError);
      return false;
   }
   if (FILisDirectory(SslFilePath)){
      *pError = "File is a directory rather than a file: '" + SslFilePath + '\'';
      COL_VAR(*pError);
      return false;
   }
   return true;
}

bool IWEBtestSslCertFileValid(const COLstring& SslCertFilePath, COLstring* pError){
   COL_FUNCTION(IWEBtestSslCertFileValid);
   COL_VAR(SslCertFilePath);
   if (!IWEBtestSslFileValid(SslCertFilePath, pError)){ return false; }
   SCKSsslContext Context;
   Context.init();
   if (Context.setCertificate(SslCertFilePath) != 0){
      *pError = "'" + SslCertFilePath + "' is not a valid certificate";
      COL_VAR(*pError);
      return false;
   }
   return true;
}

bool IWEBtestSslKeyFileValid(const COLstring& SslPrivateKeyFilePath, COLstring* pError){
   COL_FUNCTION(IWEBtestSslKeyFileValid);
   COL_VAR(SslPrivateKeyFilePath);
   if (!IWEBtestSslFileValid(SslPrivateKeyFilePath, pError)){ return false; }
   SCKSsslContext Context;
   Context.init();
   if (Context.setPrivateKey(SslPrivateKeyFilePath) != 0){
      *pError = "'" + SslPrivateKeyFilePath + "' is not a valid private key";
      COL_VAR(*pError);
      return false;
   }
   return true;
}