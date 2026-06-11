#ifndef __CMPC_UTILS_H__
#define __CMPC_UTILS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCutils
//
// Description:
//
// TODO
//
// Author: Tyler Brown
// Date:   Wednesday 05 April 2023 - 01:03PM
// ---------------------------------------------------------------------------
#include <COL/COLclosure.h>

class BLUAinstanceManager;
class COLaddress;
class COLstring;
class COLwebRequest;
class COLvar;
class SCKloop;
struct CFGconfig;
struct CFGmap;

COLclosure0* CMPCcallback(const COLstring& Api);

void CMPCsaveConfig(const COLstring& Username, const CFGconfig& Config, bool SkipCommit, SCKloop* pLoop,
                    const COLaddress& Address);

bool CMPCisDevelopment(const COLstring& Commit);

void CMPCremoveRequireFromAllFiles(const COLstring& Dir, const COLstring& StringPattern);

bool CMPCsetErrorVar(COLvar* pOut, const COLstring& Error);
bool CMPCcheckoutOrCreateBranch(const COLstring& Username, const COLstring& Dir, const COLstring& BranchName,
                                bool* pCreated, COLstring* pError);

void CMPCstartComponentWithRun(const COLstring& ComponentId, BLUAinstanceManager* pManager, SCKloop* pLoop,
                               CFGmap* pMap);

COLstring CMPCgetRunConfigField(const COLstring& Component, const COLstring& Field);
COLstring CMPCgetRunBranch(const COLstring& Component);
COLstring CMPCgetRunRepo(const COLstring& Component);
COLstring CMPCgetRunCommit(const COLstring& Component);


#endif  // end of defensive include
