#ifndef LNETUTILS_H
#define LNETUTILS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LNETutils.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   18/11/24 3:59 PM
//  ---------------------------------------------------------------------------
class BLUAinstance2;
class COLstring;

bool LNETcheckCertAndPrivateKey(const COLstring& Cert, const COLstring& Key, COLstring* pError);

#endif // end of defensive include
