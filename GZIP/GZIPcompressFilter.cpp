//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GZIPcompressFilter
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   Tuesday, February 2nd, 2010 @ 03:18:05 PM
//
//---------------------------------------------------------------------------
#include "GZIPprecomp.h"
#pragma hdrstop

#include "GZIPcompressFilter.h"

#include <zlib.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#define GZIP_CHUNK 16384

class GZIPcompressFilterPrivate
{
public:
   GZIPcompressFilterPrivate() : StreamState(-1){}
   int StreamState;
   z_stream Zstream;
   COLuint8 OutputBuffer[GZIP_CHUNK];
};

GZIPcompressFilter::GZIPcompressFilter(COLsink* pNext) : COLfilter(pNext)
{
   pMember = new GZIPcompressFilterPrivate;
   resetFilter();
}

GZIPcompressFilter::~GZIPcompressFilter()
{
   clearFilter();
   delete pMember;
}

bool GZIPcompressFilter::doZip(bool Flush)
{
   /* run deflate() on input until output buffer not full, finish
      compression if all of source has been read in */
   do
   {
      pMember->Zstream.avail_out = GZIP_CHUNK;
      pMember->Zstream.next_out = pMember->OutputBuffer;

      int Ret = deflate(&pMember->Zstream, (Flush ? Z_FINISH : Z_NO_FLUSH)); /* no bad return value */
      COLPRECONDITION(Ret != Z_STREAM_ERROR);  /* state not clobbered */
      COLuint32 OutSize = GZIP_CHUNK - pMember->Zstream.avail_out;

      COLuint32 NextWriteAmount = next().write( pMember->OutputBuffer, OutSize );

      if (NextWriteAmount < OutSize)
      {
         //bail, we can't continue
         clearFilter();
         return false;
      }
   } while (pMember->Zstream.avail_out != GZIP_CHUNK);
   return true;
}


COLuint32 GZIPcompressFilter::write( const void* cpBuffer, COLuint32 SizeOfBuffer )
{
   pMember->Zstream.avail_in = SizeOfBuffer;
   pMember->Zstream.next_in = (unsigned char*)cpBuffer;
   if (!doZip(false))
   {
      return 0;
   }
   return SizeOfBuffer;
}

void GZIPcompressFilter::onEndStream()
{
   if (pMember->StreamState == Z_OK)
   {
      pMember->Zstream.avail_in = 0;
      pMember->Zstream.next_in = 0;//pMember->InputBuffer;
      doZip(true);
      clearFilter();
   }
}

void GZIPcompressFilter::clearFilter()
{
   if (pMember->StreamState == Z_OK)
   {
      deflateEnd(&pMember->Zstream);
      /* allocate deflate state */
      pMember->StreamState = -1;
   }
   ::memset(&pMember->Zstream, 0, sizeof(pMember->Zstream));
}

void GZIPcompressFilter::resetFilter()
{
   clearFilter();
   //note that deflateInit2 is for gzip format
   pMember->StreamState = deflateInit2(&pMember->Zstream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS + 16, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
   COLPRECONDITION(pMember->StreamState == Z_OK);
}
