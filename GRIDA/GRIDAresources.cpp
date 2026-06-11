//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiPeerStats.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <GRID/GRIDrequest.h>
#include <GRIDA/GRIDAresources.h>
COL_LOG_MODULE;

static void GRIDAresourcesCallback(const COLvar& Result, const COLaddress Address) {
   COL_FUNCTION(GRIDAresourcesCallback);
   COLrespondSuccess(Address, Result);
}

// /iguanas/resources
void GRIDAresources(const COLwebRequest& Request, GRIDnode* pNode, SCKloop* pLoop) {
   COL_FUNCTION(GRIDAresources);
   COLauto<COLclosure1<const COLvar&>> pCallback = COLnewClosure1(&GRIDAresourcesCallback, Request.Address);
   GRIDrequestAll("/resources/current", COLvar(), pNode, pLoop, pCallback);
}