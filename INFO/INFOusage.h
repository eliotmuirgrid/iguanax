#ifndef __INFO_USAGE_H__
#define __INFO_USAGE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INFOusage
//
// Description:
//
// API for monitoring system usage for IguanaX
//
// Author: Matthew Sobkowski
// Date:   Wed 12 May 2021 9:23:45 EDT
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLstring.h>

class COLvar;

class INFOmemory {
public:
   COLuint64 Virtual  = 0;
   COLuint64 Resident = 0;
   COLuint64 Physical = 0;
};

class INFOdisk {
public:
   COLuint64 Size = 0;
   COLuint64 Free = 0;
   COLuint64 Used = 0;
};

INFOmemory INFOmemoryStats();
double INFOcpuPercent();
INFOdisk INFOdiskStats(const COLstring& DirPath = ".");
int INFOfileHandleCount();
int INFOfileHandleLimit();
void INFOsetFdLimit(int Limit);

void INFOmemoryUsage(COLvar* pInput);
void INFOcpuUsage(COLvar* pInput);
void INFOdiskUsage(COLvar* pInput);

#endif