//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: STMZIPfilterDecompress
//
// Description:
//
// Implementation
//
// Author: Ryan Dewsbury
//
//---------------------------------------------------------------------------
#include "STMZIPprecomp.h"
#pragma hdrstop

#include "STMZIPfilterDecompress.h"
#include <BZP/bzlib.h>
#include <COL/COLerror.h>

/* Block Size
   Parameter blockSize100k specifies the block size to be used for compression. 
   It should be a value between 1 and 9 inclusive, and the actual block size used 
   is 100000 x this figure. 9 gives the best compression but takes most memory. 
*/
#define BZIP_BLOCKSIZE 9

/* Verbosity
   Parameter verbosity should be set to a number between 0 and 4 inclusive. 
   0 is silent, and greater numbers give increasingly verbose monitoring/debugging 
   output. If the library has been compiled with -DBZ_NO_STDIO, no such output will 
   appear for any verbosity setting. 
*/
#define BZIP_VERBOSITY 0

/* Work Factor
   Parameter workFactor controls how the compression phase behaves when presented 
   with worst case, highly repetitive, input data. If compression runs into 
   difficulties caused by repetitive data, the library switches from the standard 
   sorting algorithm to a fallback algorithm. The fallback is slower than the 
   standard algorithm by perhaps a factor of three, but always behaves reasonably, 
   no matter how bad the input. 

   Lower values of workFactor reduce the amount of effort the standard algorithm will 
   expend before resorting to the fallback. You should set this parameter carefully; 
   too low, and many inputs will be handled by the fallback algorithm and so compress 
   rather slowly, too high, and your average-to-worst case compression times can 
   become very large. The default value of 30 gives reasonable behaviour over a wide 
   range of circumstances. 

   Allowable values range from 0 to 250 inclusive. 0 is a special case, equivalent to 
   using the default value of 30. 
*/
#define BZIP_WORKFACTOR 0

/* Small
   If small is nonzero, the library will use an alternative decompression algorithm 
   which uses less memory but at the cost of decompressing more slowly (roughly 
   speaking, half the speed, but the maximum memory requirement drops to around 2300k).
*/
#define BZIP_SMALL 0

struct STMZIPfilterDecompressPrivate
{
   char OutBuffer[ 0x8000 ];
   bz_stream BzipDecompressStream;
};

STMZIPfilterDecompress::STMZIPfilterDecompress( COLsink* pNext_ ):COLfilter( pNext_ )
{
   pMember = new STMZIPfilterDecompressPrivate;
   ::memset( &pMember->BzipDecompressStream, 0, sizeof( bz_stream) );
}

STMZIPfilterDecompress::~STMZIPfilterDecompress()
{
   if( pMember->BzipDecompressStream.next_in != 0 )
   {
      BZ2_bzDecompressEnd( &pMember->BzipDecompressStream );
   }
   delete pMember;
}


COLuint32 STMZIPfilterDecompress::write( const void* pBuffer, COLuint32 SizeOfBuffer )
{
   COLuint32 TotalBefore = pMember->BzipDecompressStream.total_out_lo32;
   if( pMember->BzipDecompressStream.next_in == 0 )
   {
      pMember->BzipDecompressStream.avail_in = sizeof( pMember->OutBuffer );
      if( BZ2_bzDecompressInit( &pMember->BzipDecompressStream, BZIP_VERBOSITY, BZIP_SMALL ) != BZ_OK )
      {
         throw COLerror( "bzip error", COLerror::PreCondition );
      }
   }
   pMember->BzipDecompressStream.next_in = (char*)pBuffer;
   pMember->BzipDecompressStream.avail_in = SizeOfBuffer;
   while( pMember->BzipDecompressStream.avail_in > 0 )
   {
      COLuint32 TotalBefore = pMember->BzipDecompressStream.total_out_lo32;
      pMember->BzipDecompressStream.next_out = pMember->OutBuffer;
      pMember->BzipDecompressStream.avail_out = sizeof( pMember->OutBuffer );
      if( BZ2_bzDecompress( &pMember->BzipDecompressStream ) < 0 )
      {
         throw COLerror( "bzip error", COLerror::PreCondition );
      }
      next().write( pMember->OutBuffer, pMember->BzipDecompressStream.total_out_lo32 - TotalBefore);
   }

   return SizeOfBuffer;
}

void STMZIPfilterDecompress::flush()
{
   next().flush();
}
