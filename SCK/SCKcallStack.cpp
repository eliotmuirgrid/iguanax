// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKcallStack
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 23 August 2023 - 01:02PM
// ---------------------------------------------------------------------------

#include <SCK/SCKcallStack.h>
#include <SCK/SCKconnection.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void SCKcallStackInc(SCKconnection* pConnection, const char* pFile, int Line){
   pConnection->m_CallbackStack++;
   COL_TRC("INC Callstack = " << pConnection->m_CallbackStack << " " << pConnection << " from " << pFile << ":" << Line);
}

void SCKcallStackDec(SCKconnection* pConnection, const char* pFile, int Line){
   pConnection->m_CallbackStack--;
   COL_TRC("DEC Callstack = " << pConnection->m_CallbackStack << " " << pConnection << " from " << pFile << ":" << Line);
}
