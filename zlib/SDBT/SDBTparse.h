#ifndef __SDBT_PARSE_H__
#define __SDBT_PARSE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTparse
//
// Description:
//
// Test functionality for parsing.
//
// Author: Eliot Muir 
// Date:   Monday 27 February 2023 - 10:35AM
// ---------------------------------------------------------------------------

class SDBcontent;
class COLstring;

#include <COL/COLlist.h>

void SDBTparse(COLstring* pBuffer, int* pCount); //COLlist<SDBcontent*>* pMessageList);

#endif // end of defensive include
