#ifndef __COL_SINK_CALLBACK_H__
#define __COL_SINK_CALLBACK_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLsinkCallback
//
// Description:
//
// Callback sink object. Almost identical to the COLsink class,
// except that it allows for a custom write() method. It's somewhat
// legacy though, since it converts the call back to using the 
// COLcallback function interface.
//
// Note that since the callback we're using has been declared long
// before the transition from write() returning void to write() returning
// COLuint32, it is made such that by default, COLsinkCallback::write()
// returns SizeOfBuffer. This is a limitation, since the callback you
// define may not necessarily use all of the data in SizeOfBuffer.
//
// Author: Eliot Muir
// Date:   Fri 01/16/2004 
//---------------------------------------------------------------------------

#include "COLsink.h"
#include "COLstring.h"

class COLsinkCallback : public COLsink{
public:
   typedef void (COL_STD_CALL* flushCallback)(const char* callback);
public:
   COLsinkCallback(flushCallback iCallback);
   COLuint32 write(const void* cpBuffer, COLuint32 SizeOfBuffer);
   COLuint32 writeString(const COLstring& String);

   void flush();
private:
   COLstring LineBuffer;
   flushCallback Callback;
   COLsinkCallback(const COLsinkCallback& Orig); // not allowed
   COLsinkCallback& operator=(const COLsinkCallback& Orig); // not allowed
};
// Cannot remember why we undefine the symbol here....
#undef COL_STD_CALL

#endif // end of defensive include

