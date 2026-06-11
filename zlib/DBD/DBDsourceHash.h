#ifndef __DBDSOURCEHASH_H__
#define __DBDSOURCEHASH_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DBDsourceHash.h
//
//  Description: Used to automatically refresh browser if source hash doesnt match
//
//  Author: Matthew Sobkowski
//  Date:   10/11/23 10:01 AM
//  ---------------------------------------------------------------------------
class COLwebRequest;

void DBDsourceHash(const COLwebRequest& Request);

#endif // end of defensive include
