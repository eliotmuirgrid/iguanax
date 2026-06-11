#ifndef __ENV_ENVPARSE_H__
#define __ENV_ENVPARSE_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ENVparse.h
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/24/23 11:08 AM
//  ---------------------------------------------------------------------------
#include <COL/COLmap.h>

class COLstring;

struct ENVmap {
   COLmap<COLstring, COLstring> Map;
};

void ENVparse(COLstring* pEnv, ENVmap* pEnvMap, COLstring* pErr);

#endif // end of defensive include