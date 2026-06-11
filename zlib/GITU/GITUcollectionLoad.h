#ifndef __GITU_COLLECTION_LOAD_H__
#define __GITU_COLLECTION_LOAD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUcollectionLoad
//
// Description:
//
// Serialization routine to read git collection
//
// Author: Eliot Muir 
// Date:   Monday 13 February 2023 - 11:31AM
// ---------------------------------------------------------------------------
#include <COL/COLmap.h>

class COLvar;
class GITUcollection;
class COLstring;
class GITUlist;

bool GITUcollectionLoad(GITUcollection* pCollection, const COLvar& Data, const COLstring& GitUrl, COLstring* pError);

bool GITUcollectionLoadFromUrl(GITUcollection* pCollection, const COLstring& GitUrl, COLstring* pError);

void GITUcollectionLoadFromCatalogue(const GITUlist& List, GITUcollection* pCollection);

#endif // end of defensive include
