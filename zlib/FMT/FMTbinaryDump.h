#ifndef __FMT_BINARY_DUMP_H__
#define __FMT_BINARY_DUMP_H__
//------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  FMTbinaryDump
//
// Description
//
// TODO - update description
//
// Simple class which takes the buffer pointed to and outputs a
// binary dump of the message received.  The implementation is not
// terribly efficient - it's just intended to give a good quick and
// easy diagnostic printout.
//
// Author: Eliot Muir
// Date:   Thu 04/27/2000 
//---------------------------------------------------------------------------
#include <COL/COLostream.h>

class COLostream;
class COLsink;

//same as below, but assumes that the buffer is at least 16 bytes long or less
//and does not put in the newline at the end

// TODO - at least 16 bytes long or less - isn't that a little bit of a contradiction?
void FMTbinaryDump16(
   COLsink& Sink, 
   const COLuint8* pBuffer,
   COLuint32 Length
);

//a over-loaded method of FMTbinaryDump, got rid of the indirection and use COLsink instead,
#ifdef _WIN32
COL_DLL void FMTbinaryDump(
   COLsink&        Sink, 
   const COLuint8* pBuffer, 
   COLindex        Length,
   const char* newline = "\r\n"
);
#else
void FMTbinaryDump(
   COLsink&        Sink, 
   const COLuint8* pBuffer, 
   COLindex        Length,
   const char* newline = "\n"
);
#endif

void FMTtoHex(COLsink& Sink, COLuint8 Character);

//indicates if FMTfromHex will succeed or fail
bool FMTfromHexIsValid(char UpperHex, char LowerHex);

COLuint8 FMTfromHex(char UpperHex, char LowerHex);

//old operators for backward compatibility
void FMTtoHex(COLostream& Stream, COLuint8 Character);

void FMTbinaryDump
(
   COLostream& Stream,
   const COLuint8* pBuffer,
   COLindex Length
 
);

#ifdef _WIN32
COLostreamManip* FMTstreamHex(const void* pBuffer, COLindex Length, const char* Newline = "\r\n");
#else
COLostreamManip* FMTstreamHex(const void* pBuffer, COLindex Length, const char* Newline = "\n");
#endif

#endif // end of defensive include
