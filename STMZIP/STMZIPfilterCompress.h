#ifndef __STMZIP_FILTER_COMPRESS_H_
#define __STMZIP_FILTER_COMPRESS_H_
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: STMZIPfilterCompress
//
// Description:
//
// Author: Ryan Dewsbury
//
//---------------------------------------------------------------------------


#include <COL/COLfilter.h>

struct STMZIPfilterCompressPrivate;

class STMZIPfilterCompress : 
   public COLfilter
{
public:
   STMZIPfilterCompress( COLsink* ipNext );
   virtual ~STMZIPfilterCompress();

   virtual COLuint32 write( const void* cpBuffer, COLuint32 SizeOfBuffer );
   virtual void flush();

   virtual void resetFilter(){};
private:
   STMZIPfilterCompressPrivate* pMember;
};

#endif //end of defensive include
