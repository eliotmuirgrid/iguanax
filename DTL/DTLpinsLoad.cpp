//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DTLpinsLoad.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   16/07/24 3:30 PM
//  ---------------------------------------------------------------------------
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <DIR/DIRpath.h>
#include <DTL/DTLpinsLoad.h>
#include <FIL/FILutils.h>

#include <SCK/SCKloop.h>
#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool DTLpinsLoadFile(const COLstring& Username, COLvar* pPins, COLstring* pError){
	COL_FUNCTION(DTLpinsLoadFile);
	COLvar& Pins = *pPins;
	COLstring& Error  = *pError;
	const COLstring PositionsFile = DIRconfigUserDir(Username) + "pins.json";
	COL_VAR(PositionsFile);
	if (!FILfileExists(PositionsFile)) {
		Error = PositionsFile + " does not exist";
		return false;
	}
	try {
		COLstring RawData;
		FILreadFile(PositionsFile, &RawData);
		if (RawData.is_null()) { return true; }
		if (!Pins.parse(RawData) || !Pins.isMap()) {
			Error = "Invalid pins data";
			return false;
		}
	} catch (const COLerror& ErrorCatch) { 
		Error = ErrorCatch.description();
		return false;
	}
	return true;
}

bool DTLpinsPosParse(const COLvar& PosData, DTLpinsPos* pData){
	COL_FUNCTION(DTLpinsPosParse);
	DTLpinsPos& Data = *pData;
	if(!PosData.isMap()) {
		return false;
	}
	if(!PosData.exists("px") || !PosData.exists("py")){
		return false;
	}
	Data.PosX = PosData["px"].asInt();
	Data.PosY = PosData["py"].asInt();
	return true;
}

bool DTLpinsParse(const COLvar& Pins, CFGmap* pMap, DTLpinsData* pData, COLstring* pError){
	COL_FUNCTION(DTLpinsParse);
	DTLpinsData& Data = *pData;
	COLstring& Error = *pError;
	if(!Pins.isMap()) {
		Error = "Invalid pins data";
		return false;
	}
	for(auto it = Pins.map().begin(); it != Pins.map().end(); it++) { 
		COLstring ComponentId = it.key();
      COLvar  PosVar        = it.value();
		DTLpinsPos PosData;
		if(!DTLpinsPosParse(PosVar, &PosData)){
			Error = "Invalid pins data";
			return false;
		}
		if (pMap->ConfigMap.count(ComponentId)) { Data.Map[ComponentId] = PosData; } // only store pin data for valid component guids
   }
	return true;
}

COLvar DTLpinsPosToVar(const DTLpinsPos& Data){
	COL_FUNCTION(DTLpinsPosToVar);
	COLvar PosData;
	PosData["px"] = Data.PosX;
	PosData["py"] = Data.PosY;
	return PosData;
}

void DTLpinsToVar(const DTLpinsData& Data, COLvar* pPins){
	COL_FUNCTION(DTLpinsToVar);
	COLvar& Pins = *pPins;
	Pins.setMapType();
	for(auto it = Data.Map.begin(); it !=  Data.Map.end(); it++) { 
		COLvar PosData = DTLpinsPosToVar(it.value());
		Pins[it.key()] = PosData;
	}
}

bool DTLpinsVerify(const COLvar& Pins, CFGmap* pMap, COLvar* pResult, COLstring* pError){
	COL_FUNCTION(DTLpinsVerify);
	DTLpinsData Data;
	if(!DTLpinsParse(Pins, pMap, &Data, pError)){
		COL_TRC("Could not parse pins data: " << *pError);
		return false; 
	}
	COLvar& Result = *pResult;
	DTLpinsToVar(Data, &Result);
	return true;
}

bool DTLpinsLoadImpl(const COLstring& User, CFGmap* pMap, COLvar* pPins, COLstring* pError) {
	COL_FUNCTION(DTLpinsLoadImpl);
	COLvar& Pins = *pPins;
	Pins.setMapType();
	COLvar Result;
	if(!DTLpinsLoadFile(User, &Pins, pError)){
		COL_TRC("Could not load pins data: " << *pError);
		return true; // not an error - if file doesn't exist pins aren't being used.
	}
	return DTLpinsVerify(Pins, pMap, &Result, pError);
}

static void DTLpinsLoadWorker(COLstring User, CFGmap* pMap, COLaddress Address) {
	COL_FUNCTION(DTLpinsLoadWorker);
	COLvar Pins;
	COLstring Error;
	if (!DTLpinsLoadImpl(User, pMap, &Pins, &Error)){ COLrespondError(Address, Error);  }
	else 													  		{ COLrespondSuccess(Address, Pins); }
}

// /pins/load
void DTLpinsLoad(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
	COL_FUNCTION(DTLpinsLoad);
	COLstring User = Request.User;
	pLoop->threadPool()->scheduleTask(COLnewClosure0(&DTLpinsLoadWorker, User, pMap, Request.Address));
}