#ifndef __STMZIP_FILTER_DECOMPRESS_H_
#define __STMZIP_FILTER_DECOMPRESS_H_
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: STMZIPfilterDecompress
//
// Description:
//
// Author: Ryan Dewsbury
//
//---------------------------------------------------------------------------

#include <COL/COLfilter.h>

struct STMZIPfilterDecompressPrivate;

class STMZIPfilterDecompress : public COLfilter
{
public:
   STMZIPfilterDecompress( COLsink* ipNext );
   virtual ~STMZIPfilterDecompress();

   virtual COLuint32 write( const void* cpBuffer, COLuint32 SizeOfBuffer );
   virtual void flush();
   virtual void resetFilter(){};

private:
   STMZIPfilterDecompressPrivate* pMember;
};

#endif //end of defensive include
