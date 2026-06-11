#ifndef _SSOAUTHSET_H
#define _SSOAUTHSET_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOauthSet.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-09-24, 1:32p.m.
//  ---------------------------------------------------------------------------
class COLwebRequest;
class SCKloop;

void SSOauthSet(const COLwebRequest& Request, SCKloop* pLoop);

#endif  // end of defensive include
