// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNDcacheInfo
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 14 June 2023 - 02:04PM
// ---------------------------------------------------------------------------
#include <LND/LNDcacheInfo.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void LNDpopulateCacheInfo(LNDcacheInfo* pInfo, const COLstring& File, bool LegacyWarning, COLstring ConversionWarning, const time_t& LastWrite){
   COL_FUNCTION(LNDpopulateCacheInfo);
   pInfo->m_FilePath    = File;
   pInfo->m_IsLegacy    = LegacyWarning;
   pInfo->m_LastAccess  = LastWrite;
   pInfo->m_ConversionWarning = ConversionWarning;
}