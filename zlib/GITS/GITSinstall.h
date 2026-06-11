#ifndef __GITS_GITSINSTALL_H__
#define __GITS_GITSINSTALL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITSinstall
//
// Description: Downloads and executes git for windows if git not installed. If on posix, only checks for git on path
//
// Author: Matthew Sobkowski
// Date:   Monday 10 April 2023
// ---------------------------------------------------------------------------
class COLstring;
class COLwebRequest;

bool GITSinstallImpl(COLstring* pError);
void GITSinstall(const COLwebRequest& Request);

#endif // end of defensive include
