#ifndef _GRIDAPIGRANTACCESS_H
#define _GRIDAPIGRANTACCESS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiGrantAccess.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLwebRequest;
class GRIDnode;
class SCKloop;

void GRIDAsetAccess(const COLwebRequest& Request, GRIDnode* pNode, SCKloop* pLoop);

#endif
