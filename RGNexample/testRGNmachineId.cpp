//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Implementation of RGNmachineId tests
//
// Author: George Ma
// Date:   Tue 03/02/2004
//
//---------------------------------------------------------------------------

#include "RGNexamplePrecomp.h"
#pragma hdrstop

// for using isxdigit
#include <ctype.h>

#include <FIL/FILfile.h>
#include <RGN/RGNmachineId.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>
#include <TST/TSThelpers.h>
#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifndef _WIN32
#include <PRO/PROenv.h>
#endif
#include <RGN/RGNhash.h>

#ifdef __APPLE__
#include <COL/COLmap.h>
#endif

COLstring RGNmacAddress();

void testRGNmacAddress()
{
   COL_FUNCTION(testRGNmacAddress);
   COLstring MacAddress = RGNmacAddress();

   COL_TRC("RGNmacAddress returns " << MacAddress << ".");

   bool IsHex = true;
   for (int i = 0; i < MacAddress.size(); ++i) {
      if ( ! isxdigit(MacAddress[i]) ) {
         IsHex = false;
         break;
      }
   }

   COL_TRC("Check if MacAddress appears valid, i.e. it's hex digit and 12 chararcters long.");
   COL_VAR3(MacAddress, MacAddress.size(), IsHex);
   
   // On Linux the MacAddress can be "" if eth0 doesn't exist. Rather than improve on this
   // it's going to be left alone since fixing it may break many licenses.
   if ( ! MacAddress.is_null()) {
      COL_TRC("MacAddress was not null.");
      TST_ASSERT( (MacAddress.size() == 12) && IsHex );
   } else {
      COL_TRC("MacAddress was null.");
   }
}

#ifdef _WIN32

#include <windows.h>
extern COLstring RGNsoftwareRegistryPath(const COLstring& SubPath);
extern const char* CHMmachineIdSubPath;
extern bool RGNisWindowsVersionAtLeast(DWORD ExpectedMajor, DWORD ExpectedMinor);

void testRGNmachineIdPath()
{
   COL_FUNCTION(testRGNmachineIdPath);
   // display the registry path where we store the cached Machine ID

   COLstring MachineIdPath = RGNsoftwareRegistryPath(CHMmachineIdSubPath);
   COL_TRC("Machine ID path: " << MachineIdPath);

   COLstring ExpectedMachineIdPath = "SOFTWARE\\Classes\\Ld.Banner";
   if(!RGNisWindowsVersionAtLeast(6,1))
   {
      ExpectedMachineIdPath = "SOFTWARE\\Wow6432Node\\Classes\\Ld.Banner";
   }
   TST_ASSERT_EQUALS(ExpectedMachineIdPath, MachineIdPath);
}

#endif

void testRGNmachineId()
{
   RGNcheckLicense();
   TST_ASSERT(RGNisLicensed());
}


#define RGNT_TEST_HOSTNAME(HostName, Expected) COL_TRC(HostName << " -> " << RGNmachineIdFromHostName(HostName)); TST_ASSERT_EQUALS(COLstring(Expected), RGNmachineIdFromHostName(HostName));

void testRGNmachineIdFromHostName()
{
   RGNT_TEST_HOSTNAME("", "5_8759D93A71");
   RGNT_TEST_HOSTNAME("z", "5_C599DF81CA");
   RGNT_TEST_HOSTNAME("zz", "5_BD82FD36B4");
   RGNT_TEST_HOSTNAME("zzz", "5_5604AEF772");
   RGNT_TEST_HOSTNAME("ab", "5_CC7E1066D3");
   RGNT_TEST_HOSTNAME("ba", "5_A99B0E50DB");
   RGNT_TEST_HOSTNAME("ubuntu-8-04-x64aaaaa", "5_E770452FD3");
   RGNT_TEST_HOSTNAME("ubuntu-8-04-x64aaaa", "5_18C6B6B85F");
   RGNT_TEST_HOSTNAME("ubuntu-8-04-x64aaa", "5_C93312186F");
   RGNT_TEST_HOSTNAME("ubuntu-8-04-x64aa", "5_6C54F742E4");
   RGNT_TEST_HOSTNAME("ubuntu-8-04-x64a", "5_4281896D7D");
   RGNT_TEST_HOSTNAME("ubuntu-8-04-x64", "5_5EAFC402D1");
   RGNT_TEST_HOSTNAME("ubuntu-8-04-x65", "5_4459E120D3");
   RGNT_TEST_HOSTNAME("ubuntu-8-04-x66", "5_AD22CCF7DB");
   RGNT_TEST_HOSTNAME("ubuntu-8-04-x67", "5_4C88A97660");
   RGNT_TEST_HOSTNAME("ubuntu-8-04-x68", "5_C53AE60FF3");
   RGNT_TEST_HOSTNAME("ubuntu-8-04-x69", "5_748D115003");
   RGNT_TEST_HOSTNAME("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz", "5_7D51973982");
}


// This is a scoped class allowing us to temporarily change the license
// because it is scoped it should be exception safe.
class RGNtestTempRegistrationChange
{
public:
   RGNtestTempRegistrationChange() : OrigDoesNotExpire(0)
   {
      OrigDoesNotExpire = RGNdoesNotExpire();
      OrigRegistrationCode = RGNregistrationCode();
      OrigExpiryDateAsString = RGNexpiryDate();
   }
   ~RGNtestTempRegistrationChange()
   {
      try
      {
         #ifdef _WIN32
            RGNsetRegistrationCode(OrigRegistrationCode.c_str(),OrigExpiryDateAsString.c_str(),OrigDoesNotExpire);
         #else
            PROsetenv( "CHM_REGKEY", OrigRegistrationCode );
            if (OrigDoesNotExpire)
            {
               PROsetenv( "CHM_EXPIRY_DATE", "NOEXPIRY" );
            }
            else
            {
               PROsetenv( "CHM_EXPIRY_DATE", OrigExpiryDateAsString );
            }
         #endif
      }
      catch(...)
      {
         COLASSERT(0 && "Cannot Reset Registration Entries");
      }
   }

   void setRegistration(const COLdateTime& ExpiryDate,bool NotExpiring)
   {
      COLstring ExpiryDateAsString;
      if (!ExpiryDate.isNull())
      {
         ExpiryDateAsString = ExpiryDate.format("%Y%m%d");
      }
      COLstring NewLicense = RGNlicenseKey(RGNmachineId(),ExpiryDateAsString.c_str(),(NotExpiring ? 1 : 0) );
#ifdef _WIN32
      RGNsetRegistrationCode(NewLicense.c_str(),ExpiryDateAsString.c_str(),(NotExpiring ? 1 : 0));
#else
      PROsetenv( "CHM_REGKEY", NewLicense );
      if (NotExpiring)
      {
         PROsetenv( "CHM_EXPIRY_DATE", "NOEXPIRY" );
      }
      else
      {
         PROsetenv( "CHM_EXPIRY_DATE", ExpiryDateAsString );
      }
#endif
   }

private:
   COLstring OrigRegistrationCode;
   COLstring OrigExpiryDateAsString;
   COLint32 OrigDoesNotExpire;
};

void testRGNexpiryDateAsDateTime()
{
   {
      RGNtestTempRegistrationChange TempChange;
      COLdateTime InputExpiryDate;
      InputExpiryDate.setDateTime(2000,1,1,0,0,0);
      TempChange.setRegistration(InputExpiryDate,false);

      COLdateTime OutputDate;
      RGNexpiryDateAsDateTime(OutputDate);

      TST_ASSERT(InputExpiryDate == OutputDate);
   }

   {
      RGNtestTempRegistrationChange TempChange;
      COLdateTime InputExpiryDate;
      TempChange.setRegistration(InputExpiryDate,false);

      COLdateTime OutputDate;
      RGNexpiryDateAsDateTime(OutputDate);

      TST_ASSERT(OutputDate.isNull());
   }
}

TSTtestable* getRGNTests()
{
   TST_START_COLLECTION("RGN Tests")
      TST_ADD_TEST_FUNCTION(testRGNmacAddress);
#ifdef _WIN32
      TST_ADD_TEST_FUNCTION(testRGNmachineIdPath);
#endif
      TST_ADD_TEST_FUNCTION(testRGNmachineId);
      TST_ADD_TEST_FUNCTION(testRGNmachineIdFromHostName);
      TST_ADD_TEST_FUNCTION(testRGNexpiryDateAsDateTime);
   TST_END_COLLECTION
}
