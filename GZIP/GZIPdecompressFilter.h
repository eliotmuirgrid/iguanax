#ifndef __GZIP_DECOMPRESS_FILTER_H__
#define __GZIP_DECOMPRESS_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GZIPdecompressFilter
//
// Description:
//
// GZIPdecompressFilter class.
//
// Author: Nasron Cheong
// Date:   Friday, November 5th, 2010 @ 01:17:49 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLfilter.h>

class GZIPdecompressFilterPrivate;

class GZIPdecompressFilter : public COLfilter
{
public:
   GZIPdecompressFilter(COLsink* pNext);
   // This is non virtual because we are assuming no classes will inherit from this class
   ~GZIPdecompressFilter();

   virtual COLuint32 write( const void* cpBuffer, COLuint32 SizeOfBuffer );
   virtual void onEndStream();
   
   virtual void resetFilter();
   
private:
   void clearFilter();
   bool doUnzip(bool Flush);
   GZIPdecompressFilterPrivate* pMember;
   GZIPdecompressFilter(const GZIPdecompressFilter& Orig); // not allowed
   GZIPdecompressFilter& operator=(const GZIPdecompressFilter& Orig); // not allowed
};

#endif // end of defensive include
