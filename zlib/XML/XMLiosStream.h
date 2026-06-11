#ifndef __XML_IOS_STREAM_H__
#define __XML_IOS_STREAM_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc. All Rights Reserved
//
// Description:
//
// Stream that does escaping for XML. The stream is an efficient buffering 
// class that can do escaping for tags and data differently on the fly,
// by using dynamic string sinks that write to the same buffer (which this stream
// owns).
//
// Typically this is used as:
//
// myXMLstream << startTag << "HELLO" << newline << flush;
// 
// will result in <HELLO>
//
// similiar usage exists for the other state changing functions (eg, endTag, unescaped, etc)
//
// Author: Nasron Cheong
// Date:   Nov 2003
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
//#include <COL/COLstring.h>

class XMLiosStreamPrivate;
class XMLiosTagFilter;
class XMLiosDataFilter;
class COLsink;
class COLstring;

class XMLiosStream
{
public:
   XMLiosStream(char XmlDelimiter, COLsink* pSink, bool isOwner = false);
   virtual ~XMLiosStream();

   XMLiosStream& operator<<(XMLiosStream&(*x)(XMLiosStream&));
   XMLiosStream& operator<<(XMLiosStream&(*)(XMLiosStream&, int n)){return *this;}; 

   XMLiosStream& operator<<(char Character);
   XMLiosStream& operator<<(unsigned char Character);
   XMLiosStream& operator<<(const char* pString);
   XMLiosStream& operator<<(char* pString);
   XMLiosStream& operator<<(const COLstring& String);

   XMLiosStream& operator<<(unsigned long Number);
   XMLiosStream& operator<<(long Number);
   XMLiosStream& operator<<(short Number);
   XMLiosStream& operator<<(double Number);
   XMLiosStream& operator<<(unsigned int Number);
   XMLiosStream& operator<<(int Number);
#if defined(_WIN64)
   // this is causing confusion for WIN64 - a workaround or a cast is needed
   // limiting it to WIN64 to avoid side effects
   XMLiosStream& operator<<(size_t Number);
#endif
   
   void setNewLine(const char* NewLine);
   void setXmlDelimiter(char XmlDelimiter);
   
   //sets the target sink;
   void setSink(COLsink* pSink, bool isOwner = true);
   void setSink(COLsink& Sink) { setSink(&Sink,false); }
   
   //clears the current state and returns to element mode.
   //does not clear the underlying sink
   void clear();
   
   //sets the sinks used to escape the tags and data
   //takes ownership
   void setTagFilter(XMLiosTagFilter* pTagFilter);
   void setDataFilter(XMLiosDataFilter* pDataFilter);

   //adds the ability to output an attribute, must be in 
   //a start tag state
   void addAttribute(const char* pAttributeName, const char* pAttribute);

   //public due to the state changing functions
   XMLiosStreamPrivate* pMember;

   //performs a write operation
   void write(const void* pData, COLuint32 Size);

private:
   XMLiosStream(const XMLiosStream& Orig);
   XMLiosStream(const COLostream& Orig);
   XMLiosStream& operator=(const XMLiosStream& Orig);
};


//will cause the stream to enter escape tag mode, where the resulting output
//will be a escaped with tag escaping conventions.
//
// eg myXMLstream << escapeTag << "123HEL$LO" << newline << flush;
//
// results in the output "_23HELLO"
XMLiosStream& escapeTag(XMLiosStream& _outs);

//will cause the stream to enter data mode, where the resulting output
//will be a escaped with xml data escaping conventions.
//
// eg myXMLstream << elementData << "& HELLO" << newline << flush;
//
// results in the output "&amp; HELLO"
XMLiosStream& elementData(XMLiosStream& _outs);

//will cause the stream to enter attribute data mode, where the resulting output
//will be a escaped with xml data escaping conventions.
//
// eg myXMLstream << attributeData << "& HELLO" << newline << flush;
//
// results in the output "&amp; HELLO"
XMLiosStream& attributeData(XMLiosStream& _outs);

//will insert the current delimiter into the stream
//can only be used while still in tag mode
XMLiosStream& delimiter(XMLiosStream& _outs);

//will cause the stream to enter escape tag mode, where the resulting output
//will be a escaped with tag escaping conventions, along with the brackets.
//
// eg myXMLstream << startTag << "123HEL$LO" << newline << flush;
//
// results in the output "<_23HELLO>"
XMLiosStream& startTag(XMLiosStream& _outs);

//will cause the stream to enter escape tag mode, where the resulting output
//will be a escaped with tag escaping conventions, along with the brackets.
//
// eg myXMLstream << endTag << "123HEL$LO" << newline << flush;
//
// results in the output "</_23HELLO>"
XMLiosStream& endTag(XMLiosStream& _outs);

//will write directly to the underlying buffer without escaping
XMLiosStream& unescaped(XMLiosStream& _outs);

//will put a newline the stream, and immediately switch to elementData mode
XMLiosStream& newline(XMLiosStream& _outs);

//flushes current sink
XMLiosStream& flush(XMLiosStream& _outs);

#endif // end of defensive include
