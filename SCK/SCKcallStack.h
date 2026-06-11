#ifndef __SCK_CALL_STACK_H__
#define __SCK_CALL_STACK_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKcallStack
//
// Description:
//
// Wrappers around callstack increment and decrement to check for inbalance.
//
// Author: Eliot Muir 
// Date:   Wednesday 23 August 2023 - 01:02PM
// ---------------------------------------------------------------------------

class SCKconnection;

#define SCK_INC_CALLSTACK(pConnection) SCKcallStackInc(pConnection, __FILE__,__LINE__);
#define SCK_DEC_CALLSTACK(pConnection) SCKcallStackDec(pConnection, __FILE__, __LINE__);

void SCKcallStackInc(SCKconnection* pConnection, const char* pFile, int Line);
void SCKcallStackDec(SCKconnection* pConnection, const char* pFile, int Line);

#endif // end of defensive include
