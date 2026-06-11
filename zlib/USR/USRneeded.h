#ifndef __USR_NEEDED_H__
#define __USR_NEEDED_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRneeded
//
// Description:
//
// This is used in start up mode.  It says whether or not a new user account is needed.
//
// Author: Eliot Muir 
// Date:   Saturday 27 May 2023 - 09:12AM
// ---------------------------------------------------------------------------

#include <COL/COLmap.h>

class COLvar;
class COLwebRequest;
class USRuser;

void USRneeded(const COLwebRequest& Request, COLmap<COLstring, USRuser>*  pUserMap);

#endif // end of defensive include
