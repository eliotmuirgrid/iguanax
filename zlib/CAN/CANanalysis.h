#ifndef __CAN_ANALYSIS_H__
#define __CAN_ANALYSIS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CANanalysis
//
// Description:
//
// Routines to iterate through all the source code in IguanaX to understand
// it's source.
//
// Author: Eliot Muir 
// Date:   Wednesday 15 January 2025 - 12:10PM
// ---------------------------------------------------------------------------

class COLstring;

void CANanalysis(const COLstring& RootDir);
void CANloadList(const COLstring& RootDir, const COLstring& ListFile);

#endif // end of defensive include
