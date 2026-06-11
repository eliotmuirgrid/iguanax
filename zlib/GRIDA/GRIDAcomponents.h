#ifndef _GRIDACOMPONENTS_H
#define _GRIDACOMPONENTS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDAcomponents.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLwebRequest;
class GRIDnode;

struct CFGmap;

void GRIDAcomponentsImpl(CFGmap* pComponentMap, COLvar* pOut);
void GRIDAcomponents(const COLwebRequest& Request, GRIDnode* pNode, CFGmap* pComponentMap);

#endif  // end of defensive include
