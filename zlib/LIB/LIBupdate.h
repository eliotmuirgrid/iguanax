#ifndef LIBUPDATE_H
#define LIBUPDATE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LIBupdate.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   11/06/25 10:05 AM
//  ---------------------------------------------------------------------------
#include <COL/COLweb.h>

class SCKloop;

void LIBupdate(const COLwebRequest& Request, SCKloop* pLoop);


#endif // end of defensive include
