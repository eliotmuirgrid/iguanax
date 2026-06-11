// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DTLpinsShift
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Thursday 18 July 2024 - 10:14AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <DTL/DTLpinsLoad.h>
#include <DTL/DTLpinsSave.h>
#include <DTL/DTLpinsShift.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void DTLpinsShiftImpl(COLstring User, COLvar Shift, CFGmap* pMap, COLaddress Address){
	COL_FUNCTION(DTLpinsShiftImpl);
	COLstring Error;
	DTLpinsData ShiftData;
	if(!DTLpinsParse(Shift, pMap, &ShiftData, &Error)){
		COL_TRC("Could not load shift pins data: " << Error);
		return COLrespondError(Address, Error);
	}
	COLvar Pins;
	if(!DTLpinsLoadFile(User, &Pins, &Error)){
		COL_TRC("Could not load pins data: " << Error);
		Error = "Cannot shift pins of unset components. Failed to load pins data: " + Error; 
		return COLrespondError(Address, Error);
	}
	DTLpinsData PinsData;
	if(!DTLpinsParse(Pins, pMap, &PinsData, &Error)){
		COL_TRC("Could not parse pins data: " << Error);
		return COLrespondError(Address, Error);
	}
	for(auto it = ShiftData.Map.begin(); it != ShiftData.Map.end(); it++) { 
		COLstring Component = it.key();
		if(PinsData.Map.count(Component) == 0) continue;
		DTLpinsPos& PinsPos = PinsData.Map[Component];
		DTLpinsPos& ShiftPos = it.value();
		PinsPos.PosX = PinsPos.PosX + ShiftPos.PosX;
		PinsPos.PosY = PinsPos.PosY + ShiftPos.PosY;
	}
	if(DTLpinsSaveFile(User, PinsData, &Error)){ return COLrespondSuccess(Address); }
	return COLrespondError(Address, Error);
}

// /pins/shift
void DTLpinsShift(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
	COL_FUNCTION(DTLpinsShift);
   const COLvar& Shift = COLvarRequiredObject(Request.Data, "shift");
   COLstring User = Request.User;
	pLoop->threadPool()->scheduleTask(COLnewClosure0(&DTLpinsShiftImpl, User, Shift, pMap, Request.Address));
}