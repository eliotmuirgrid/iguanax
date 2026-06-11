#ifndef __GZIP_COMPRESS_FILTER_H__
#define __GZIP_COMPRESS_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GZIPcompressFilter
//
// Description:
//
// GZIPcompressFilter class.
//
// Author: Nasron Cheong
// Date:   Tuesday, February 2nd, 2010 @ 03:18:05 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLfilter.h>

class GZIPcompressFilterPrivate;

class GZIPcompressFilter : public COLfilter
{
public:
   GZIPcompressFilter(COLsink* pNext);
   // This is non virtual because we are assuming no classes will inherit from this class
   ~GZIPcompressFilter();

   virtual COLuint32 write( const void* cpBuffer, COLuint32 SizeOfBuffer );
   virtual void onEndStream();

   virtual void resetFilter();

private:
   void clearFilter();
   bool doZip(bool Flush);

   GZIPcompressFilterPrivate* pMember;
   GZIPcompressFilter(const GZIPcompressFilter& Orig); // not allowed
   GZIPcompressFilter& operator=(const GZIPcompressFilter& Orig); // not allowed
};

#endif // end of defensive include
