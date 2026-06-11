#ifndef __TSM_WRITER_H__
#define __TSM_WRITER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSMwriter
//
// Description:
//
// Every abstraction comes with a price...
//
// TSMwriter class.  The idea here is to abstract a little bit from
// the format being written too.  I say a little bit in the sense that
// the abstraction here is designed to give reasonable support for the
// features of the two formats that we are supporting - namely XML
// and a smaller binary format.
//
// Since XML supports the concepts of tag attributes, self contained
// tags etc. like:
// <config name="fred" index="1"/>
//
// and tags which contain other tags:
// <config name="fred" index="1">
//    <segment name="PID"/>
// </config>
//
// Then our interfaces need to support these concepts.  Also this interface
// is designed so that it can be implemented using streaming which gives
// fast performance since we don't have to build up string and trees in memory
// (no unncessary copying and mallocs) 
// This means that we cannot abstract the order in which
// we define things.  Attributes have to be defined before sub objects since
// we have to close the tag they are contained in.
//
// This code was originally in TXM/TXMwriter 
//
// Author: Eliot Muir
// Date:   Monday, February 12th, 2007 @ 09:23:12 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class TSMlabel;

class TSMwriter 
{
public:
   TSMwriter() {}
   virtual ~TSMwriter() {}

   virtual void beginObject(const TSMlabel& Label)=0;

   virtual void addBoolean(const TSMlabel& Label, bool Value)=0; 
   virtual void addInt32  (const TSMlabel& Label, COLint32 Value)=0;
   virtual void addString (const TSMlabel& Label, const COLstring& Value)=0;

   // This is where our leaky abstraction shows.  In XML endPropertyList will
   // basically end the start tag using a '>' and a newline.  This still allows
   // objects to be added.
   virtual void endPropertyList(const TSMlabel& Label)=0;
   // In the case we have an object with no sub objects we use endSelfContainedObject
   // which ends the tag as a self contained tag - ala "/>".  This means that the next
   // object will be adjacent rather than being a sub object.
   virtual void endSelfContainedObject(const TSMlabel& Label)=0;

   // This method generates the final closing tag - ala </config>.  It should only
   // be used for objects that have sub objects.
      
   virtual void endObject(const TSMlabel& Label)=0;

   virtual void flush()=0;
private:
   TSMwriter(const TSMwriter& Orig); // not allowed
   TSMwriter& operator=(const TSMwriter& Orig); // not allowed
};

#endif // end of defensive include
