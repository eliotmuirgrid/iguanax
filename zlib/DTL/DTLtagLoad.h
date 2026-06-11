#ifndef __DTLTAGLOAD_H__
#define __DTLTAGLOAD_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DTLtagLoad.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/03/24 12:27 PM
//  ---------------------------------------------------------------------------
#include <COL/COLweb.h>

bool DTLtagLoadImpl(const COLstring& User, COLvar* pInterfaces, COLstring* pError);
void DTLtagLoad(const COLwebRequest& Request);

#endif // end of defensive include
