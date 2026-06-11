#ifndef __GITU_LIST_LOAD_H__
#define __GITU_LIST_LOAD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUlistLoad
//
// Description:
//
// Loading routines for reading various configuration files from disc and putting
// then into a GITUlist.
//
// Author: Eliot Muir 
// Date:   Friday 31 March 2023 - 10:21AM
// ---------------------------------------------------------------------------

class GITUlist;
class COLstring;
class COLvar;

bool GITUlistCollectionMap(const COLstring& SourceGitUrl, const COLvar& Data, GITUlist* pList, COLstring* pError);

bool GITUlistCollectionLoad(const COLstring& SourceGitUrl, const COLstring& FileName, GITUlist* pList, COLstring* pError);

void GITUlistGitModuleLoad(const COLstring& SourceGitUrl, const COLstring& FileName, GITUlist* pList);

void GITUlistCatalogueMap(const COLvar& Data, const COLstring Username, bool CheckUrls, GITUlist* pList);
void GITUlistCatalogueMap(const COLvar& Data, GITUlist* pList);

void GITUdefaultCatalogueOnStartUp();

bool GITUlistCatalogueLoad(GITUlist* pList);

void GITUlistCatalogueSave(const GITUlist& List);

// will detect a git collection file and load it.
bool GITUlistLoad(const COLstring& Dir, const COLstring& SourceGitUrl, GITUlist* pList);

#endif // end of defensive include
