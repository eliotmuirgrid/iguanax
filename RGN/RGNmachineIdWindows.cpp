//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: RGNmachineId
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Mon 28/06/1999
//
//---------------------------------------------------------------------------
#include "RGNprecomp.h"
#pragma hdrstop

#include "RGNmachineId.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <iphlpapi.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <REG/REGregistryWindows.h>
#include <nb30.h>
#include <FIL/FILutils.h>
#include <SFI/SFIhexEncodeFilter.h>
#include <COL/COLdll.h>
#include <COL/COLdateTime.h>
#include <COL/COLmath.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

const COLint16 RGN_MACHINE_ID_DATE_LENGTH = 10;
const char* CHMlongCode = "SDFDHDDGFWI";
const char* CHMnoNetworkCardMachineId = "SD43FHGGWFKF";

const char* CHMmachineIdSubPath = "Classes\\Ld.Banner";

//From registry
const char* CHMregistrationKey = "RegistrationKey";
const char* CHMmachineId     = "Version";
const char* CHMexpiryDate    = "ReleaseDate";
const char* CHMdoesNotExpire = "ModeType";

//For error reporting
const char* CHMregKey        = "CHM_REGKEY";
const char* CHMregExpiry     = "CHM_EXPIRY_DATE";

COLstring CHMregistrySubPath = "INTERFACEWARE\\CHAMELEON";

bool RGNisWindowsVersionAtLeast(DWORD ExpectedMajor, DWORD ExpectedMinor) {
   COL_FUNCTION(RGNisWindowsVersionAtLeast);
   OSVERSIONINFO OsVersionInfo;
   memset(&OsVersionInfo, 0, sizeof(OsVersionInfo));
   OsVersionInfo.dwOSVersionInfoSize = sizeof(OsVersionInfo);

   if(!GetVersionEx(&OsVersionInfo)) {
      return false; // assume that it's not as high as we hoped
   }

   if(OsVersionInfo.dwMajorVersion > ExpectedMajor || (OsVersionInfo.dwMajorVersion == ExpectedMajor && OsVersionInfo.dwMinorVersion >= ExpectedMinor)) {
      return true;
   }
   return false;
}

COLstring RGNsoftwareRegistryPath(const COLstring& SubPath) {
   COL_FUNCTION(RGNsoftwareRegistryPath);
   // a function which returns a complete SOFTWARE\\ key,
   // but referring to the Wow6432 if we're Win64
   COLstring RegistryPath = "SOFTWARE\\";
#if defined(_WIN64)
   bool DoWow6432Thing = true; // we do it by default, unless we recognize a situation where we don't

   if(SubPath == CHMmachineIdSubPath) { // this is a fix to resolve the specific issue with duplicate Machine IDs
      // we're accessing the SOFTWARE\\Classes registry subtree - it behaves differently in Windows 7 - not subject to the Wow6432Node effect
      // which affects Machine ID caching, storing 2 separate, potentially different copies of cached Machine ID for 32-bit vs. 64-bit modes
      if(RGNisWindowsVersionAtLeast(6,1)) {
         DoWow6432Thing = false;
      }
   }

   if(DoWow6432Thing) {
      RegistryPath += "Wow6432Node\\";
   }
#endif
   RegistryPath += SubPath;
   COL_VAR(RegistryPath);
   return RegistryPath;
}

COLstring CHMregistryPath() {
   COL_FUNCTION(CHMregistryPath);
   COLstring RegistryPath = RGNsoftwareRegistryPath(CHMregistrySubPath);
   COL_VAR(RegistryPath);
   return RegistryPath;
}

COLstring CHMmachineIdPath() {
   COL_FUNCTION(CHMmachineIdPath);
   COLstring MachineIdPath = RGNsoftwareRegistryPath(CHMmachineIdSubPath);
   COL_VAR(MachineIdPath);
   return MachineIdPath;
}

typedef struct _ASTAT_
{
   ADAPTER_STATUS adapt;
   NAME_BUFFER    NameBuff [30];
}ASTAT, * PASTAT;

static const char* pAdminNeededError = "Could not read/write registry entry. Please restart the application as a user with Administrator access to initialize the registry.";

void RGNsetUpRegistrationEntries() {
   //try to open and read+create entries,
   //these functions will throw if there is a read/write error to the registry
   RGNmachineId();
   RGNregistrationCode();
   RGNexpiryDate();
   RGNdoesNotExpire();
}

void RGNsetRegistryPath(const COLstring& RegistrySubPath) {
   COLstring SubPathNoSlash = RegistrySubPath;
   if (SubPathNoSlash[SubPathNoSlash.size()-1] == '\\') {
      SubPathNoSlash = SubPathNoSlash.substr(0, SubPathNoSlash.size()-1);
   }
   CHMregistrySubPath = SubPathNoSlash;
   COL_VAR(CHMregistrySubPath);
}

COLstring RGNscrambleId(const COLstring& Id) {
   COLstring Result(strlen(Id.c_str()), ' ');

   for (COLindex CharIndex = 0; CharIndex < strlen(Id.c_str()); CharIndex++) {
      Result[CharIndex] = Id[CharIndex] + 2;
   }
   return Result;
}

COLstring RGNunscrambleId(const COLstring& Id) {
   COLstring Result(strlen(Id.c_str()), ' ');
   for (COLindex CharIndex = 0; CharIndex < strlen(Id.c_str()); CharIndex++) {
      Result[CharIndex] = Id[CharIndex] - 2;
   }
   COL_VAR2(Id, Result);
   return Result;
}

COLstring RGNmakeDummyString() {
   COLstring RandomString;
   COLmath::resetRandomGenerator();
   for (COLuint32 CharIndex=0; CharIndex < 15; CharIndex++) {
      COLuint32 RandomNumber = COLmath::randomNumber(35);
      if (RandomNumber <=25 ) {
         RandomString += (char)('A' + RandomNumber);
      } else {
         RandomString += (char)('0' + RandomNumber - 26);
      }
   }
   return RandomString;
}

COLstring RGNmakeDummyId() {
   COL_FUNCTION(RGNmakeDummyId);
   try {
      REGregistry Key2(HKEY_LOCAL_MACHINE);
      Key2.openKey(CHMmachineIdPath(), true);
      Key2.writeString(CHMmachineId, RGNscrambleId(RGNmakeDummyString()));
   } catch (const COLerror&) {
      COL_ERROR_STREAM(pAdminNeededError,COLerror::SystemError);
   }
   return "";
}

typedef UCHAR (__stdcall *RGNnetCall)(NCB* pNcb);


void putWcharStringInVar(PWCHAR In, const COLstring& Name, COLvar* pVar) {
   COL_FUNCTION(putWhcharStringInVar);
   COLstring Tmp(In);
   (*pVar)[Name] = Tmp;
}

COLstring RGNmacAddress() {
   COL_FUNCTION(RGNmacAddress);
   DWORD ReturnCode = 0;
   int Multiplier = 10;
   unsigned long TankSize = sizeof(IP_ADAPTER_ADDRESSES) * Multiplier;
   bool Working = true;
   PIP_ADAPTER_ADDRESSES pAddresses = NULL;
   PIP_ADAPTER_ADDRESSES pCurr      = NULL;

   while (Working) {
      pAddresses = new IP_ADAPTER_ADDRESSES[TankSize];
      ReturnCode = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_ALL_INTERFACES, NULL, pAddresses, &TankSize);
      switch (ReturnCode) {
         case ERROR_BUFFER_OVERFLOW:
            delete pAddresses;
            pAddresses = NULL;
            break;
         case NO_ERROR:
            Working = false;
            break;
         default:
            delete pAddresses;
            pAddresses = NULL;
            COLASSERT(false);
            break;
      }
   }

   pCurr = pAddresses;
   COLstring Result;
   while (pCurr) {
      COLvar Look;
      Look["adapter_name"] = pCurr->AdapterName;
      putWcharStringInVar(pCurr->Description, "description", &Look);
      putWcharStringInVar(pCurr->FriendlyName, "friendly_name", &Look);
      COLstring Mac((char*)pCurr->PhysicalAddress, pCurr->PhysicalAddressLength);
      COLstring HexMac = SFIhexEncode(Mac).toLowerCase();
      if (Result.is_null() && !HexMac.is_null()) {
         Result = HexMac;
      }
      Look["physical_address"] = HexMac;
      COL_VAR(Look.json(true, COLvar::StrEncJs));
      pCurr = pCurr->Next;
   }

   if (pAddresses) {
      delete pAddresses;
      pAddresses = NULL;
   }
   COL_VAR(Result);
   return Result;
}

COLstring RGNmachineId() {
	COL_FUNCTION(RGNmachineId);
   // first try and get it from the registry
   try {
      REGregistry Key(HKEY_LOCAL_MACHINE);
      Key.openKey(CHMmachineIdPath(), true);
      COLstring Id = Key.readString(CHMmachineId, "NOID");
      if (Id != "NOID") {
         Id = RGNunscrambleId(Id);
         if (Id != "NONETWORKCARD") {
            COL_TRC("Machine id fetched from the registry.");
            COL_VAR(Id);
            return Id;
         }
         // else, we'll generate a new code and cache it in the registry
      }
   } catch (const COLerror&) {
      COL_ERROR_STREAM(pAdminNeededError,COLerror::SystemError);
   }

   COL_TRC("Generating a new machine id.");
	COLstring Result = RGNmacAddress();
	COLstring FinalCode;

	if (!Result.is_null()) {
		for (COLindex CharIndex = Result.size() - 1; CharIndex > 0; CharIndex--) {
			if ('0' != Result[CharIndex]) {
				char Character = Result[CharIndex];
				if ('1' <= Character && '9' >= Character) {
					FinalCode += char('A' + Character - '1');
				}
				if ('a' <= Character && 'f' >= Character) {
					FinalCode += char('K' + Character - 'a');
				}
			}
		}
	}

	if (FinalCode.size() == 0) {
		FinalCode = CHMnoNetworkCardMachineId;
	}
	if (FinalCode.size() > RGN_MACHINE_ID_MAC_ADDRESS_LENGTH) {
		FinalCode = FinalCode.substr(0, RGN_MACHINE_ID_MAC_ADDRESS_LENGTH);
	}

   try {
      REGregistry Key2(HKEY_LOCAL_MACHINE);
      Key2.openKey(CHMmachineIdPath(), true);
      Key2.writeString(CHMmachineId, RGNscrambleId(FinalCode));
   } catch(const COLerror& Error) {
      COL_ERROR_STREAM(pAdminNeededError,COLerror::SystemError);
   }
	COL_VAR(FinalCode);
	return FinalCode;
}

COLstring RGNfullMachineId(const COLstring& OrigMachineId, const COLstring& ExpiryDate, int DoesNotExpire) {
   COLstring MachineId;
   if (1 == DoesNotExpire) {
      MachineId = OrigMachineId + CHMlongCode;
   } else {
      MachineId = OrigMachineId + ExpiryDate;
   }
   COL_VAR(MachineId);
   return MachineId;
}


COLstring RGNdemoKey(const COLstring& OrigMachineId, const COLstring& ExpiryDate, int DoesNotExpire) {
   COL_FUNCTION(RGNdemoKey);
   COLstring MachineId = RGNfullMachineId(OrigMachineId, ExpiryDate, DoesNotExpire);
   COLstring Key;
   COLPRECONDITION(MachineId.size() >= 2);
   for (COLindex CharIndex = MachineId.size() - 1; CharIndex > 1; CharIndex-=2) {
      Key += 'Z' - MachineId[CharIndex] + 'A';
   }
   COL_VAR(Key);
   return Key;
}

COLstring RGNextensionKey(const COLstring& OrigMachineId, const COLstring& ExpiryDate, int DoesNotExpire) {
   COL_FUNCTION(RGNextensionKey);
   COLstring Key;
   COLstring MachineId = RGNfullMachineId(OrigMachineId, ExpiryDate, DoesNotExpire);
   COLPRECONDITION(MachineId.size() > 0);
   for (COLindex CharIndex = MachineId.size() - 1; CharIndex > 0; CharIndex--) {
      Key += 'M' - MachineId[CharIndex] + 'A';
   }
   COL_VAR(Key);
   return Key;
}

COLstring RGNlicenseKey(const COLstring& OrigMachineId, const COLstring& ExpiryDate, int DoesNotExpire) {
   COL_FUNCTION(RGNlicenseKey);
   const long q = 33654393L;
   const long q32 = q<<5;

   long Hash  = 0 ;
   COLstring MachineId = RGNfullMachineId(OrigMachineId, ExpiryDate, DoesNotExpire);
   COLPRECONDITION(MachineId.size() > 0);

   for (COLindex CharIndex = MachineId.size() - 1; CharIndex > 0; CharIndex--) {
      Hash = ( (Hash<<5) + MachineId[CharIndex] ) % q;
   }

   char Buffer[16];
   snprintf(Buffer, 16, "%04x", Hash);
   Hash = (Hash + 1231231L) % 2;
   snprintf(Buffer + 4, 16 - 4, "%04x", Hash);

   COL_VAR(Buffer);
   return Buffer;
}

COLstring RGNregistrationCode() {
   COL_FUNCTION(RGNregistrationCode);
   COLstring RegKey;
   try {
      REGregistry Key(HKEY_LOCAL_MACHINE);
      Key.openKey(CHMregistryPath(), true);
      RegKey = Key.readString(CHMregistrationKey, "NOTREGISTERED");
   } catch(const COLerror&) {
      COL_ERROR_STREAM(pAdminNeededError,COLerror::SystemError);
   }
   COL_VAR(RegKey);
   return RegKey;
}

COLstring RGNexpiryDate() {
   COL_FUNCTION(RGNexpiryDate);
   COLstring Date ;
   try {
      REGregistry Key(HKEY_LOCAL_MACHINE);
      Key.openKey(CHMregistryPath(), true);
      Date = Key.readString(CHMexpiryDate, "40000101");
   } catch(const COLerror&) {
      COL_ERROR_STREAM(pAdminNeededError,COLerror::SystemError);
   }
   COL_VAR(Date);
   return Date;
}

void RGNexpiryDateAsDateTime(COLdateTime& Date) {
   try {
      COLstring DateAsString = RGNexpiryDate();
      Date = COLdateTime();
      if (!DateAsString.is_null()) {
         int nYear=0, nMonth=0, nDay=0, nHour=0, nMin=0, nSec=0;
         int ReturnCode = sscanf(DateAsString.c_str(), "%04d%02d%02d",
                                &nYear,
                                &nMonth,
                                &nDay);
         if( ReturnCode != 3 ) {
           COL_ERROR_STREAM("Unable to get expiry date from " << DateAsString ,COLerror::SystemError);
         }
         Date.setDateTime( nYear, nMonth, nDay, nHour, nMin, nSec );
      }
   } catch(const COLerror&) {
      COL_ERROR_STREAM(pAdminNeededError,COLerror::SystemError);
   }
}

// Pretty pointless function, now that it's a no-op.
bool RGNnoLicenseRequired() {
   return false;
}

COLint32 RGNdoesNotExpire() {
   if (::RGNnoLicenseRequired()) {
      return 1;
   }
   COLstring Flag;
   try {
      REGregistry Key(HKEY_LOCAL_MACHINE);
      Key.openKey(CHMregistryPath(), true);
      Flag = Key.readString(CHMdoesNotExpire, "F");
   } catch(const COLerror&) {
      COL_ERROR_STREAM(pAdminNeededError,COLerror::SystemError);
   }
   if ("T" == Flag) {
      return 1;
   } else {
      return 0;
   }
}

RGNlicenseLevel RGNregistrationLevel(
   const COLstring& MachineId,
   const COLstring& iRegKey,
   const COLstring& ExpiryDate,
   int              DoesNotExpire
)
{
   COL_FUNCTION(RGNregistrationLevel);
   COL_VAR(iRegKey);
   COLstring RegKey(iRegKey);
   RegKey.stripAll(' ');
   if (::RGNnoLicenseRequired()) {
      return RGN_LICENSE_FULL;
   }
   COLstring Hash;

   if (1 != DoesNotExpire) {
      // check that now < ExpiryDate
      time_t tNow;
      time(&tNow);
#ifdef _WIN32
      struct tm* tmNow = localtime(&tNow); // Windows uses thread-local tm struct
#else
      struct tm tmStruct;
      localtime_r(&tNow, &tmStruct); // localtime() is not threadsafe on POSIX
      struct tm* tmNow = &tmStruct;
#endif
      char bufNow[RGN_MACHINE_ID_DATE_LENGTH];
      strftime(bufNow, RGN_MACHINE_ID_DATE_LENGTH, "%Y%m%d", tmNow);
      long lExpiry = atoi(ExpiryDate.c_str());
      long Now = atoi(bufNow);
      if (Now > lExpiry) {
         return RGN_EXPIRED;
      }
   }

   Hash = RGNdemoKey(MachineId, ExpiryDate, DoesNotExpire);
   if (Hash.compare(RegKey) == 0) {
      return RGN_LICENSE_DEMO;  // demo license
   }

   Hash = RGNextensionKey(MachineId, ExpiryDate, DoesNotExpire);
   if (Hash.compare(RegKey) == 0) {
      return RGN_LICENSE_EXTENSION;  // extension license
   }

   Hash = RGNlicenseKey(MachineId, ExpiryDate, DoesNotExpire);
   if (Hash.compare(RegKey) == 0) {
      return RGN_LICENSE_FULL;  // fully licensed
   }
   return RGN_NOT_LICENSED;// not licensed
}

void RGNsetRegistrationCode(const char* iCode, const char* ExpiryDate, int DoesNotExpire) {
   COL_FUNCTION(RGNsetRegistrationCode);
   COL_VAR(iCode);
   try {
      COLstring Code(iCode);
      Code.stripAll(' ');
      REGregistry Key(HKEY_LOCAL_MACHINE);
      Key.openKey(CHMregistryPath(), true);
      Key.writeString(CHMregistrationKey, Code.c_str());
      Key.writeString(CHMexpiryDate, ExpiryDate);
      if (1 == DoesNotExpire) {
         Key.writeString(CHMdoesNotExpire, "T");
      } else {
         Key.writeString(CHMdoesNotExpire, "F");
      }
   } catch(const COLerror&) {
      COL_ERROR_STREAM(pAdminNeededError,COLerror::SystemError);
   }
}

void RGNcheckLicense() {
   COL_FUNCTION(RGNcheckLicense);
   RGNlicenseLevel LicenseLevel = RGNregistrationLevel(RGNmachineId(), RGNregistrationCode(), RGNexpiryDate(), RGNdoesNotExpire());

   COL_VAR(LicenseLevel);
   if ((LicenseLevel == RGN_EXPIRED) || (LicenseLevel == RGN_NOT_LICENSED)) {
      COLstring ErrorString;
      COLostream ErrorStream(ErrorString);
      ErrorStream << "This host machine does not have a valid Chameleon license." << newline;
      ErrorStream << "To set this license code please select Chameleon from the Start menu" << newline;
      ErrorStream << "and follow the instructions described there to obtain the license code.";

      throw COLerror(ErrorString, RGN_REGISTRATION_ERROR);
   }
}

bool RGNisLicensed() {
   COL_FUNCTION(RGNisLicensed);
   RGNlicenseLevel LicenseLevel = RGNregistrationLevel(RGNmachineId(), RGNregistrationCode(), RGNexpiryDate(), RGNdoesNotExpire());

   bool IsLicensed = LicenseLevel != RGN_EXPIRED && LicenseLevel != RGN_NOT_LICENSED;
   COL_VAR(IsLicensed);
   return IsLicensed;
}
