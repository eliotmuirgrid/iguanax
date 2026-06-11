#ifndef __COL_SINK_COMPOSITE_H__
#define __COL_SINK_COMPOSITE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLcompositeSink
//
// Description:
//
// Composite sink object.  This is a sink which will write
// to 1 to many other sink objects.  It's useful for times when
// you want to write to more than one place - i.e. the screen
// and a file.
//
// Author: Eliot Muir
// Date:   Fri 01/16/2004 
//---------------------------------------------------------------------------
#include "COLsink.h"

class COLsinkCompositePrivate;

class COLsinkComposite : public COLsink{
public:
   COLsinkComposite();
   virtual ~COLsinkComposite();
   virtual COLuint32 write(const void* cpBuffer, 
                           COLuint32 SizeOfBuffer);
   virtual COLuint32 writeString(const COLstring& String);

   virtual void flush();   

   void addSink(COLsink* pSink, bool IsOwner);

   void clearSinkVector();

private:
   COLsinkCompositePrivate* pMember;
};

#endif // end of defensive include

