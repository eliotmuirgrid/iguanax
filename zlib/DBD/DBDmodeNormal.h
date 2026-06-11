#ifndef __DBDMODENORMAL_H__
#define __DBDMODENORMAL_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DBDmodeNormal.h
//
//  Description: Api to set Iguana to normal mode
//
//  Author: Matthew Sobkowski
//  Date:   5/2/23 10:04 AM
//  ---------------------------------------------------------------------------
class DBDworld;
class ACTactionSet;

void DBDmodeNormalShutdownList(DBDworld* pWorld, ACTactionSet* pShutDownSeries);
void DBDmodeNormal(DBDworld* pWorld);

#endif // end of defensive include
