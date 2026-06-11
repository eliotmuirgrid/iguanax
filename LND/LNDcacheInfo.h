#ifndef __LND_CACHE_INFO_H__
#define __LND_CACHE_INFO_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNDcacheInfo
//
// Description:
//
// POD class for storing data members related to an EDIcollection or NTABschema
//
// Author: Vismay Shah 
// Date:   Wednesday 14 June 2023 - 02:04PM
// ---------------------------------------------------------------------------
#include <COL/COLstring.h>
#include <time.h>

class LNDcacheInfo{
public:
   time_t     m_LastAccess;
   COLstring  m_FilePath;
   COLstring  m_ConversionWarning;
   bool       m_IsLegacy;
};

void LNDpopulateCacheInfo(LNDcacheInfo* pInfo, const COLstring& File, bool LegacyWarning, COLstring ConversionWarning, const time_t& LastWrite);

#endif // end of defensive include
