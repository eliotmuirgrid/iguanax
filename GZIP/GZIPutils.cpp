//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GZIPutils
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sun 28 Dec 2014 22:13:52 EST
//---------------------------------------------------------------------------
#include "GZIPprecomp.h"
#pragma hdrstop

#include "GZIPcompressFilter.h"
#include "GZIPdecompressFilter.h"
#include "GZIPutils.h"

#include <zlib.h>

#include <FIL/FILutils.h>
#include <FIL/FILfile.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Level = 9 is the best and slowest.  Level = Z_DEFAULT_STRATEGY
// Eliot - I don't think the deflateBound routine always works for every situation.  I have some hacks
// below to allocate a little more space than needed.  Suspect I will need to revisit this code later.
void GZIPcompress(const COLstring& Input, COLstring* pOutput, int Level){
   COL_FUNCTION(GZIPcompress);
   COL_VAR3(Input.size(), Input.capacity(), pOutput->capacity());
   int StreamState;
   z_stream Zstream;
   ::memset(&Zstream, 0, sizeof(Zstream));
   StreamState = deflateInit2(&Zstream, Level, Z_DEFLATED, MAX_WBITS + 16, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
   int SizeRequired = deflateBound(&Zstream, Input.size())+10;  // For a single pass we know what size we will need with this function. +10 is the ignorance factor (ugh)
   SizeRequired = COL_MAX(24, SizeRequired);
   COL_VAR(SizeRequired);
   pOutput->setCapacity(SizeRequired);
   COL_TRC("Called init.");
   COLASSERT(StreamState == Z_OK);
   Zstream.avail_in = Input.size();
   Zstream.next_in = (unsigned char*)Input.c_str();
   Zstream.avail_out = pOutput->capacity()-1;
   Zstream.next_out = (unsigned char*)pOutput->c_str();
   COL_TRC("Call deflate");
   int Ret = deflate(&Zstream, Z_FINISH);
   COL_TRC("Done deflate");
   COL_VAR(Ret);
   COLASSERT(Z_STREAM_END == Ret);
   pOutput->setSize(pOutput->capacity() - Zstream.avail_out-1);
   pOutput->setCapacity(pOutput->capacity() - Zstream.avail_out);
   COL_VAR(pOutput->size());
   deflateEnd(&Zstream); //TODO: check for error codes.
}

void GZIPuncompress(const COLstring& Input, COLstring* pOutput){
   COL_FUNCTION(GZIPuncompress);
   COLsinkString Sink(pOutput, false);
   GZIPdecompressFilter Filter(&Sink);
   Filter.write(Input.c_str(), Input.size());
   Filter.onEndStream();
}

void GZIPwriteFromFileToSink(FILfile* pInFile, COLsink* pSink, int BufferSize){
   COL_FUNCTION(GZIPwriteFromFileToSink);
   COLstring Buffer;
   Buffer.setCapacity(BufferSize);
   int AmountRead =0;
   do{
      AmountRead = pInFile->read(&Buffer);
      if (AmountRead > 0){
         pSink->write(Buffer.c_str(), AmountRead);
      }
   } while (AmountRead > 0);
   pSink->onEndStream();
}

void GZIPcompressFile(const COLstring& InputFile, const COLstring& OutputFile){
   COL_FUNCTION(GZIPcompressFile);
   COL_VAR2(InputFile, OutputFile);
   FILfile InFile(InputFile, FILfile::Read);
   FILfile OutFile(OutputFile, FILfile::Rewrite);
   GZIPcompressFilter Filter(&OutFile);
   GZIPwriteFromFileToSink(&InFile, &Filter, 50000);
}

void GZIPuncompressFile(const COLstring& InputFile, const COLstring& OutputFile){
   COL_FUNCTION(GZIPuncompressFile);
   COL_VAR2(InputFile, OutputFile);
   FILfile InFile(InputFile, FILfile::Read);
   FILfile OutFile(OutputFile, FILfile::Rewrite);
   GZIPdecompressFilter Filter(&OutFile);
   GZIPwriteFromFileToSink(&InFile, &Filter, 50000);
}

void GZIPcompressFileFast(const COLstring& InputFile, const COLstring& OutputFile){
   COL_FUNCTION(GZIPcompressFileFast);
   COL_VAR2(InputFile, OutputFile);
   COLstring Dest;
   COL_TRC("Reading file into string");
   FILreadFile(InputFile, &Dest);
   COLstring CompressedData;
   COL_TRC("Compressing...");
   GZIPcompress(Dest, &CompressedData, 9);
   COL_TRC("Writing compressed datastring to file");
   FILwriteFile(OutputFile, CompressedData);
}
