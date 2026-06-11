#ifndef __USR_ADMIN_UNSAFE_H__
#define __USR_ADMIN_UNSAFE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRadminUnsafe
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Tuesday 29 August 2023 - 03:14PM
// ---------------------------------------------------------------------------

#include <COL/COLmap.h>

class COLwebRequest;
class COLstring;
class USRuser;

void USRadminUnsafe(const COLwebRequest& Request, COLmap<COLstring, USRuser>*  pUserMap);

#endif // end of defensive include
