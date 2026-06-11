#ifndef __COL_SIMULATE_OS_H__
#define __COL_SIMULATE_OS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLsimulateOs
//
// Description:
//
// Utility to simulate an OS. It's easier developing on Mac OS X.
// This provides a tool to make it possible to develop windows functionality
// on Mac.
//
// Author: Eliot Muir 
// Date:   Monday 14 August 2023 - 08:51AM
// ---------------------------------------------------------------------------

class COLstring;

void COLsimulateOS();

COLstring COLsimulatedOs();

bool COLisWindows();

#endif // end of defensive include
