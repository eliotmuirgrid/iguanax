// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBepochTime
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sunday 02 April 2023 - 09:47AM
// ---------------------------------------------------------------------------

#ifndef _WIN32
#include <sys/time.h>
#else
#include <windows.h>
#endif

#include <SDB/SDBepochTime.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifdef _WIN32
static inline void FileTimeToUnixTime(COLuint64* result, const FILETIME* in) {
   const COLuint64 DELTA_EPOCH_IN_USEC = 11644473600000000 /*LL*/;
   COLuint64 v = in->dwHighDateTime;
   v = v << 32;
   v |= in->dwLowDateTime;
   v /= 10;   // convert from 100 nanosecond periods to microseconds
   v -= DELTA_EPOCH_IN_USEC;  // convert to Unix epoch
  *result = v;
}
#endif

COLuint64 SDBtime() {
   COLuint64 now = 0;
#ifdef _WIN32
   FILETIME ftime;
   GetSystemTimeAsFileTime(&ftime);
   FileTimeToUnixTime(&now, &ftime);
   now /= 1000;
#else // POSIX
   struct timeval tv;
   ::gettimeofday(&tv, 0);
   now = tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
   return now;
}

void SDBformatHexByte(int Byte, COLstring* pOut){
   char Char = (Byte & 0xF0) >> 4;
   Char += (Char >= 10) ? ('A' - 10) : '0';
   *pOut += Char;
   Char = Byte & 0xF;
   Char += (Char >= 10) ? ('A' - 10) : '0';
   *pOut += Char;
}

COLstring SDBepochTimePath(time_t Time){
   COL_FUNCTION(SDBepochTimePath);
   COLstring Out;
   //COLostream Stream(Out);
   //Stream << hex << Time;
   int ByteArray[8] = {0,0,0,0, 0,0,0,0};
   int i = 0;
   while (Time != 0){
     int Byte = Time & 0xFF;
     ByteArray[i] = Byte;
     i++;
     Time = Time >> 8;
   }
   SDBformatHexByte(ByteArray[7], &Out);
   SDBformatHexByte(ByteArray[6], &Out);
   SDBformatHexByte(ByteArray[5], &Out);
   SDBformatHexByte(ByteArray[4], &Out);
   Out += FIL_DIR_SEPARATOR;
   SDBformatHexByte(ByteArray[3], &Out);
   Out += FIL_DIR_SEPARATOR;
   SDBformatHexByte(ByteArray[2], &Out);
   Out += FIL_DIR_SEPARATOR;
   SDBformatHexByte(ByteArray[1], &Out);
   SDBformatHexByte(ByteArray[0], &Out);
   Out += ".log";

   return Out;
}

time_t SDBepochTime(const COLstring& EpochTimePath) {
   COL_FUNCTION(SDBepochTime);
   COLASSERT(FILpathExtLast(EpochTimePath, true) == ".log");
   COLstring Clean = EpochTimePath.substr(0, EpochTimePath.size()-4);
   Clean.stripAll(FIL_DIR_SEPARATOR[0]);
   COL_VAR(Clean);
   return strtoll(Clean.data(), NULL, 16);
}

bool SDBepochRollover(time_t Current, time_t Base){
   COL_FUNCTION(SDBepochRollover);
   time_t T1 = Base    & 0xFFFFFFFFFFFF0000;
   time_t T2 = Current & 0xFFFFFFFFFFFF0000;
   return T1 != T2;
}
