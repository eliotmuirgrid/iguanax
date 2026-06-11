#ifndef _SSOSERIALIZE_H
#define _SSOSERIALIZE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOserialize.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-09-24, 2:31p.m.
//  ---------------------------------------------------------------------------
#include <SSO/SSOconfig.h>

bool SSOload(SSOconfig& Config, COLstring& Err);
void SSOconfigToVar(const SSOconfig& Config, COLvar& ConfigVar);
void SSOconfigFromVar(const COLvar& Var, SSOconfig& Config);

#endif  // end of defensive include
