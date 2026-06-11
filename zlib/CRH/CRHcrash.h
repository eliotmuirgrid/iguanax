#ifndef __CRH_CRASH_H__
#define __CRH_CRASH_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: CRHcrash
//
// Author: Eliot Muir
//
// Description:
//
// Routines to handle crash dumps.
//---------------------------------------------------------------------------

class COLstring;
class COLostreamString;

bool CRHenableCrashDump();
bool CRHfindLastDump(COLstring* pFileName);

void CRHsetUpCrashLog();

// helper functions for CRHenableCrashDump() and CRHsetUpCrashLog()

COLstring CRHpidFileName ();
bool CRHcoresDirExistsWithRightPermissions();
COLstring CRHgetOS();

COLstring CRHsignalException(int Signal);
void CRHsetAbortMessage(const COLstring& Message);
void CRHcrashContext(COLostreamString& Stream);
void CRHcrashHeader(COLostreamString& DumpStream, const COLstring& Exception, int ThreadId);
void CRHfinalizeCrashDumpGeneration(COLostreamString& DumpStream);

// return the value of env var IFW_CRASH_DUMP_MODE
// 0 : default - call stack for all threads
// 1 : call stack for current thread only
int CRHcrashDumpMode();

#endif // end of defensive include
