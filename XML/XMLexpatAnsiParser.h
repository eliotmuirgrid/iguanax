#ifndef __XML_EXPAT_ANSI_PARSER_H__
#define __XML_EXPAT_ANSI_PARSER_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLexpatAnsiParser
//
// Description:
//
// Expat gives all data provided through its callbacks in UTF-8 encoding
// which is fine for most characters with a value less than 128. However
// that means that characters between 128 and 255 are multi-byte encoded.
//
// Since currently all uses of expat are in the context of ANSI, this 
// class wraps expat and attempts to transcode all UTF-8 multi-byte data into
// the ANSI value, and throws an exception if the transcoding fails (character
// value is more than 255).
//
// Author: Nasron Cheong
// Date:   May 2005
//
//---------------------------------------------------------------------------

#include "XMLexpatParser.h"

// Note that the interface is the same as XMLexpatParser
// but the functionality is subclassed by the private member
// in order to retain the same virtual function names.
class XMLexpatAnsiParserPrivate;
class XMLexpatAnsiParser
{
public:
   XMLexpatAnsiParser();
   virtual ~XMLexpatAnsiParser();

   void parse(const COLstring& XMLdata);

   // the following methods must be called if the parseBuffer/flush routine
   // is being used.
   void init();

   void destroy();

   // see note about init and destroy
   void parseBuffer(const char* Buffer, COLuint32 SizeOfBuffer, bool IsFinal);
   void flush();

   //current line/col info
   COLuint32 currentLineNumber();
   COLuint32 currentColumnNumber();

   // The parameters of most of these virtual functions are transcoded before being passed
   // to the implementation
   virtual void onStartElement(const char* Name, const char** attr)=0;
   virtual void onEndElement(const char* Name)=0;
   virtual void onCharacterData(const char* Data, int Length)=0;
   virtual void onComment(const char* Data){};
   virtual void onStartCdataSection(){};
   virtual void onEndCdataSection(){};

   virtual XMLexpatParserError onError
   (
      const COLstring& ErrorText,
      int Line, int Column, long Byte, int Length
   );

   //for unittest
   friend class XMLexpatAnsiParserTest;

private:
   XMLexpatAnsiParserPrivate* pMember;

   //these are exposed for the unittest
   const char** utf8PtrArrayToAnsi(const char** pPtrArrayIn);
   COLstring utf8ToAnsiColString
   (
      //both parameters must be in the same string
      const char* pOrigString, //the beginning of the entire string
      const char* pStartTranscodeString //the first character at which transcoding should start
   );      
   COLuint32 utf8CharToAnsiChar(const COLuint8* pInput, COLuint8* pOutput);
   COLstring utf8ToAnsi(const char* pInput);   
   
   // not allowed
   XMLexpatAnsiParser(const XMLexpatAnsiParser& Orig);
   XMLexpatAnsiParser& operator=(const XMLexpatAnsiParser& Orig);
};
#endif // end of defensive include


