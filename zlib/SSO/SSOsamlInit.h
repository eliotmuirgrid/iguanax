#ifndef _SSOSAML_H
#define _SSOSAML_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOsaml.h
//
//  Description: SSO SSOsaml Authentication Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLwebRequest;
class SCKloop;

void SSOsamlInit(const COLwebRequest& Request, SCKloop* pLoop);

#endif  // end of defensive include
