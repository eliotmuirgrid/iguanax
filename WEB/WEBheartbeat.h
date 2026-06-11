#ifndef __WEB_HEARTBEAT_H__
#define __WEB_HEARTBEAT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBheartbeat
//
// Description:
//
// Seperated login for session heartbeat
//
// Author: Eliot Muir
// Date:   Thu  1 Sep 2022 12:00:18 EDT
//---------------------------------------------------------------------------

class COLstring;

#include <WEB/WEBsessionHandler.h>  // I give up for the COLclosures.

void WEBsessionInfo(COLvar* pOut, const COLstring& Username);
void WEBheartbeat(const WEBrequestData& Data,  COLmap<COLstring, WEBsession>* pSessionMap);
void WEBheartbeatSet(COLclosure1<COLstring>* pCallback);

#endif // end of defensive include
