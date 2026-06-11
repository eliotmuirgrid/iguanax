#ifndef __SDBT_DIR_H__
#define __SDBT_DIR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTdir
//
// Description:
//
// Functions to test the creation of logs files
//
// Author: Eliot Muir
// Date:   Thursday 02 March 2023 - 01:27PM
// ---------------------------------------------------------------------------

#include <time.h>

class COLstring;

// Creates files from 00->FF
void SDBTcreateFullRangeDir(const COLstring& Dir);

#endif // end of defensive include
