#ifndef __COL_IO_CHECK_H__
#define __COL_IO_CHECK_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLioCheck
//
// Description:
//
// Utility to assert that we don't have blocking IO on the main event loop
//
// Author: Eliot Muir
// Date:   Friday 13 January 2023 - 10:03AM
// ---------------------------------------------------------------------------

void COLsetMainThread();
bool COLisMainThread();
void COLclearMainThread();
void COLassertIfMainThread();

#endif // end of defensive include
