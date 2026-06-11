#ifndef _GRIDAREMOTEREQUEST_H
#define _GRIDAREMOTEREQUEST_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiMakeRemoteRequest.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLwebRequest;
class GRIDnode;
class SCKloop;

void GRIDAremoteRequest(const COLwebRequest& Request, GRIDnode* pNode, SCKloop* pLoop);

#endif
