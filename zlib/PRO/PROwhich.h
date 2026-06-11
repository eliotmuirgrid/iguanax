#ifndef __PRO_WHICH_H__
#define __PRO_WHICH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PROwhich
//
// Description:
//
// C API to find if Program is installed and where
//
// Author: Eliot Muir 
// Date:   Wednesday 01 February 2023 - 10:45AM
// ---------------------------------------------------------------------------

class COLstring;

// Returns true if Program installed and puts the path to the Program executable into the pPath variable
bool PROwhich(COLstring* pPath, const COLstring& Program);

#endif // end of defensive include
