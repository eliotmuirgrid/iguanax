#ifndef __TSM_WRITE_BUFFER_H__
#define __TSM_WRITE_BUFFER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSMwriteBuffer
//
// Description:
//
// TXMwriteBuffer class.  This idea of this class is to buffer writes
// so that we do a few large writes of data to the file system rather than
// many small writes.  It gives some pretty dramatic improvements in speed.
//
// WARNING - make sure that flush is called at the end of using this buffer
// before the underlying sink is cleared.
//
// Author: Eliot Muir
// Date:   Monday, February 12th, 2007 @ 02:11:52 PM
//
//---------------------------------------------------------------------------

#include <COL/COLsink.h>

class TSMwriteBuffer : public COLsink
{
public:
   TSMwriteBuffer(COLsink& Sink);
   // This is non virtual because we are assuming no classes will inherit from this class
   ~TSMwriteBuffer();

   virtual COLuint32 write(const void* cpBuffer, COLuint32 SizeOfBuffer);

   virtual void flush();

private:
   // If this changes, update testTSMwriteBuffer!
   enum {MaxBufferSize=30000};
   char Buffer[MaxBufferSize];
   COLuint32 SizeOfData;

   COLsink* pSink;

   TSMwriteBuffer(const TSMwriteBuffer& Orig); // not allowed
   TSMwriteBuffer& operator=(const TSMwriteBuffer& Orig); // not allowed
};

#endif // end of defensive include
