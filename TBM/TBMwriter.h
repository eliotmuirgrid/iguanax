#ifndef __TBM_WRITER_H__
#define __TBM_WRITER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TBMwriter
//
// Description:
//
// TBMwriter implements the TXMwriter interface outputing the data in binary
// format.
//
// Author: Eliot Muir
// Date:   Friday, February 9th, 2007 @ 07:32:49 PM
//
//---------------------------------------------------------------------------

#include <TSM/TSMwriter.h>

class COLsink;

class TBMwriter : public TSMwriter
{
public:
   TBMwriter(COLsink& Sink);
   // This is non virtual because we are assuming no classes will inherit from this class
   virtual ~TBMwriter();

   virtual void beginObject(const TSMlabel& Label);

   virtual void addBoolean(const TSMlabel& Label, bool Value); 
   virtual void addInt32 (const TSMlabel& Label, COLint32 Value); 
   virtual void addString (const TSMlabel& Label, const COLstring& Value);

   virtual void endPropertyList(const TSMlabel& Label);

   virtual void endSelfContainedObject(const TSMlabel& Label);
   
   virtual void endObject(const TSMlabel& Label);

   virtual void flush();
private:
   COLsink& Sink;
   TBMwriter(const TBMwriter& Orig); // not allowed
   TBMwriter& operator=(const TBMwriter& Orig); // not allowed
};
#endif // end of defensive include
