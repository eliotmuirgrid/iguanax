#ifndef __GITU_FULL_DETECT_H__
#define __GITU_FULL_DETECT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUfullDetect
//
// Description:
//
// This routine does a full comprehensive check of whether GIT is installed and fully configured.
//
// Author: Eliot Muir 
// Date:   Tuesday 14 March 2023 - 04:48PM
// ---------------------------------------------------------------------------

class COLstring;

bool GITUfullDetect(const COLstring& Username, COLstring* pError);

#endif // end of defensive include
