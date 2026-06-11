//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITCserversLoad.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   04/07/24 1:34 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <GITC/GITCserversLoad.h>
#include <GSV/GSVmap.h>
#include <GSV/GSVlink.h>
COL_LOG_MODULE;

void GITCserversLoad(const COLwebRequest& Request) {
	COL_FUNCTION(GITCserversLoad);
	COLvar Data = GSVmaptoVar();
	if(Data.isMap()) {
		// also add a server link to each entry
		for (auto it : Data.map()){
		COLstring Temp;
		if (GSVconstructServerLink(it->first, &Temp)) it->second["web"] = Temp;
		}
	}
	COLrespondSuccess(Request.Address, Data);
}