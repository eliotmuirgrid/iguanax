#ifndef MAIN2PROJECT_GRIDAPIRECONNECT_H
#define MAIN2PROJECT_GRIDAPIRECONNECT_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiReconnect.h
//
//  Description: API for reconnecting to GRID Iguanas.
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLwebRequest;
class GRIDnode;
class SCKloop;

void GRIDAconnect(const COLwebRequest& Request, GRIDnode* pNode);

#endif  // end of defensive include
