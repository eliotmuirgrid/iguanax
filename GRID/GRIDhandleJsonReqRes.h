#ifndef __GRIDHANDJSONREQRES_H__
#define __GRIDHANDJSONREQRES_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDnode.h
//
//  Description:
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLstring;
class GRIDnode;
class SCKloop;

void GRIDonJson(const COLstring* pJsonPayload, GRIDnode* pNode, COLstring IguanaId, SCKloop* pLoop);

#endif
