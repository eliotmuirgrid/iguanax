//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapi.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <GRID/GRIDnode.h>
#include <GRIDA/GRIDAadd.h>
#include <GRIDA/GRIDAapi.h>
#include <GRIDA/GRIDAcacheNames.h>
#include <GRIDA/GRIDAconnect.h>
#include <GRIDA/GRIDAdisconnect.h>
#include <GRIDA/GRIDAgetStats.h>
#include <GRIDA/GRIDAlist.h>
#include <GRIDA/GRIDAme.h>
#include <GRIDA/GRIDAremoteRequest.h>
#include <GRIDA/GRIDAremove.h>
#include <GRIDA/GRIDAresources.h>
#include <GRIDA/GRIDAsetAccess.h>
#include <GRIDA/GRIDAstatus.h>
#include <GRIDA/GRIDAcomponents.h>
COL_LOG_MODULE;

void GRIDAapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SCKloop* pLoop, GRIDnode* pNode, CFGmap* pComponentMap) {
   COL_FUNCTION(GRIDAapi);
   ApiMap.add("/iguanas/list",        COLnewClosure1(&GRIDAlist, pNode));
   ApiMap.add("/iguanas/names",       COLnewClosure1(&GRIDAcacheNames, pNode, pLoop));

   ApiMap.add("/iguanas/me",          COLnewClosure1(&GRIDAme, pNode, pComponentMap));
   ApiMap.add("/iguanas/status",      COLnewClosure1(&GRIDAstatus, pNode));

   ApiMap.add("/iguanas/add",         COLnewClosure1(&GRIDAaddPeer,pNode, pLoop));
   ApiMap.add("/iguanas/remove",      COLnewClosure1(&GRIDAremovePeer, pNode, pLoop));
   ApiMap.add("/iguanas/connect",     COLnewClosure1(&GRIDAconnect, pNode));
   ApiMap.add("/iguanas/disconnect",  COLnewClosure1(&GRIDAdisconnect, pNode));

   ApiMap.add("/iguanas/access/set",  COLnewClosure1(&GRIDAsetAccess, pNode, pLoop));

   ApiMap.add("/iguanas/remote_call", COLnewClosure1(&GRIDAremoteRequest, pNode, pLoop));

   ApiMap.add("/iguanas/resources",   COLnewClosure1(&GRIDAresources, pNode, pLoop));
   ApiMap.add("/resources/current",   COLnewClosure1(&GRIDAgetCurrentStats, pNode, pLoop));
   ApiMap.add("/resources/history",   COLnewClosure1(&GRIDAgetStatHistory, pNode, pLoop));

   ApiMap.add("/iguanas/components",  COLnewClosure1(&GRIDAcomponents, pNode, pComponentMap));
}
