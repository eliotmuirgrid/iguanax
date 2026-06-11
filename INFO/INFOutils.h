#ifndef __INFO_UTILS_H__
#define __INFO_UTILS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2026 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INFOutils
//
// Description:
//
// Function to check if we are running out of file descriptors
//
// Author: Vismay Shah 
// Date:   Thursday 22 January 2026 - 12:44PM
// ---------------------------------------------------------------------------
#define IFW_FD_LIMIT "IFW_FD_LIMIT"

bool INFOapproachingFdLimit(int Threshold = 20);
void INFOsetFdLimitWrapper();

#endif // end of defensive include
