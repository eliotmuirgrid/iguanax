// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITWcollections
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 15 February 2023 - 10:05AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <GITU/GITUcollection.h>
#include <GITU/GITUcollectionLoad.h>
#include <GITU/GITUlist.h>
#include <GITU/GITUlistLoad.h>
#include <GITW/GITWcollections.h>
#include <GSV/GSVlink.h>
COL_LOG_MODULE;

// /git/catalogue
void GITWcollections(const COLwebRequest& Request) {
	COL_FUNCTION(GITWcollections);
	COLvar Result;
	Result.setArrayType();
	GITUlist List;
	COLstring Error;
	if(!GITUlistCatalogueLoad(&List)) { return COLrespondError(Request.Address, "Failed to load git collection."); }
	int j = 0;
	for(auto& i : List.List) {
		// TODO eliminate GITUcollection so we can reduce the code.
		GITUcollection Collection;
		COLstring GitUrl = i.GitUrl;
		COLstring Name, Description;
		if(GITUcollectionLoadFromUrl(&Collection, GitUrl, &Error)) {
			Name			= Collection.m_Name;
			Description = Collection.m_Description;
		} else {
			Name			= "New collection: " + GitUrl;
			Description = "";
		}
		COL_VAR3(GitUrl, Name, Description);
		COLvar& Entry = Result[j];
		Entry["name"] = Name;
		Entry["git"]  = GitUrl;
		GSVlink Link;
		if(GSVconstructLink(GitUrl, &Link)) { Entry["web"] = Link.m_Web; }
		Entry["description"] = Description;
		j++;
	}
	COLrespondSuccess(Request.Address, Result);
}
