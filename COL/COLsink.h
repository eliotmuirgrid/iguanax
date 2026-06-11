#ifndef __COL_SINK_H__
#define __COL_SINK_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLsink
//
// Description:
//
// This is the base class for all objects that can act as
// a data sink.  Data in a COLsource object is transferred to
// an attached COLsink object when writeAll() is called on the
// source object.  
//
// COLostreams can also be used to stream data to COLsinks
// using the << operator.
//
// The COLsource and COLsink abstract base classes should be used
// to implement all reading/writing of data between application 
// objects.
//
// Subclasses only need to implement the write() method
// found on this interface. It will be called by the associated
// COLsource object when data is ready to be sent to it.
//
// The data transfer process from a COLsource to a COLsink is
// initiated when the writeAll() method is called on the source.
// The writeAll() method will invoke writeChunk() until it
// sets EndOfSource to true.
//
// Example usage:
//
//   MYsinkClass SinkObject;
//   ...
//   MYsourceClass SourceObject(SinkObject);
//   ...
//   SourceObject.writeAll();
//
// Important exception to the above: If this COLsink choses to
// override the readAll() method (and return true from it) then
// this COLsink assumes responsibility for getting
// the COLsource to send its data.  This means that the COLsink
// has the job of calling writeChunk(ChunkSize) on the source object.  
// This gives the sink the ability to pull data from
// the source exactly when it needs it so that the sink doesn't need
// to buffer too much data.  Otherwise (in the case that readAll() is not 
// implemented by the sink) the source will repeatedly
// call writeChunk(ChunkSize) until EndOfStream is true as described
// earlier.
//
// Author: Eliot Muir, Ryan Dewsbury, Rob Moyse
// Date:   November 12, 2004
//
//---------------------------------------------------------------------------
#include "COLminimumInclude.h"

class COLstring;
class COLsource;

class COLsink{
public:
   COLsink();
   virtual ~COLsink();
   // write() should return the amount of bytes
   // actually written from cpBuffer.
   virtual COLuint32 write( const void* cpBuffer, 
                            COLuint32 SizeOfBuffer ) = 0;
   virtual COLuint32 writeString(const COLstring& String);
   virtual void flush();
   virtual void onEndStream();

   // This method should only be implemented by sinks that wish
   // to actively pull data from a source object.  Most sinks
   // will not need this since they can function efficiently
   // having data pushed to them. (see discussion in header)
   //
   // Any sink that implements this readAll() functionality
   // must return true from this function (default is false).
   //
   // The source attached to this sink (ie. the original source,
   // not any of the filters) will be passed into the readAll() method
   // when Source.writeAll() is called. If readAll() returns
   // true then the source will not push any data to the sink.
   // Instead, the implementation of this method should
   // call Source.writeChunk() to retrieve data from the 
   // source when it wants the data.
   virtual bool readAll(COLsource& Source);
};

#endif // end of defensive include
