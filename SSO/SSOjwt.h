#ifndef _SSOJWT_H
#define _SSOJWT_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOjwt.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <SSO/SSOconfig.h>

bool SSOverifyToken(const COLstring& Jwt, const COLstring& ClientId, COLvar* pOut, COLstring& Error, SSOprovider Provider);

#endif  // end of defensive include
