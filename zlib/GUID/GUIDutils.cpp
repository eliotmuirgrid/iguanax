//---------------------------------------------------------------------------
// Copyright (C) 1997-2017 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GUIDutils
//
// Description:
//
// Implementation
//
// Author:  Akshay Ganeshen
// Date:    Tue Nov 17 10:55:33 EST 2015
//---------------------------------------------------------------------------

#include "GUIDutils.h"

#ifndef _WIN32
#include <sys/time.h>
#endif

#include <COL/COLerror.h>
#include <COL/COLstring.h>
#include <COL/COLwindows.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <SFI/SFIrandom.h>
#include <SFI/SFImd5.h>

COLstring GUIDentropyString() {
   COL_FUNCTION(GUIDentropyString);
   COLstring EntropyTime, EntropyRng;
#ifdef _WIN32
   int TempMs = 0;
   SYSTEMTIME Time = {0};
   ::GetLocalTime(&Time);
   EntropyTime.write(&Time,sizeof(Time));
   EntropyTime.write(&Time.wYear,sizeof(Time.wYear));
   EntropyTime.write(&Time.wMonth,sizeof(Time.wMonth));
   EntropyTime.write(&Time.wDay,sizeof(Time.wDay));
   EntropyTime.write(&Time.wHour,sizeof(Time.wHour));
   EntropyTime.write(&Time.wMinute,sizeof(Time.wMinute));
   EntropyTime.write(&Time.wSecond,sizeof(Time.wSecond));
   EntropyTime.write(&Time.wMilliseconds,sizeof(Time.wMilliseconds));
#else
   timeval Time = {0};
   ::gettimeofday(&Time,0); //no errors
   EntropyTime.write(&Time.tv_sec,sizeof(Time.tv_sec));
   EntropyTime.write(&Time.tv_usec,sizeof(Time.tv_usec));
#endif
   char RngResult[16];
   SFIrandom Rng;
   Rng.generate(&RngResult,sizeof(RngResult));
   EntropyRng.write(&RngResult,sizeof(RngResult));

   COLstring EntropyHash = SFImd5Raw(EntropyTime + EntropyRng);
   COL_VAR3(EntropyTime, EntropyRng, EntropyHash);
   return EntropyHash;
}

char GUIDbase62ValueToCharacter(char Value) {
   COL_FUNCTION(GUIDencodeBase62Byte);
   COLASSERT(Value >= 0 && Value < 62);
   if (Value < 26) {
      return 'a' + Value;
   }
   if (Value < 52) {
      return 'A' + Value - 26;
   }
   return '0' + Value - 52;
}

COLstring GUIDencodeBase62(const COLstring& RawBytes) {
   COL_FUNCTION(GUIDencodeBase62);

   COLstring Encoded;
   // Using an int to attempt to avoid overflow issues
   // Even if it does, no big deal
   int LeftoverBuffer = 0;
   for (int i = 0; i < RawBytes.size(); i++) {
      // Carry over the remainder
      LeftoverBuffer += LeftoverBuffer * 64;
      // Ugh... unsigned vs signed issue here
      char CurrentByte = RawBytes[i];
      LeftoverBuffer += CurrentByte > 0 ? CurrentByte : (1 << 8) + CurrentByte;
      char Remainder = LeftoverBuffer % 62;
      COL_VAR2(LeftoverBuffer, (int)Remainder);
      Encoded.append(1, GUIDbase62ValueToCharacter(Remainder % 62));
      LeftoverBuffer = LeftoverBuffer / 62;
   }
   // Don't use the crud at the end of the leftover buffer
   return Encoded;
}
