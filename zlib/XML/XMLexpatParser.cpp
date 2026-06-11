//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLexpatParser
//
// Description:
//
// Simple C++ class wrapper over Jame's Clark's expat parser
// Implementation
//
// Author: Eliot Muir
// Date:   Sun 06/03/2001 
//
//---------------------------------------------------------------------------

#include "XMLprecomp.h"
#include "XMLexpatParser.h"
#pragma hdrstop

#include <EXP/expat.h>
#include <XML/XMLexpatParser.h>
#include <COL/COLerror.h>
#include <COL/COLauto.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

class XMLexpatParserPrivate
{
public:
   XMLexpatParserPrivate() : parser(NULL), StartElems(0), EndElems(0) {
      COL_METHOD(XMLexpatParserPrivate::XMLexpatParserPrivate);
   }
   
   XML_Parser parser;

   // this must be called before each invocation to the parser
   // of a new document since the expat parser requires XML_ParserCreate
   // and XML_ParserFree to be called before and after each parse of the data.
   
   void initParser(XMLexpatParser* pThis);  
   void destroyParser();

   static void startElementHandler(void* userData, 
                                   const XML_Char* Name,
                                   const XML_Char** atts);

   static void endElementHandler(void* userData,
                                 const XML_Char* Name);

   static void characterDataHandler(void *userData,
                                    const XML_Char *s,
                                    int len);
   static void commentHandler(void* userData,
                              const XML_Char* Data);

   static void startCdataSectionHandler(void* userData);
   static void endCdataSectionHandler(void* userData);

   COLauto<COLerror> pLastException;

   // #24788 Expat parser tolerates incomplete XML documents if the outermost
   // closing tags are missing. Track opened and closed tags. Use isUnbalanced() 
   // and throw an error if the counts don't match.
   int StartElems;
   int EndElems;
};

void XMLexpatParserPrivate::initParser(XMLexpatParser* pThis)
{
   COL_METHOD(XMLexpatParserPrivate::initParser);
   StartElems = 0;
   EndElems = 0;
   pLastException = 0;
   COLPRECONDITION(parser == NULL);
   parser = ::XML_ParserCreate(NULL);
   ::XML_SetUserData(parser, pThis); 
   ::XML_SetElementHandler(parser, XMLexpatParserPrivate::startElementHandler,
                           XMLexpatParserPrivate::endElementHandler); 
   ::XML_SetCharacterDataHandler(parser, XMLexpatParserPrivate::characterDataHandler);
   ::XML_SetCommentHandler (parser, XMLexpatParserPrivate::commentHandler);
   ::XML_SetCdataSectionHandler(parser, XMLexpatParserPrivate::startCdataSectionHandler,
                           XMLexpatParserPrivate::endCdataSectionHandler); 
}

void XMLexpatParserPrivate::destroyParser()
{
   COL_METHOD(XMLexpatParserPrivate::destroyParser);
   COL_VAR2(StartElems, EndElems);
   if (parser)
   {
      ::XML_ParserFree(parser); 
   }
   parser = NULL;
}


void XMLexpatParserPrivate::commentHandler(void* userData, const XML_Char* Data)
{
   XMLexpatParser* pCppParser = ((XMLexpatParser*)userData);
   if (!pCppParser->pMember->pLastException.get())
   {
      try
      {
         ((XMLexpatParser*)userData)->onComment(Data);
      }
      catch(COLerror& Error)
      {
         pCppParser->pMember->pLastException = new COLerror(Error);
      }
   }
};


void XMLexpatParserPrivate::startCdataSectionHandler(void* userData)
{
   XMLexpatParser* pCppParser = ((XMLexpatParser*)userData);
   if (!pCppParser->pMember->pLastException.get())
   {
      try
      {
         ((XMLexpatParser*)userData)->onStartCdataSection();
      }
      catch(COLerror& Error)
      {
         pCppParser->pMember->pLastException = new COLerror(Error);
      }
   }   
};

void XMLexpatParserPrivate::endCdataSectionHandler(void* userData)
{
   XMLexpatParser* pCppParser = ((XMLexpatParser*)userData);
   if (!pCppParser->pMember->pLastException.get())
   {
      try
      {
         ((XMLexpatParser*)userData)->onEndCdataSection();
      }
      catch(COLerror& Error)
      {
         pCppParser->pMember->pLastException = new COLerror(Error);
      }
   }   
};


void XMLexpatParserPrivate::endElementHandler(void* userData, const XML_Char* Name)
{
   COL_FUNCTION(XMLexpatParserPrivate::endElementHandler);
   XMLexpatParser* pCppParser = ((XMLexpatParser*)userData);
   if (!pCppParser->pMember->pLastException.get())
   {
      try
      {
         pCppParser->pMember->EndElems++;
         COL_VAR(pCppParser->pMember->EndElems);
         ((XMLexpatParser*)userData)->onEndElement(Name);
      }
      catch(COLerror& Error)
      {
         pCppParser->pMember->pLastException = new COLerror(Error);
      }
   }   
}

void XMLexpatParserPrivate::startElementHandler(void* userData, const XML_Char* Name, const XML_Char** atts)
{
   COL_FUNCTION(XMLexpatParserPrivate::startElementHandler);
   XMLexpatParser* pCppParser = ((XMLexpatParser*)userData);
   if (!pCppParser->pMember->pLastException.get())
   {
      try
      {
         pCppParser->pMember->StartElems++;
         COL_VAR(pCppParser->pMember->StartElems);
         ((XMLexpatParser*)userData)->onStartElement(Name,atts); 
      }
      catch(COLerror& Error)
      {
         pCppParser->pMember->pLastException = new COLerror(Error);
      }
   }   
}

void XMLexpatParserPrivate::characterDataHandler(void *userData, const XML_Char* Data, int Length)
{
   COL_FUNCTION(XMLexpatParserPrivate::characterDataHandler);
   XMLexpatParser* pCppParser = ((XMLexpatParser*)userData);
   if (!pCppParser->pMember->pLastException.get())
   {
      try
      {
         COL_VAR(Data);
         ((XMLexpatParser*)userData)->onCharacterData(Data, Length);
      }
      catch(COLerror& Error)
      {
         pCppParser->pMember->pLastException = new COLerror(Error);
      }
   }   
}

////////////////////////////////////////////////////////////////////////////////


XMLexpatParser::XMLexpatParser()
{
   COL_METHOD(XMLexpatParser::XMLexpatParser);
   pMember = new XMLexpatParserPrivate();
}

XMLexpatParser::~XMLexpatParser()
{   
   COL_METHOD(XMLexpatParser::~XMLexpatParser);
   if (pMember->parser != NULL)
   {
      pMember->destroyParser();
   }
   delete pMember;
}

void XMLexpatParser::parse(const COLstring& XMLdata)
{
   COL_METHOD(XMLexpatParser::parse);

   COL_VAR(XMLdata);
   pMember->initParser(this);
   try
   {
      parseBuffer(XMLdata.c_str(), XMLdata.size(), true);
   }
   catch (COLerror& Error)
   { 
      pMember->destroyParser();
      throw Error;
   }

   pMember->destroyParser();
}

void XMLexpatParser::flush()
{
   COL_METHOD(XMLexpatParser::flush);
   COLPRECONDITION(pMember->parser != NULL)
   parseBuffer(0,0,true);
}

XMLexpatParserError XMLexpatParser::onError
(
   const COLstring& XmlError,
   int Line, int Column, long Byte, int Length
)
{
   COL_METHOD(XMLexpatParser::onError);
   COLstring ErrorString;
   COLostream ErrorStream(ErrorString);
   ErrorStream << COL_T("xml error on line ") << Line << COL_T(" column ") << Column << COL_T(". ") << XmlError;

   return XMLexpatParserError(ErrorString, COLerror::SystemError, Line, Column, Byte, Length);
}

void XMLexpatParser::init()
{
   COL_METHOD(XMLexpatParser::init);
   if (!pMember->parser)
   {
      pMember->initParser(this);
   }
}

void XMLexpatParser::destroy()
{
   COL_METHOD(XMLexpatParser::destroy);
   pMember->destroyParser();
}

bool XMLexpatParser::isUnbalanced() const{
   return !(pMember->StartElems == pMember->EndElems);
}

int XMLexpatParser::countOfStartElement() const{
   return pMember->StartElems;
}

int XMLexpatParser::countOfEndElement() const{
   return pMember->EndElems;
}

void XMLexpatParser::parseBuffer(const char* Buffer, COLuint32 SizeOfBuffer, bool IsFinal) 
{
   COL_FUNCTION(XMLexpatParser::parseBuffer);
   COLPRECONDITION(pMember->parser != NULL);

   if (!::XML_Parse(pMember->parser, Buffer, SizeOfBuffer, IsFinal)) {
      COL_TRC("XML parsing error occurred");
      throw onError(XML_ErrorString(XML_GetErrorCode(pMember->parser)),
                    XML_GetCurrentLineNumber(pMember->parser),
                    XML_GetCurrentColumnNumber(pMember->parser),
                    XML_GetCurrentByteIndex(pMember->parser),
                    XML_GetCurrentByteCount(pMember->parser));
   } 
 
   //check if exception occurred
   if (pMember->pLastException.get()) {
      throw *pMember->pLastException;
   }
}

COLuint32 XMLexpatParser::currentLineNumber()
{
   COLPRECONDITION(pMember->parser != NULL);
   return XML_GetCurrentLineNumber(pMember->parser);
}

COLuint32 XMLexpatParser::currentColumnNumber()
{
   COLPRECONDITION(pMember->parser != NULL);
   return XML_GetCurrentColumnNumber( pMember->parser );
}

