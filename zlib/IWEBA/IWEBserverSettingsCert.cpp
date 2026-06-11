//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: IWEBserverSettingsCert.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <IWEBA/IWEBAutils.h>
#include <IWEBA/IWEBserverSettingsCert.h>
COL_LOG_MODULE;

// /web/server/certificate/check
void IWEBserverCertificate(const COLwebRequest& Request) {
   COL_FUNCTION(IWEBserverCertificate);
   const COLvar& Input = Request.Data;
   COL_VAR(Input.json(1));
   COLvar          Result;
   const COLstring CertificateFile = COLvarRequiredString(Input, "file");
   COLstring       Error;
   IWEBtestSslCertFileValid(CertificateFile, &Error) ? COLrespondSuccess(Request.Address)
                                                           : COLrespondError(Request.Address, Error);
}