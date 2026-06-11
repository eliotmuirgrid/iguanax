//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CFGconfigLoad
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Mon 11 Jul 2022 14:34:23 EDT
//---------------------------------------------------------------------------
#include "CFGconfigLoad.h"

#include <CFG/CFGconfig.h>
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PASS/PASSword.h>
COL_LOG_MODULE;

static bool CFGloadFile(const COLstring& FilePath, COLstring* pConfigData) {
	if(!FILfileExists(FilePath)) {
		COL_TRC("Component configuration file does not exist");
		return false;
	}
	if (CFGcompJsonLimit < FILfileSize(FilePath)) { 
		COL_TRC("Component configuration file way too big - greater than 100mb.");
		return false;
	}
	try {
		COLstring RawData;
		FILreadFile(FilePath, &RawData);
		if(!PASSdecrypt(RawData, pConfigData)) { *pConfigData = RawData; }
		COL_VAR(*pConfigData);
	} catch(const COLerror& E) {
		COL_TRC(E.description());
		return false;
	}
	return true;
}

static bool CFGconfigLoadData(const COLstring& ComponentId, COLstring* pData) {
	COL_FUNCTION(CFGconfigLoadData);
	CFGconfig Config;
	Config.CoreConfig.Guid = ComponentId;
	COLstring FileName	  = DIRcomponentConfig(Config.CoreConfig.Guid);
	COL_VAR(FileName);
	return CFGloadFile(FileName, pData);
}

void CFGconfigSetFields(const COLvar& FieldVar, CFGconfig* pConfig) {
	COL_FUNCTION(CFGconfigSetFields);
	if(!FieldVar.isMap()) { return; }
	pConfig->CoreConfig.Fields.clear();
	for(const auto& it : FieldVar.map()) {
		COLstring Value;
		try {
			if(!PASSdecrypt(it.second, &Value)) { Value = it.second; }
			COL_VAR(Value);
		} catch(const COLerror& E) { COL_TRC(E.description()); }
		pConfig->CoreConfig.Fields.add(it.first, Value);
	}
}

void CFGconfigSetSources(const COLvar& SourcesVar, CFGconfig* pConfig) {
	COL_FUNCTION(CFGconfigSetSources);
	if(!SourcesVar.isArray()) { return; }
	pConfig->CoreConfig.Sources.clear();
	for(const auto& it : SourcesVar.array()) { pConfig->CoreConfig.Sources.push_back(it); }
}

void CFGcongigSetTags(const COLvar& TagsVar, CFGconfig* pConfig) {
	COL_FUNCTION(CFGcongigSetTags);
	if(!TagsVar.isArray()) { return; }
	pConfig->CoreConfig.Tags.clear();
	for(const auto& it : TagsVar.array()) { pConfig->CoreConfig.Tags.push_back(it); }
}

static bool CFGparseAndMap(const COLstring& Content, CFGconfig* pConfig) {
	COL_FUNCTION(CFGparseAndMap);
	COLvar CoreData;
	if(!CoreData.parse(Content)) {
		COL_TRC("Invalid JSON configuration file.");
		return false;
	}
	pConfig->CoreConfig.Guid		  = CoreData["guid"];
	pConfig->CoreConfig.Name		  = CoreData["name"];
	pConfig->CoreConfig.Description = CoreData["description"];
	pConfig->CoreConfig.Template	  = CoreData["template"];
	pConfig->CoreConfig.Commit		  = CoreData.exists("commit") ? CoreData["commit"] : CoreData["run_commit"];
	pConfig->CoreConfig.Repo		  = CoreData.exists("repo") ? CoreData["repo"] : CoreData["run_repo"];
	pConfig->CoreConfig.RunComp	  = CoreData["run_comp"];
	if(CoreData.exists("sources")) { CFGconfigSetSources(CoreData["sources"], pConfig); }
	if(CoreData.exists("tags")) { CFGcongigSetTags(CoreData["tags"], pConfig); }
	if(CoreData.exists("fields")) { CFGconfigSetFields(CoreData["fields"], pConfig); }
	return true;
}

bool CFGconfigLoad(const COLstring& ComponentId, CFGconfig* pConfig) {
	COL_FUNCTION(CFGconfigLoad);
	COLstring Content;
	if(!CFGconfigLoadData(ComponentId, &Content)) { return false; }
	if(!CFGparseAndMap(Content, pConfig)) { return false; }
	COL_VAR(*pConfig);
	return true;
}
