//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GZIPdecompressFilter
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   Friday, November 5th, 2010 @ 01:17:49 PM
//
//---------------------------------------------------------------------------
#include "GZIPprecomp.h"
#pragma hdrstop

#include "GZIPdecompressFilter.h"

#include <zlib.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#define GZIP_DECOMPRESS_CHUNK 16384

class GZIPdecompressFilterPrivate
{
public:
   GZIPdecompressFilterPrivate() : StreamState(-1){}
   int StreamState;
   z_stream Zstream;
   COLuint8 OutputBuffer[GZIP_DECOMPRESS_CHUNK];
};

GZIPdecompressFilter::GZIPdecompressFilter(COLsink* pNext) : COLfilter(pNext)
{
   pMember = new GZIPdecompressFilterPrivate;
   resetFilter();
}

GZIPdecompressFilter::~GZIPdecompressFilter()
{
   clearFilter();
   delete pMember;
}

bool GZIPdecompressFilter::doUnzip(bool Flush)
{
   /* run inflate() on input until output buffer not full, finish
      decompression if all of source has been read in */
   do
   {
      pMember->Zstream.avail_out = GZIP_DECOMPRESS_CHUNK;
      pMember->Zstream.next_out = pMember->OutputBuffer;

      int Ret = inflate(&pMember->Zstream, (Flush ? Z_FINISH : Z_NO_FLUSH)); /* no bad return value */
      COLPRECONDITION(Ret != Z_STREAM_ERROR);  /* state not clobbered */
      COLuint32 OutSize = GZIP_DECOMPRESS_CHUNK - pMember->Zstream.avail_out;

      COLuint32 NextWriteAmount = next().write( pMember->OutputBuffer, OutSize );

      if (NextWriteAmount < OutSize)
      {
         //bail, we can't continue
         clearFilter();
         return false;
      }
   } while (pMember->Zstream.avail_out != GZIP_DECOMPRESS_CHUNK);
   return true;
}


COLuint32 GZIPdecompressFilter::write( const void* cpBuffer, COLuint32 SizeOfBuffer )
{
   pMember->Zstream.avail_in = SizeOfBuffer;
   pMember->Zstream.next_in = (unsigned char*)cpBuffer;
   if (!doUnzip(false))
   {
      return 0;
   }
   return SizeOfBuffer;
}

void GZIPdecompressFilter::onEndStream()
{
   if (pMember->StreamState == Z_OK)
   {
      pMember->Zstream.avail_in = 0;
      pMember->Zstream.next_in = 0;//pMember->InputBuffer;
      doUnzip(true);
      clearFilter();
   }
}

void GZIPdecompressFilter::clearFilter()
{
   if (pMember->StreamState == Z_OK)
   {
      inflateEnd(&pMember->Zstream);
      /* allocate inflate state */
      pMember->StreamState = -1;
   }
   ::memset(&pMember->Zstream, 0, sizeof(pMember->Zstream));
}

void GZIPdecompressFilter::resetFilter()
{
   clearFilter();
   //note that inflateInit2 is for gzip format
   pMember->StreamState = inflateInit2(&pMember->Zstream, MAX_WBITS + 16);
   COLPRECONDITION(pMember->StreamState == Z_OK);
}
