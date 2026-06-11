//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: MICmachineId
//
// Description:
//
// Implementation
//
// Author: Art Harrison
// Date:   Tuesday, June 15th, 2010 @ 04:26:22 PM
//
//---------------------------------------------------------------------------
#include "Precomp.h"
#pragma hdrstop

#include "MICmachineId.h"

#include <RGN/RGNobfuscate.h>
#include <RGN/RGNhash.h>

#ifdef __linux__
#include <unistd.h>
#include <PIP/PIPexecuteCommand.h>
#endif

#ifdef _WIN32
#include <REG/REGregistryWindows.h>
#include <RGN/RGNmachineId.h>
#endif

#include <IP/IPutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifdef _WIN32

extern const char* CHMmachineId;

extern COLstring CHMmachineIdPath();

extern COLstring RGNscrambleId(const COLstring& Id);

extern COLstring RGNunscrambleId(const COLstring& Id);

extern COLstring RGNmacAddress();

extern const char* CHMnoNetworkCardMachineId;

extern const COLint16 RGN_MACHINE_ID_DATE_LENGTH;

static const char* pAdminNeededError = "Could not read/write registry entry. Please restart the application as a user with Administrator access to initialize the registry.";

COLstring MICcomputeMachineIdVerbose(COLostream& Log)
{
   COLstring Result = RGNmacAddress();
   //COLstring Result = RGNhardDriveSerialNumber();

   COLstring FinalCode;

   Log << "Got RMA: " << Result << newline;
   if (!Result.is_null())
   {
      for (COLindex CharIndex = Result.size() - 1;
         CharIndex > 0;
         CharIndex--)
      {
         if ('0' != Result[CharIndex])
         {
            char Character = Result[CharIndex];
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
      Log << "Got FC from RMA: " << FinalCode << newline;
   }
   else
   {
      Log << "RMA is null." << newline;
   }

   if (FinalCode.size() == 0)
   {
      Log << "FC is null - using NNC" << newline;
      FinalCode = CHMnoNetworkCardMachineId;
   }
   if (FinalCode.size() > RGN_MACHINE_ID_MAC_ADDRESS_LENGTH)
   {

      FinalCode = FinalCode.substr(0, RGN_MACHINE_ID_MAC_ADDRESS_LENGTH);
      Log << "Truncating down to MA length: FC=" << FinalCode << newline;
   }
   return FinalCode;
}

COLstring MICmachineId(COLostream& Log, bool Read, bool Write, bool UseHash5)
{
   if(!Read)
   {
      Log << "Computing Machine ID, skipping caching" << newline;
   }
   else
   {
      // first try and get it from the registry
      Log << "Will try to read Machine ID: " << CHMmachineIdPath() << " name=" << CHMmachineId << newline;
      try
      {
         REGregistry Key(HKEY_LOCAL_MACHINE);
         Key.openKey(CHMmachineIdPath(), true);
         COLstring Id = Key.readString(CHMmachineId, "NOID");
         if (Id != "NOID")
         {
            Log << "Found raw Id=" << Id << newline;
            Id = RGNunscrambleId(Id);
            Log << "Returning Id=" << Id << newline;
            return Id;
         }
         Log << "Id not found - got " << Id << newline;
      }
      catch (const COLerror& Error)
      {
         Log << "Failed to check for Id - got error:" << newline << Error << newline;
         Log << "Will throw: " << pAdminNeededError << newline;
         COL_ERROR_STREAM(pAdminNeededError,COLerror::SystemError);
      }

      Log << "Proceeding past Id checking" << newline;
   }

   COLstring FinalCode;
   if(UseHash5)
   {
      Log << "Will compute MachineID using Hash5 method" << newline;
      COLstring HostName = COLlowerCase(IPlocalHostName());
      Log << "Using HostName: " << HostName << newline;
      FinalCode = RGNmachineIdFromHostName(HostName);
   }
   else
   {
      Log << "Will compute MachineID using standard method" << newline;
      FinalCode = MICcomputeMachineIdVerbose(Log);
   }

   Log << "Got FC=" << FinalCode << newline;

   if(!Write)
   {
      Log << "Not storing computed Machine ID" << newline;
   }
   else
   {
      Log << "Will try to cache FC" << newline;
      try
      {
         REGregistry Key2(HKEY_LOCAL_MACHINE);
         Log << "Opening: " << CHMmachineIdPath() << newline;
         Key2.openKey(CHMmachineIdPath(), true);
         Log << "Writing: " << CHMmachineId << " = " << FinalCode << newline;
         Key2.writeString(CHMmachineId, RGNscrambleId(FinalCode));
         Log << "Written." << newline;
      }
      catch(const COLerror& Error)
      {
         Log << "Failed to write with error:" << newline << Error << newline;
         Log << "Will throw: " << pAdminNeededError << newline;
         COL_ERROR_STREAM(pAdminNeededError,COLerror::SystemError);
      }
   }
   Log << "Returning FC=" << FinalCode << newline;
   return FinalCode;
}

#endif // _WIN32

#ifdef __linux__

void MICexecuteCommandSafe(const COLstring& Command, COLostream& Stream)
{
   try
   {
      PIPexecuteCommand(".", Command, );
   }
   catch(const COLerror& Error)
   {
      Stream << "ERROR: failed to execute command " << Stream << ":" << newline;
      Stream << Error;
   }
}

void MICdumpPosixInfo(COLostream& Stream)
{
   Stream << "int size: " << sizeof(int) << newline;
   Stream << "long size: " << sizeof(long) << newline;
   Stream << newline;
   try
   {
      COLstring IfconfigCommand = "ifconfig -a";
      if(FILfileExists("/sbin/ifconfig"))
      {
         IfconfigCommand = "/sbin/ifconfig -a";
      }
      Stream << "ifconfig -a:" << newline;
      COLcout << newline;
      MICexecuteCommandSafe(IfconfigCommand);
      Stream << newline;
   }
   catch(const COLerror& Error)
   {
      Stream << "unexpected error while trying to run ifconfig:" << newline;
      Stream << Error << newline;
   }
   unsigned HostId = gethostid();
   COLcout << "HostId: " << hex << HostId << newline;
   char Buffer[100];
   snprintf(Buffer, 100, "%d : %lx", sizeof(gethostid()), gethostid());
   Stream << Buffer << newline;
   Stream << newline;
   Stream << "uname -a:" << newline;
   Stream << newline;
   MICexecuteCommandSafe("uname -a");
   Stream << newline;
   COLcout << "RMA: " << MICstringToHex(RGNmacAddress()) << newline;
   COLcout << newline;
   COLcout << "cpuinfo:" << newline;
   COLcout << newline;
   MICexecuteCommandSafe("cat /proc/cpuinfo");
   COLcout << newline;
   COLcout << "lshw:" << newline;
   COLcout << newline;
   MICexecuteCommandSafe("lshw");
   COLcout << newline;
};


COLstring MICmachineId(COLostream& Log, bool Compute, bool Write)
{
   // Read and Write options are meaningless on Posix, as there is no caching

   COLstring Result = RGNmacAddress();
   Log << "Found RMA: " << Result << newline;

   if (Result.is_null())
   {
      Log << "RMA is null - using legacy" << newline;
      Log << "HID=" << hex << getshotid() << newline;
      COLstring LegacyMachineId = RGNlegacyMachineId();
      Log << "LMI=" << LegacyMachineId << newline;
      return LegacyMachineId;
   }

   COLstring FinalCode;
   for (COLindex CharIndex = Result.size() - 1;
      CharIndex > 0;
      CharIndex--)
   {
      if ('0' != Result[CharIndex])
      {
         char Character = Result[CharIndex];
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

   Log << "Computed FC=" << FinalCode << newline;

   if (FinalCode.size() == 0)
   {
      FinalCode = "SD43FHGGWFKF";
      Log << "FC was empty - using " << FinalCode << newline;
   }
   if (FinalCode.size() > RGN_MACHINE_ID_MAC_ADDRESS_LENGTH)
   {
      FinalCode = FinalCode.substr(0, RGN_MACHINE_ID_MAC_ADDRESS_LENGTH);
      Log << "FC was too long - truncated to " << FinalCode << newline;
   }

   Log << "Returning FC=" << FinalCode << newline;
   return FinalCode;
}

#endif
