#ifndef __GITU_COLLECTION_H__
#define __GITU_COLLECTION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUcollection
//
// Description:
//
// POD for git collection information
//
// Author: Eliot Muir 
// Date:   Monday 13 February 2023 - 11:23AM
// ---------------------------------------------------------------------------
#include <GITU/GITUlist.h>

class GITUcollection{
public:
   GITUcollection();
   ~GITUcollection();

   COLstring m_Name;
   COLstring m_Description;

   GITUlist m_List;
};

COLostream& operator<<(COLostream& Stream, const GITUcollection& Collection);

void GITUcollectionVar(const GITUcollection& Collection, COLvar* pOut);

COLvar GITUcollectionItems(const GITUcollection& Collection, bool FrontEnd);

void GITUcollectionVarComponents(const GITUcollection& Collection, COLvar* pOut);

void GITUcollectionVarLibraries(const GITUcollection& Collection, COLvar* pOut);


#endif // end of defensive include
