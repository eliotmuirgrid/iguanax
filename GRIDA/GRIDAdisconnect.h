#ifndef _GRIDAPIDISCONNECT_H
#define _GRIDAPIDISCONNECT_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiDisconnect.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLwebRequest;
class GRIDnode;

void GRIDAdisconnect(const COLwebRequest& Request, GRIDnode* pNode);

#endif  // end of defensive include
