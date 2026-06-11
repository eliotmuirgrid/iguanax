//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: RGNhash
//
// Description:
//
// Implementation
//
// Author: Art Harrison
// Date:   Thursday, July 1st, 2010 @ 06:51:34 PM
//
//---------------------------------------------------------------------------
#include "RGNprecomp.h"
#pragma hdrstop

#include "RGNobfuscate.h"

#include "RGNhash.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <stdio.h>

COLstring RGNhash5(const COLstring& Padding, const COLstring& Value)
{
   // requirement: to produces a reasonably abstract hash of fixed width
   // given an input value string of any length and contents
   // so that the result is a nice random-looking scrambled value
   // that changes radically with any change in the input value - length, value, position
   // and is not likely to produce predictable or degenerate values
   // returned as 5 bytes in hex (%02x) = 10 hex digits
   // in simple, portable, self-sufficient code (no adding dependencies to RGN).
   // it is cryptographically imperfect as far as hashes go,
   // but it's not expected to be - just good enough to produce a machine id
   // from an arbitrary string.
   // particularly needed to produce a stable Machine ID from a hostname,
   // as a workaround for some customers on Posix
   // without eth0, with degenerate or flaking hostid and/or flaking MAC address,
   // which means unstable Machine ID
   // which means trouble for the customer, and licensing and support trouble for us

   COLstring PaddedValue = Padding + Value;

   const int BUFFER_SIZE = 5;
   unsigned int Buffer[BUFFER_SIZE] = {177, 137, 249, 97, 201};
   // pre-seeded values in the buffer with magic pseudo-random values,
   // magic values are obtained from:
   // map( lambda x: (x*x) % 256, [3001,3011,3019,3023,3037] )
   // i.e. primes starting above 3000, squared, mod 256

   // if we're short of characters, we wrap around the PaddedValue,
   // so that we have at least one pass around the Buffer.
   // also wrap to a multiple of BUFFER_SIZE
   int MaxCharIndex = PaddedValue.size();
   if(MaxCharIndex % BUFFER_SIZE)
   {
      MaxCharIndex += BUFFER_SIZE - (PaddedValue.size() % BUFFER_SIZE);
   }

   // Shift is a value we use to both rotate the input reading position and the char values
   // it's based on both length and total sum of the characters, each squared and all added up,
   unsigned int Shift = PaddedValue.size() * PaddedValue.size();
   {
      for(int CharIndex=0; CharIndex < PaddedValue.size(); CharIndex++)
      {
         unsigned int CharValue = (unsigned char) PaddedValue[CharIndex];
         Shift += (CharIndex + CharValue) * (CharIndex + CharValue);
      }
   }

   // finally, perform the calculation
   {
      for(int CharIndex = 0; CharIndex < MaxCharIndex; CharIndex++)
      {
         unsigned int CharCode = PaddedValue[(CharIndex+Shift) % PaddedValue.size()];
         int BufferIndex = CharIndex % BUFFER_SIZE;
         Buffer[BufferIndex] = (Buffer[BufferIndex] + CharCode + Shift) % 256;
      }
   }

   COLstring ResultingMachineId;
   {
      for(int ByteIndex=0; ByteIndex < BUFFER_SIZE; ByteIndex++)
      {
         char FormatBuffer[8];
         snprintf(FormatBuffer, 8, "%02X", Buffer[ByteIndex]);
         ResultingMachineId += FormatBuffer;
      }
   }

   return ResultingMachineId;
}

COLstring RGNmachineIdFromHostName(const COLstring& HostName)
{
   return "5_" + RGNhash5("hostname:", COLlowerCase(HostName));
}
