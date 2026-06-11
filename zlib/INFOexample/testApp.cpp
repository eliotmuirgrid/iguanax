//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testApp
//
// Description:
//
// Implementation
//
// Author: Matthew Sobkowski
// Date:   Thu 05/13/2021
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include <INFO/INFOusage.h>
#include <RGN/RGNmachineId.h>

#include <COL/COLvar.h>
#include <COL/COLutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


static void testMemory() {
   COL_FUNCTION(testMemory);
   COLvar Memory;
   INFOmemoryUsage(&Memory);
   COL_VAR(Memory);
}

static void testCpuUsage() {
   COL_FUNCTION(testCpuUsage);
   COLvar CPU;
   INFOcpuUsage(&CPU);
   COL_VAR(CPU);
}

static void testDiskUsage() {
   COL_FUNCTION(testDiskUsage);
   COLvar Disk;
   INFOdiskUsage(&Disk);
   COL_VAR(Disk);
}

static void testMachineId() {
   COL_FUNCTION(testMachineId);
   // COLstring  MachineId = RGNmachineId;
   // COL_VAR(MachineId);
}


void testApp(UNITapp* pApp){
   pApp->add("app/testMemory", &testMemory);
   pApp->add("app/testCPU", &testCpuUsage);
   pApp->add("app/testDiskUsage", &testDiskUsage);
   // pApp->add("app/testMachineId", &testMachineId);
}
