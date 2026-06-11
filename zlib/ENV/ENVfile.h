#ifndef __ENV_ENVFILE_H__
#define __ENV_ENVFILE_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ENVsave.h
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/27/23 12:54 PM
//  ---------------------------------------------------------------------------
struct ENVmap;
class COLvar;

void ENVfileToVar(const ENVmap& EnvMap, COLvar* pOut);
void ENVfileLoad(ENVmap* pEnvMap);
void ENVfileSave(const ENVmap& EnvMap);

#endif // end of defensive include
