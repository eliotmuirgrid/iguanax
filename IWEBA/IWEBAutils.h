#ifndef _IWEBAUTILS_H
#define _IWEBAUTILS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: IWEBAutils.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLstring;

bool IWEBtestSslCertFileValid(const COLstring& SslCertFilePath,       COLstring* pError);
bool IWEBtestSslKeyFileValid (const COLstring& SslPrivateKeyFilePath, COLstring* pError);

#endif  // end of defensive include
