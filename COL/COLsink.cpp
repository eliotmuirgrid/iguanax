//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
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
//---------------------------------------------------------------------------

#include "COLsink.h"

// constructor and destructor have to go here because this function
// gets exported in a DLL
COLsink::COLsink(){
   // do nothing
}

COLsink::~COLsink()
{
  // do nothing - we used to flush but it's very bad to
  // call virtual functions in destructors
}


void COLsink::flush()
{
   // do nothing. - we do this because other wise the compiler links the symbol
   // in multiple times which causes problems when COLsink is in DLL.  Hope there
   // is a way around this problem.
}

void COLsink::onEndStream()
{
   // do nothing. - we do this because other wise the compiler links the symbol
   // in multiple times which causes problems when COLsink is in DLL.  Hope there
   // is a way around this problem.
}

bool COLsink::readAll( COLsource& Source )
{
   return false;
}
COLuint32 COLsink::writeString(const COLstring& String) {
   return 0;
}
