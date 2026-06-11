#ifndef __SDB_CONTENT_DELETE_H__
#define __SDB_CONTENT_DELETE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBcontentDelete
//
// Description:
//
// Cleanup routines for COLlist<SDBcontent*>
//
// Author: Eliot Muir 
// Date:   Monday 15 May 2023 - 12:26PM
// ---------------------------------------------------------------------------

#include <COL/COLlist.h>

class SDBcontent;

void SDBcontentDelete(COLlist<SDBcontent*>* pList);

#endif // end of defensive include
