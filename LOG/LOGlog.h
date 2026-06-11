#ifndef __LOG_LOG_H__
#define __LOG_LOG_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGlog
//
// Description:
//
// Abstracted Logging facility.
//
// Author: Eliot Muir
// Date:   Thursday 18 May 2023 - 02:40PM
// ---------------------------------------------------------------------------
#include <COL/COLclosure.h>
#include <COL/COLstring.h>

class LOGlogInfo {
  public:
   LOGlogInfo(const int Type, const COLstring& Component, const COLstring& HashTags, const COLstring& Source,
              const COLuint64& DateTime, const COLuint64& Index)
       : m_Type(Type),
         m_ComponentId(Component),
         m_HashTags(HashTags),
         m_Source(Source),
         m_DateTime(DateTime),
         m_Index(Index) {}

   int       m_Type;
   COLstring m_ComponentId;
   COLstring m_HashTags;
   COLstring m_Source;
   COLuint64 m_DateTime;
   COLuint64 m_Index;
};

void LOGlog(const LOGlogInfo& LogInfo, const COLstring& Data);

void LOGlogSetCallback(COLclosure2<const LOGlogInfo&, const COLstring&>* pCallback);

#endif // end of defensive include
