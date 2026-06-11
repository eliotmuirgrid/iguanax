//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: RGNmachineIdPosix
//
// Description:
//
// Class for getting a unique machine ID
//
// Author: Eliot Muir
// Date:   Mon 28/06/1999
//
//---------------------------------------------------------------------------

#include "RGNprecomp.h"
#pragma hdrstop

#include "RGNobfuscate.h"
#include "RGNmachineId.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// The defined blocks below used to include RGNmacAddressLinux.h, RGNmacAddressBDD.h,
// and RGNmacAddressNone.h. They each provide a different implementation for getting
// the mac address on different platforms. I have just included them here because
// it's just one function and now we can trace them.

#if defined(__linux__)

// FROM RGNmacAddressLinux.h

#include <linux/sockios.h>
#include <cerrno>

COLstring RGNmacAddress() {
   COL_FUNCTION(RGNmacAddress);
   COL_TRC("On Linux");
   COLstring MACAddress(RGN_MACHINE_ID_MAC_ADDRESS_LENGTH,'\0');
   COLint32 Socket = socket(PF_INET,SOCK_DGRAM, 0);

   if (Socket < 0) {
      COL_TRC("Call to socket failed.");
      // Checking the return value, but not throwing an exception on failure.
      // We don't want to let a customer know how the machine id is generated.
      return "";
   }

   struct ifreq Buffer;
   strcpy(Buffer.ifr_ifrn.ifrn_name, "eth0");

   if (ioctl(Socket, SIOCGIFHWADDR, &Buffer) < 0) {
      COL_TRC("Call to ioctl failed.");
      COL_VAR(strerror(errno));
      // close the socket and return the empty string if the ioctl call has failed.
      close(Socket);
      return "";
   }
   close(Socket);
   RGNprintMACAddress(MACAddress.get_buffer(), MACAddress.capacity(), (COLuint8*) &Buffer.ifr_ifru.ifru_hwaddr.sa_data);
   COL_VAR(MACAddress);
   return MACAddress;
}

// END of defined(__linux__) ******************************

#elif defined(__APPLE__)

#include <sys/sysctl.h>
static bool RGNmacIsSequoiaOrNewer(){
   COL_FUNCTION(RGNmacIsSequoiaOrNewer);
   int mib[2];
   size_t len;
   char version[256];
   // Set up sysctl to get the macOS version
   mib[0] = CTL_KERN;
   mib[1] = KERN_OSRELEASE;
   len = sizeof(version);
   // get the macOS version
   if (sysctl(mib, 2, version, &len, NULL, 0) != -1) { // annoying that this returns the internal os version and not the one that Apple icon displays. This means Sonoma is 23.x.x and Sequoia is 24.x.x
      COL_TRC("macOS Version: " << version);
      COLstring MacOsMajorVersion(version);
      COLstring Temp;
      MacOsMajorVersion.split(MacOsMajorVersion, Temp, ".");
      return 24 <= atoi(MacOsMajorVersion.c_str());
   } 
   return false;
}

#include <PRO/PROexecute.h>
static COLstring RGNmacAddressSequoia(){
   COL_FUNCTION(RGNmacAddressSequoia);
   COLstring Out, Err;
   COLstring Command("/bin/bash -c \"ioreg -rd1 -c IOPlatformExpertDevice | grep -i 'IOPlatformUUID'\"");
   int Result = PROexecuteString(".", Command, &Out, &Err, 1); // this will give "IOPlatformUUID" = "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX"
   COL_VAR3(Result, Out, Err);
   if (Out.is_null()) { COL_TRC("Found no hardware uuid."); return ""; }
   Out = COLremoveAllWhiteSpace(Out);
   Out.stripAll('-');
   COL_VAR(Out);
   if (Out.size() < 36) { COL_TRC("Failed to parse uuid"); return ""; }
   Out = Out.substr(Out.find_last_of("=")+2, 32);
   COL_VAR(Out);
   return Out;
}
// From RGNmacAddressBSD.h
#include <net/if_dl.h>
#include <net/if_types.h>
#include <COL/COLmap.h>
const COLint32 RGN_MACHINE_ID_BUFFER_SIZE = 4096;

COLstring RGNmacAddress() {
   COL_FUNCTION(RGNmacAddress);
   COL_TRC("On BSD/MAC");
   if (RGNmacIsSequoiaOrNewer()) {
      COL_TRC("We are on Sequoia or newer, use hardware uuid instead of mac address"); // IX-3766 due to rotating MAC addresses 
      return RGNmacAddressSequoia(); 
   }
   struct   ifconf InterfaceConfig;
   struct   ifreq* pInterfaceRequest;
   struct   sockaddr_dl* pSocketAddress;
   char     TempBuffer[RGN_MACHINE_ID_BUFFER_SIZE];
   char*    pByte;

   COLmap<COLstring,COLstring> AddressList;
   COLint32 Socket = socket(AF_INET, SOCK_DGRAM, 0);

   // Check the socket call
   // In RGNMACAddress we are simply returning the empty string
   // if an API call fails.  This is because we don't want to give
   // our users information on how the machine id is generated
   if (Socket < 0)
      return "";

   InterfaceConfig.ifc_len = sizeof(TempBuffer);
   InterfaceConfig.ifc_buf = TempBuffer;

   if (ioctl(Socket, SIOCGIFCONF, &InterfaceConfig) < 0) {
      close(Socket);
      return "";
   }

   close(Socket);
   for (pByte = TempBuffer; pByte < TempBuffer + InterfaceConfig.ifc_len ; ) {
      pInterfaceRequest = (struct ifreq*) pByte;

      if (pInterfaceRequest->ifr_addr.sa_family == AF_LINK) {
         pSocketAddress = (struct sockaddr_dl*) &pInterfaceRequest->ifr_addr;

         if (pSocketAddress->sdl_type == IFT_ETHER) {
            COLstring InterfaceName = pInterfaceRequest->ifr_name;
            COLstring MACAddress(RGN_MACHINE_ID_MAC_ADDRESS_LENGTH, '\0');
            RGNprintMACAddress(MACAddress.get_buffer(), MACAddress.capacity(), (COLuint8*) LLADDR(pSocketAddress));
            COL_VAR2(InterfaceName, MACAddress);
            AddressList.add(InterfaceName, MACAddress);
         }
      }
      pByte += _SIZEOF_ADDR_IFREQ(*pInterfaceRequest);
   }

   COLstring En0 = AddressList["en0"];
   if (En0 != "") {
      COL_TRC("Returning mac address for en0: " << En0);
      return En0;
   }

   COLstring En1 = AddressList["en1"];
   if (En1 != "") {
      COL_TRC("Returning mac address for en1: " << En1);
      return En1;
   }

   COLmapPlace LastPlace = AddressList.last();
   COLstring Last = AddressList[LastPlace];

   if (Last != "") {
      COL_TRC("Returning the last item in the list, to ensure it's not awdlo (see #25927): " << Last);
      return Last;
   }

   COL_TRC("Found no mac address. This simply shoudn't be ...");
   return "";
};
// END of defined(CHM#_BSD) || defined(__APPLE__) *******************************************

#else

// RGNmacAddressNone.h
COLstring RGNmacAddress() {
   COL_FUNCTION(RGNmacAddress);
   COL_TRC("On RGNmacAddressNone");
   return "";
}

#endif

// END of conditional definitions of RGNmacAddress() *************************************

#include "RGNhash.h"

const COLint16 RGN_MACHINE_ID_DATE_LENGTH = 10;
const char* CHMlongCode = "SDFDHDDGFWI";

#define MAGIC_KEY "26246235277372889332353428082271853562675930014984348193034932751056110753915"

const char* CHMregistrationKey = "RegistrationKey";
const char* CHMmachineId     = "Version";
const char* CHMexpiryDate    = "ReleaseDate";
const char* CHMdoesNotExpire = "ModeType";
const char* CHMregKey        = "CHM_REGKEY";
const char* CHMregExpiry     = "CHM_EXPIRY_DATE";

void RGNsetUpRegistrationEntries()
{
   //nothing to do
}

void RGNsetRegistryPath(const COLstring& RegistryPath)
{
   // does nothing on POSIX
}

COLstring RGNscrambleId(const COLstring& Id)
{
   COL_FUNCTION(RGNscrambleId);

   COLstring Result(Id.size(), ' ');
   for (COLindex CharIndex = 0; CharIndex < Id.size(); CharIndex++)
   {
      Result[CharIndex] = Id[CharIndex] + 2;
   }
   return Result;
}

COLstring RGNunscrambleId(const COLstring& Id)
{
   COL_FUNCTION(RGNunscrambleId);
   COLstring Result(Id.size(), ' ');
   for (COLindex CharIndex = 0; CharIndex < Id.size(); CharIndex++)
   {
      Result[CharIndex] = Id[CharIndex] - 2;
   }
   return Result;
}

void RGNprintMACAddress(char* pBuffer, int Capacity, unsigned char* pMACAddress)
{
   COL_FUNCTION(RGNprintMACAddress);
   COLASSERT(Capacity > RGN_MACHINE_ID_MAC_ADDRESS_LENGTH);
   snprintf(pBuffer, Capacity, "%02x%02x%02x%02x%02x%02x",
      pMACAddress[0], pMACAddress[1], pMACAddress[2],
      pMACAddress[3], pMACAddress[4], pMACAddress[5]);
}

COLstring RGNlegacyMachineId()
{
   COL_FUNCTION(RGNlegacyMachineId);
   COLstring MachineId(RGN_MACHINE_ID_MAC_ADDRESS_LENGTH,'\0');
   COLstring MachineIdResult;

   snprintf(MachineId.get_buffer(), MachineId.capacity(), "%8x", (unsigned)gethostid());
   MachineId.stripAll(' ');
   for (COLindex CharIndex = 0; CharIndex < MachineId.size(); CharIndex++)
   {
      MachineIdResult += MachineId[CharIndex] + 3;
   }
   return MachineIdResult;
}

COLstring RGNhostName()
{
   COL_FUNCTION(RGNhostName);
   const int BUFFER_SIZE = 256;
   char Buffer[BUFFER_SIZE+1];
   Buffer[0] = 0;
   int rc = gethostname(Buffer, BUFFER_SIZE);
   Buffer[BUFFER_SIZE]=0; // just in case
   return Buffer;
}

const char* RGN_MACHINE_ID_MODE = "CHM_MACHINE_ID_MODE";
const char* RGN_MODE_H5 = "h5";

COLstring RGNmachineId()
{
   COL_FUNCTION(RGNmachineId);
   COLstring MachineIdMode = ::getenv(RGN_MACHINE_ID_MODE);
   if(MachineIdMode == RGN_MODE_H5)
   {
      // if we have this variable set up, use the h5 mode regardless of whether or not we have eth0
      // this is intended for situations where MAC address is flaky,
      // while keeping it working unchanged where it's not an issue
      return RGNmachineIdFromHostName(RGNhostName());
   }

   COLstring MacAddress = RGNmacAddress();
   if (MacAddress.is_null())
   {
      return RGNlegacyMachineId();
   }

   COLstring FinalCode;
   for (COLindex CharIndex = MacAddress.size() - 1; CharIndex > 0; CharIndex--)
   {
      if ('0' != MacAddress[CharIndex])
      {
         char Character = MacAddress[CharIndex];
         if ('1' <= Character && '9' >= Character)
         {
            FinalCode += char('A' + Character - '1');
         }
         if ('a' <= Character && 'f' >= Character)
         {
            FinalCode += char('K' + Character - 'a');
         }
      }
   }
   if (FinalCode.size() == 0)
   {
      FinalCode = "SD43FHGGWFKF";
   }
   if (FinalCode.size() > RGN_MACHINE_ID_MAC_ADDRESS_LENGTH)
   {
      FinalCode = FinalCode.substr(0, RGN_MACHINE_ID_MAC_ADDRESS_LENGTH);
   }

   return FinalCode;
}

COLstring RGNfullMachineId(const COLstring& OrigMachineId, const COLstring& iExpiryDate, int DoesNotExpire)
{
   COL_FUNCTION(RGNfullMachineId);
   COLstring ExpiryDate(iExpiryDate);
   ExpiryDate.stripAll(' ');
   COLstring MachineId;

   if (1 == DoesNotExpire)
   {
      MachineId = OrigMachineId + CHMlongCode;
   }
   else
   {
      MachineId = OrigMachineId + ExpiryDate;
   }
   return MachineId;
}


COLstring RGNdemoKey(const COLstring& OrigMachineId, const COLstring& iExpiryDate, int DoesNotExpire)
{
   COL_FUNCTION(RGNdemoKey);
   COLstring ExpiryDate(iExpiryDate);
   ExpiryDate.stripAll(' ');

   COLstring MachineId = RGNfullMachineId(OrigMachineId,
                                          ExpiryDate,
                                          DoesNotExpire);
   COLstring Key;
   COLPRECONDITION(MachineId.size() >= 2);
   for(COLindex CharIndex = MachineId.size() - 1; CharIndex > 1; CharIndex -= 2)
   {
      Key += 'Z' - MachineId[CharIndex] + 'A';
   }
   return Key;
}

COLstring RGNextensionKey(const COLstring& OrigMachineId, const COLstring& iExpiryDate, int DoesNotExpire)
{
   COL_FUNCTION(RGNextensionKey);
   COLstring ExpiryDate(iExpiryDate);
   ExpiryDate.stripAll(' ');

   COLstring Key;
   COLstring MachineId = RGNfullMachineId(OrigMachineId, ExpiryDate, DoesNotExpire);

   COLPRECONDITION(MachineId.size() > 0);
   for (COLindex CharIndex = MachineId.size()-1; CharIndex > 0; CharIndex--)
   {
      Key += 'M' - MachineId[CharIndex] + 'A';
   }
   return Key;
}

COLstring RGNlicenseKey
(
   const COLstring& OrigMachineId,
   const COLstring& iExpiryDate,
   int              DoesNotExpire
)
{
   COL_FUNCTION(RGNlicenseKey);
   COLstring ExpiryDate(iExpiryDate);
   ExpiryDate.stripAll(' ');
   const long q = 33654393L;
   const long q32 = q<<5;

   long Hash  = 0 ;
   COLstring MachineId = RGNfullMachineId(OrigMachineId,
                                          ExpiryDate,
                                          DoesNotExpire);
   COLPRECONDITION(MachineId.size() > 0);
   for(COLindex CharIndex = MachineId.size() - 1;
       CharIndex > 0;
       CharIndex--)
   {
      Hash = ( (Hash<<5) + MachineId[CharIndex] ) % q;
   }
   char Buffer[32]; // #10246 - the famous rhel5 -O2 problem
   snprintf(Buffer, 32, "%04x", (unsigned)Hash);
   Hash = (Hash + 1231231L) % 2;
   snprintf(Buffer + 4, 32 - 4, "%04x", (unsigned)Hash);

   COLstring Key = Buffer;
   return Key;
}

COLstring RGNregistrationCode()
{
   COL_FUNCTION(RGNregistrationCode);
   COLstring RegKey = getenv(CHMregKey);
   RegKey.stripAll(' ');
   return RegKey;
}

COLstring RGNexpiryDate()
{
   COL_FUNCTION(RGNexpiryDate);
   COLstring Date = getenv(CHMregExpiry);
   Date.stripAll(' ');
   return Date;
}

void RGNexpiryDateAsDateTime(COLdateTime& Date)
{
   COL_FUNCTION(RGNexpiryDateAsDateTime);
   COLstring DateAsString = RGNexpiryDate();
   Date = COLdateTime();
   if (!DateAsString.is_null())
   {
      int nYear=0, nMonth=0, nDay=0, nHour=0, nMin=0, nSec=0;
      int ReturnCode = sscanf(DateAsString.c_str(), "%04d%02d%02d", &nYear, &nMonth, &nDay);

      if( ReturnCode != 3 )
      {
         COL_ERROR_STREAM("Unable to get expiry date from " << DateAsString ,COLerror::SystemError);
      }
      Date.setDateTime( nYear, nMonth, nDay, nHour, nMin, nSec );
   }
}

// Pretty pointless function, now that it's a no-op.
bool RGNnoLicenseRequired()
{
   COL_FUNCTION(RGNnoLicenseRequired);
   return false;
}

int RGNdoesNotExpire()
{
   COL_FUNCTION(RGNdoesNotExpire);
   COLstring Date = getenv(CHMregExpiry);
   Date.stripAll(' ');

   if (Date != "NOEXPIRY")
   {
      return 0;
   }
   else
   {
      return 1;
   }
}

RGNlicenseLevel RGNregistrationLevel(const COLstring& iMachineId, const COLstring& RegKey, const COLstring& ExpiryDate, int DoesNotExpire)
{
   COL_FUNCTION(RGNregistrationLevel);
   COLstring MachineId(iMachineId);
   MachineId.stripAll(' ');

   COLstring MagicString = MAGIC_KEY;
   if (RegKey == MagicString)
   {
      return RGN_LICENSE_FULL;
   }

   COLstring Hash;

   if (1 != DoesNotExpire)
   {
      // check that now < ExpiryDate
      time_t tNow;
      time(&tNow);
      struct tm* tmNow;
      tm TimeStore = {0};
      tmNow = localtime_r(&tNow,&TimeStore);
      char bufNow[RGN_MACHINE_ID_DATE_LENGTH+16];
      strftime(bufNow, RGN_MACHINE_ID_DATE_LENGTH, "%Y%m%d", tmNow);
      long lExpiry = atoi(ExpiryDate.c_str());
      long Now = atoi(bufNow);
      if (Now > lExpiry)
      {
         return RGN_EXPIRED;
      }
   }

   Hash = RGNdemoKey(MachineId, ExpiryDate, DoesNotExpire);
   if (Hash.compare(RegKey) == 0)
   {
      return RGN_LICENSE_DEMO;  // demo license
   }

   Hash = RGNextensionKey(MachineId, ExpiryDate, DoesNotExpire);
   if (Hash.compare(RegKey) == 0)
   {
      return RGN_LICENSE_EXTENSION;  // extension license
   }

   Hash = RGNlicenseKey(MachineId, ExpiryDate, DoesNotExpire);
   if (Hash.compare(RegKey) == 0)
   {
      return RGN_LICENSE_FULL;  // fully licensed
   }
   return RGN_NOT_LICENSED;// not licensed
}

void RGNsetRegistrationCode(const char* Code, const char* ExpiryDate, int DoesNotExpire)
{
   COL_FUNCTION(RGNsetRegistrationCode);
}

void RGNcheckLicense()
{
   COL_FUNCTION(RGNcheckLicense);
   RGNlicenseLevel LicenseLevel = RGNregistrationLevel(
      RGNmachineId(), RGNregistrationCode(), RGNexpiryDate(), RGNdoesNotExpire()
   );

   if ((LicenseLevel == RGN_EXPIRED) || (LicenseLevel == RGN_NOT_LICENSED))
   {
      COLstring ErrorString;
      COLostream ErrorStream(ErrorString);
      ErrorStream << "This host machine does not have a valid Chameleon license."                        << newline
                  << "You will need to log in to your iNTERFACEWARE Members account at:"                 << newline  << newline
                  << "   http://www.interfaceware.com/"                                                  << newline  << newline
                  << "and use this Machine ID '" << RGNmachineId() << "' to generate a unique"           << newline
                  << "registration code for this machine.  Then read this part of our online manual: "   << newline  << newline
                  << "   http://www.interfaceware.com/manual/unix.html"                                  << newline  << newline
                  << "for detailed instructions on which environmental variables to store the registration code in." << newline;

      ErrorStream << "CHM_EXPIRY_DATE="   << RGNexpiryDate()         << newline
                  << "CHM_REGKEY="        << RGNregistrationCode()   << newline;

      throw COLerror(ErrorString, COLerror::PreCondition);
   }
}

bool RGNisLicensed()
{
   COL_FUNCTION(RGNisLicensed);
   RGNlicenseLevel LicenseLevel = RGNregistrationLevel(
      RGNmachineId(), RGNregistrationCode(), RGNexpiryDate(), RGNdoesNotExpire()
   );

   return ((LicenseLevel != RGN_EXPIRED) && (LicenseLevel != RGN_NOT_LICENSED));
}
