#ifndef __SDBA_PURGE_RULES_H__
#define __SDBA_PURGE_RULES_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBApurgeRules
//
// Description:
//
// Get and set log purge rules.
//
// Author: Vismay Shah
// Date:   Thursday 21 September 2023 - 08:51AM
// ---------------------------------------------------------------------------
class COLwebRequest;
class SDBsystem;
class SCKloop;

void SDBApurgeRulesGet(const COLwebRequest& Request, SDBsystem* pSystem);
void SDBApurgeRulesSet(const COLwebRequest& Request, SDBsystem* pSystem, SCKloop* pLoop);
void SDBApurgeRulesAltDir(const COLwebRequest& Request);

#endif // end of defensive include
