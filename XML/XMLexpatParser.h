#ifndef __XML_EXPAT_PARSER_H__
#define __XML_EXPAT_PARSER_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLexpatParser
//
// Description:
//
// Simple C++ class wrapper over Jame's Clark's expat parser.  This
// is good for parsing XML strings in memory.  For parsing large XML
// files, or groups of files you should use XMLfileParser instead since
// this is much more efficient.
//
// Author: Eliot Muir
// Date:   Sun 06/03/2001 
//
//---------------------------------------------------------------------------

#include <COL/COLerror.h>

class XMLexpatParserPrivate;
class COLstring;


struct XMLexpatParserError : public COLerror
{
   XMLexpatParserError
   (
      const COLstring& ErrorText, COLuint32 ErrorCode,
      int iLine, int iColumn, long iByte, int iLength
   )
      : COLerror(ErrorText, ErrorCode)
      , Line(iLine)
      , Column(iColumn)
      , Byte(iByte)
      , Length(iLength)
   { }

   const int  Line;   // The line where the error originated.
   const int  Column; // The column on that line of same.
   const long Byte;   // The byte offset from the beginning of the file.
   const int  Length; // The length of the problem area.
};

class XMLexpatParser{
public:
   XMLexpatParser();
   virtual ~XMLexpatParser();

   void parse(const COLstring& XMLdata);

   // the following methods must be called if the parseBuffer/flush routine
   // is being used.
   void init();
   void destroy();
   void flush();

   // see note about init and destroy
   void parseBuffer(const char* Buffer, COLuint32 SizeOfBuffer, bool IsFinal);

   bool isUnbalanced() const;
   int countOfStartElement() const;
   int countOfEndElement() const;

   //current line/col info
   COLuint32 currentLineNumber();
   COLuint32 currentColumnNumber();


   virtual void onStartElement(const char* Name, const char** attr)=0;
   virtual void onEndElement(const char* Name)=0;
   virtual void onCharacterData(const char* Data, int Length)=0;
   virtual void onComment(const char* Data){};
   virtual void onStartCdataSection(){};
   virtual void onEndCdataSection(){};
      
   virtual XMLexpatParserError onError
   (
      const COLstring& XmlError,
      int Line, int Column, long Byte, int Length
   );

friend class XMLexpatParserPrivate;

private:
   XMLexpatParserPrivate* pMember;
   
   // not allowed
   XMLexpatParser(const XMLexpatParser& Orig);
   XMLexpatParser& operator=(const XMLexpatParser& Orig);
};
#endif // end of defensive include


