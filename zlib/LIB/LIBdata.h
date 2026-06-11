#ifndef LIBDATA_H
#define LIBDATA_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LIBdata.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   10/06/25 12:32 PM
//  ---------------------------------------------------------------------------
#include <COL/COLarray.h>

enum LIBtype {
   LIB_COMPONENT_DATA,
   LIB_COLLECTION_DATA
};

struct LIBurls {
   COLstring m_Ssh;
   COLstring m_Url;
   COLstring m_Web;
};

struct LIBusage {
   LIBusage();
   LIBusage(const COLstring& Component, const COLstring& Path, const COLstring& Error, const bool Ahead,
            const bool Behind, const bool HasChanges)
       : m_Component(Component),
         m_Path(Path),
         m_Error(Error),
         m_Ahead(Ahead),
         m_Behind(Behind),
         m_hasChanges(HasChanges) {}
   COLstring m_Component;
   COLstring m_Path;
   COLstring m_Error;
   bool      m_Ahead;
   bool      m_Behind;
   bool      m_hasChanges;
};

struct LIBdata {
   COLstring           m_Name;
   COLstring           m_Description;
   LIBurls             m_Urls;
   LIBurls             m_CollectionUrls;
   COLarray<LIBusage> m_Usage;
};

void LIBdataToVar(const LIBdata& Data, COLvar* pOut);
void LIBsetLibDataInfo(LIBdata* pData, const COLvar& DataVar);
void LIBsetLibDataUsage(LIBdata* pData, const COLvar& DataVar);

#endif // end of defensive include
