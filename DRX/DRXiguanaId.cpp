//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXiguanaId
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Wednesday, October 22nd, 2008 @ 02:59:29 PM
//
//---------------------------------------------------------------------------

#include "DRXiguanaId.h"
#include "DRXfilterEncrypt.h"

// #include <COL/COLsimpleBuffer.h>
#include <FIL/FILutils.h>

#include <RGN/RGNmachineId.h>

#include <SFI/SFIhexEncodeFilter.h>
#include <SFI/SFImd5.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

const char* DRX_IGUANA_ID_CHARACTER_SET = "23456789ABCDEFGHJKLMNPQRSTUVWXYZ";

// No default directory for posix.  Oh well.
#define DRX_DEFAULT_WORKING_DIRECTORY "C:\\PROGRAM FILES\\INTERFACEWARE\\IGUANA\\"

COLstring DRXiguanaId() {
   COL_FUNCTION(DRXiguanaId);
   // Eliot note to maintainers of the this code least they make the same mistake I did and forget
   // that by using FILcurrentWorkingDir() one is in effect normalizing the working directory.  I changed
   // the code to pass through the configuration directory as an argument and ran into the difficulty that d:\build\DBD
   // is different from D:\build\DBD

   COLstring WorkingDirectory = FILworkingDir();
   return DRXiguanaIdImplementation(RGNmachineId(), WorkingDirectory);
}

COLuint32 DRXparseHexDigit(const char Character) {
   if (Character >= '0' && Character <= '9') {
      return Character - '0';
   } else if (Character >= 'A' && Character <= 'F') {
      return 10 + (Character - 'A');
   } else {
      COL_ERROR_STREAM("Non-hex character encountered: '" << Character << '\'', COLerror::PreCondition);
   }
}

COLuint32 DRXparseTwoDigitHexNumber(const char FirstCharacter, const char SecondCharacter) {
   return DRXparseHexDigit(FirstCharacter)*16 + DRXparseHexDigit(SecondCharacter);
}

// TODO - looks like this is HEX de-encoding code - it could probably be replaced with SFI library code?
COLstring DRXconvertFromHexToIguanaId(const COLstring& HexString) {
   COL_FUNCTION(DRXconvertFromHexToIguanaId);
   COL_VAR(HexString);
   COLstring IguanaId(HexString.size()/2, ' ');
   COLuint32 CurrentIguanaIdChar;
   const COLuint32 CountOfPossibleIguanaIdCharacters = strlen(DRX_IGUANA_ID_CHARACTER_SET);

   for (COLuint32 CurrentIguanaIdCharIndex = 0; CurrentIguanaIdCharIndex < IguanaId.size(); ++CurrentIguanaIdCharIndex) {
      CurrentIguanaIdChar = DRXparseTwoDigitHexNumber(HexString[CurrentIguanaIdCharIndex*2], HexString[CurrentIguanaIdCharIndex*2+1]) % CountOfPossibleIguanaIdCharacters;
      IguanaId[CurrentIguanaIdCharIndex] = DRX_IGUANA_ID_CHARACTER_SET[CurrentIguanaIdChar];
   }
   COL_VAR(IguanaId);
   return IguanaId;
}

COLstring DRXiguanaIdImplementation(const COLstring& MachineId, const COLstring& WorkingDirectoryRaw) {
   COL_FUNCTION(DRXiguanaIdImplementation);
   COL_VAR2(MachineId, WorkingDirectoryRaw);
   // Eliot note to maintainers of the this code least they make the same mistake I did and forget
   // that by using FILcurrentWorkingDir() one is in effect normalizing the working directory.  I changed
   // the code to pass through the configuration directory as an argument and ran into the difficulty that d:\build\DBD
   // is different from D:\build\DBD
   COLstring WorkingDirectory(WorkingDirectoryRaw);
   FILcorrectPathSeparators(WorkingDirectory);
#ifdef _WIN32  // mea coupa - see #10826
   WorkingDirectory.toUpperCase();
   if (WorkingDirectory == DRX_DEFAULT_WORKING_DIRECTORY) {
      return MachineId;
   }
#endif

   COLstring UnencryptedIguanaId = MachineId + WorkingDirectory;
   COLstring HexString = SFImd5(UnencryptedIguanaId);
   // TODO - huh - we get the MD5 hash then unencrypt it???
   return DRXconvertFromHexToIguanaId(HexString);
}
