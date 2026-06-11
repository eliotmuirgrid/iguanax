#ifndef __DOM_SINK_H__
#define __DOM_SINK_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMsink
//
// Description:
//
// Parses XML data to populate a DOMdocument object.
//
// Note: After constructing - must call init() before writing
// to this object.
//
// Author: Rob Moyse
// Date:   November 9, 2004
//
//---------------------------------------------------------------------------

#include <COL/COLsink.h>

#include <XML/XMLexpatParser.h>

class DOMsinkPrivate;

class DOMsink 
   : public COLsink,    
     public XMLexpatParser
{
public:

   /**
    * Parser constructor.  Takes a reference to the document that
    * we're going to be populating as well as the DocumentSize which
    * allows for one-time allocation of enough memory in the DOMdocument's
    * dataBuffer() to handle all the tag, attribute information.
    */
   DOMsink( DOMdocument& Document, COLuint32 DocumentSize );
   virtual ~DOMsink();

   virtual COLuint32 write( const void* cpBuffer, COLuint32 SizeOfBuffer );
   virtual void flush();

   // expat handlers
   virtual void onStartElement(const char* pName, const char** ppAttributes);
   virtual void onEndElement(const char* Name);
   virtual void onCharacterData(const char* Data, int Length);
   virtual void onComment(const char* Data);
   virtual void onStartCdataSection();
   virtual void onEndCdataSection();

protected:

   const char* copyValueToDocumentBuffer(const char* pName, COLuint32* pLength) const;
   const char* copyValueToDocumentBuffer(const char* pName, COLuint32 Length) const;

   void handleError( COLuint32 Error ) const;

private:

   DOMsinkPrivate* pMember;

   DOMsink(); // not allowed
   DOMsink(const DOMsink& Orig); // not allowed
   DOMsink& operator=(const DOMsink& Orig); // not allowed

};

#endif // end of defensive include
