#ifndef __USR_LOAD_H__
#define __USR_LOAD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRload
//
// Description:
//
// Load users
//
// Author: Eliot Muir 
// Date:   Sunday 18 September 2022 - 06:00PM
// ---------------------------------------------------------------------------

#include <USR/USRmap.h> // because it's not a class I cannot predeclare it.

void USRload(COLmap<COLstring, USRuser>*  pUserMap);

#endif // end of defensive include
