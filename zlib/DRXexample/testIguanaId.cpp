//---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testIguanaId
//
// Description:
//
// Implementation.
//
// Author: Vismay Shah
// Date:   Thursday, January 18th, 2024 @ 12:29:40 PM
//---------------------------------------------------------------------------
#include "testIguanaId.h"
#include <UNIT/UNITapp.h>

#include <DRX/DRXiguanaId.h>

#include <FIL/FILutils.h>
#include <RGN/RGNmachineId.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void testIguanaIdNoArguments(){
   COL_FUNCTION(testIguanaIdNoArguments);
   // Test that the no-argument version of Iguana ID returns
   // the expected results.
   COLstring CurrentWorkingDirectory = FILworkingDir();
   UNIT_ASSERT_EQUALS(DRXiguanaIdImplementation(RGNmachineId(),CurrentWorkingDirectory), DRXiguanaId());
}

void testBasicIguanaId(){
   COL_FUNCTION(testBasicIguanaId);
   // Test that the same id is generated every time
#ifdef _WIN32
   COLstring ExpectedId("6STC6ES2GZ9Q82BQ");
#else
   COLstring ExpectedId("JV84U7UJP373ZFBB");
#endif
   COLstring MachineId("IPCIBLBDMA");
   COLstring InstallLocation("C:\\Program Files\\Somewhere Else\\Iguana\\");
   UNIT_ASSERT_EQUALS(ExpectedId, DRXiguanaIdImplementation(MachineId, InstallLocation));
   UNIT_ASSERT_EQUALS(ExpectedId, DRXiguanaIdImplementation(MachineId, InstallLocation));
   UNIT_ASSERT_EQUALS(ExpectedId, DRXiguanaIdImplementation(MachineId, InstallLocation));
   UNIT_ASSERT_EQUALS(ExpectedId, DRXiguanaIdImplementation(MachineId, InstallLocation));
   UNIT_ASSERT_EQUALS(ExpectedId, DRXiguanaIdImplementation(MachineId, InstallLocation));

   // Try different cases.  On Windows, they should produce the same Iguana ID.  On
   // all other platforms, they should be different.
   COLstring InstallLocationLowerCase("c:\\program files\\somewhere else\\iguana\\");
   COLstring InstallLocationUpperCase("C:\\PROGRAM FILES\\SOMEWHERE ELSE\\IGUANA\\");
#ifdef _WIN32
   UNIT_ASSERT_EQUALS(ExpectedId, DRXiguanaIdImplementation(MachineId, InstallLocationLowerCase));
   UNIT_ASSERT_EQUALS(ExpectedId, DRXiguanaIdImplementation(MachineId, InstallLocationUpperCase));
#else
   UNIT_ASSERT(ExpectedId != DRXiguanaIdImplementation(MachineId, InstallLocationLowerCase));
   UNIT_ASSERT(ExpectedId != DRXiguanaIdImplementation(MachineId, InstallLocationUpperCase));
#endif
}

void testReturnsMachineIdIfDefaultLocation(){
   COL_FUNCTION(testReturnsMachineIdIfDefaultLocation);
   COLstring MachineId("IPCIBLBDMA");
#ifdef _WIN32
   UNIT_ASSERT_EQUALS(MachineId, DRXiguanaIdImplementation(MachineId, "C:\\Program Files\\iNTERFACEWARE\\Iguana\\"));
#else
   UNIT_ASSERT_EQUALS("AN4Y7B5PNW5HE67S", DRXiguanaIdImplementation(MachineId, "C:\\Program Files\\iNTERFACEWARE\\Iguana\\"));
#endif

   // Not quite the default directory
   UNIT_ASSERT(MachineId != DRXiguanaIdImplementation(MachineId, "C:\\Program Files\\iNTERFACEWARE\\Iguana 2\\"));
}

void testIguanaIdUniqueness(){
   COL_FUNCTION(testIguanaIdUniqueness);
   {
      // Slightly different machine ids
      COLstring IguanaIdA = DRXiguanaIdImplementation("IPCIBLBDMA", "C:\\Program Files\\Iguana\\Iguana\\");
      COLstring IguanaIdB = DRXiguanaIdImplementation("IPCIBLBDMB", "C:\\Program Files\\Iguana\\Iguana\\");
      COLstring IguanaIdC = DRXiguanaIdImplementation("IPCIBLBDAM", "C:\\Program Files\\Iguana\\Iguana\\");
      UNIT_ASSERT(IguanaIdA != IguanaIdB);
      UNIT_ASSERT(IguanaIdB != IguanaIdC);
      UNIT_ASSERT(IguanaIdC != IguanaIdA);
   }

   {
      // Slightly different working directories
      COLstring IguanaIdA = DRXiguanaIdImplementation("IPCIBLBDMA", "C:\\Program Files\\Iguana\\Iguana\\");
      COLstring IguanaIdB = DRXiguanaIdImplementation("IPCIBLBDMA", "D:\\Program Files\\Iguana\\Iguana\\");
      COLstring IguanaIdC = DRXiguanaIdImplementation("IPCIBLBDMA", "C:\\Program Files\\Iguaan\\Iguana\\");
      UNIT_ASSERT(IguanaIdA != IguanaIdB);
      UNIT_ASSERT(IguanaIdB != IguanaIdC);
      UNIT_ASSERT(IguanaIdC != IguanaIdA);
   }
}

void testIguanaIdLongDirectoryPath(){
   COL_FUNCTION(testIguanaIdLongDirectoryPath);
#ifdef _WIN32
   UNIT_ASSERT_EQUALS("D6UZG7EN32X29WXW", DRXiguanaIdImplementation("IPCIBLBDMA", "C:\\Program Files\\Such\\A\\Long\\Path\\Why\\Would\\Users\\Want\\To\\Install\\Iguana\\This\\Deep\\Into\\Their\\C\\Drive\\Anyway\\Question\\Mark\\Program Files\\Such\\A\\Long\\Path\\Why\\Would\\Users\\Want\\To\\Install\\Iguana\\This\\Deep\\Into\\Their\\C\\Drive\\Anyway\\Question\\Mark\\Program Files\\Such\\A\\Long\\Path\\Why\\Would\\Users\\Want\\To\\Install\\Iguana\\This\\Deep\\Into\\Their\\C\\Drive\\Anyway\\Question\\Mark\\"));
#else
   UNIT_ASSERT_EQUALS("E9D8BW67F6YL7V8L", DRXiguanaIdImplementation("IPCIBLBDMA", "C:\\Program Files\\Such\\A\\Long\\Path\\Why\\Would\\Users\\Want\\To\\Install\\Iguana\\This\\Deep\\Into\\Their\\C\\Drive\\Anyway\\Question\\Mark\\Program Files\\Such\\A\\Long\\Path\\Why\\Would\\Users\\Want\\To\\Install\\Iguana\\This\\Deep\\Into\\Their\\C\\Drive\\Anyway\\Question\\Mark\\Program Files\\Such\\A\\Long\\Path\\Why\\Would\\Users\\Want\\To\\Install\\Iguana\\This\\Deep\\Into\\Their\\C\\Drive\\Anyway\\Question\\Mark\\"));
#endif
}

void testConvertFromHexToIguanaId(){
   COL_FUNCTION(testConvertFromHexToIguanaId);
   UNIT_ASSERT_EQUALS("",  DRXconvertFromHexToIguanaId(""));
   UNIT_ASSERT_EQUALS("2", DRXconvertFromHexToIguanaId("00"));
   UNIT_ASSERT_EQUALS("2", DRXconvertFromHexToIguanaId("20"));
   UNIT_ASSERT_EQUALS("3", DRXconvertFromHexToIguanaId("21"));
   UNIT_ASSERT_EQUALS("2", DRXconvertFromHexToIguanaId("C0"));
   UNIT_ASSERT_EQUALS("2", DRXconvertFromHexToIguanaId("20C"));
   UNIT_ASSERT_EQUALS("P", DRXconvertFromHexToIguanaId("15"));
   UNIT_ASSERT_EQUALS("Z", DRXconvertFromHexToIguanaId("FF"));

   UNIT_ASSERT_EQUALS("C863V6JAXRW559UD", DRXconvertFromHexToIguanaId("0A06E4C1DBE450081DB77C6343279AAB"));
}

void testIguanaId(UNITapp* pApp){
   COL_FUNCTION(testIguanaId);
   pApp->add("IguanaId/noargs",                   &testIguanaIdNoArguments);
   pApp->add("IguanId/basic",                     &testBasicIguanaId);
   pApp->add("IguanId/default_return_machine_id", &testReturnsMachineIdIfDefaultLocation);
   pApp->add("IguanId/uniqueness",                &testIguanaIdUniqueness);
   pApp->add("IguanId/long_directory",            &testIguanaIdLongDirectoryPath);
   pApp->add("IguanId/hex",                       &testConvertFromHexToIguanaId);
}