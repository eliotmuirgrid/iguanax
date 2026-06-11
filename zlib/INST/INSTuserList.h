#ifndef __INST_USER_LIST_H__
#define __INST_USER_LIST_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTuserList
//
// Description:
//
// Get list of users on the Operating System. 
//
// Author: Eliot Muir 
// Date:   Sunday 16 July 2023 - 04:34PM
// ---------------------------------------------------------------------------

#include <COL/COLarray.h>

class COLstring;

void INSTuserList(COLarray<COLstring>* pList);

#endif // end of defensive include
