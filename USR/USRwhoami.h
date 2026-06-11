#ifndef _USRWHOAMI_H
#define _USRWHOAMI_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: USRwhoami.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLmap.h>

class COLwebRequest;
class USRuser;

void USRwhoami(const COLwebRequest& Request, COLmap<COLstring, USRuser>* pUserMap);

#endif  // end of defensive include
