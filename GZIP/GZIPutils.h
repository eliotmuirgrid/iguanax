#ifndef __GZIP_UTILS_H__
#define __GZIP_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GZIPutils
//
// Description:
//
// GZIPutils - some new wrappers for zlib which don't have the COLfilter overhead/baggage etc.
//
// Author: Eliot Muir
// Date:   Sun 28 Dec 2014 22:12:08 EST
//---------------------------------------------------------------------------

// Level = 9 is the best and slowest. -1 is the default.
void GZIPcompress(const COLstring& Input, COLstring* pOutput, int Level=-1);

// This routine will throw if there is a format error.
void GZIPuncompress(const COLstring& Input, COLstring* pOutput);

// Both these work on single thread so neither is that 'fast'
// Number 1 using slow streaming - default 1024 byte per write.
void GZIPcompressFile(const COLstring& InputFile, const COLstring& OutputFile);

// Number 2 inhales all the data into memory and so it's 33% faster, also uses
// higher compression - so about 10% smaller.
void GZIPcompressFileFast(const COLstring& InputFile, const COLstring& OutputFile);

// reverse of compressFile
void GZIPuncompressFile(const COLstring& InputFile, const COLstring& OutputFile);

#endif // end of defensive include
