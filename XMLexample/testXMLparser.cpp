//---------------------------------------------------------------------------
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Tests for XMLexparParser
//
// Author: Nasron Cheong
// Date:   Nov 2003
//
//---------------------------------------------------------------------------
#include <XML/XMLexpatParser.h>

#include <COL/COLlog.h>
#include <COL/COLstring.h>
#include <COL/COLsinkStandard.h>
#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>
COL_LOG_MODULE;

class XMLparserTest : public XMLexpatParser
{
public:
   virtual void onStartElement(const char* Name, const char** attr)
   {
      COL_TRC("StartElement = " << Name);
   }

   virtual void onEndElement(const char* Name)
   {
      COL_TRC("EndElement = " << Name);
   }

   virtual void onCharacterData(const char* Data, int Length)
   {
      COLstring Log;
      Log.write(Data, Length);
      COL_TRC("CharacterData = " << Log);
   }
};

class XMLparserTestStartElementException : public XMLparserTest
{
public:
   virtual void onStartElement(const char* Name, const char** attr)
   {
      XMLparserTest::onStartElement(Name,attr);
      COL_ERROR_STREAM("Test Error",0)
   }
};

class XMLparserTestEndElementException : public XMLparserTest
{
public:
   virtual void onEndElement(const char* Name)
   {
      XMLparserTest::onEndElement(Name);
      COL_ERROR_STREAM("Test Error",0)
   }
};

class XMLparserTestDataException : public XMLparserTest
{
public:
   virtual void onCharacterData(const char* Data, int Length)
   {
      XMLparserTest::onCharacterData(Data,Length);
      COL_ERROR_STREAM("Test Error",0)
   }
};

void testXMLparserStartElementException()
{
   XMLparserTestStartElementException TestParser;
   bool ErrorOccurred = false;
   try
   {
      TestParser.parse("<TST>SOMEDATA</TST>");
   }
   catch(COLerror& Error)
   {
      COL_TRC(Error.description());
      ErrorOccurred = true;
   }
   TST_ASSERT(ErrorOccurred);
}

void testXMLparserEndElementException()
{
   XMLparserTestEndElementException TestParser;
   bool ErrorOccurred = false;
   try
   {
      TestParser.parse("<TST>SOMEDATA</TST>");
   }
   catch(COLerror& Error)
   {
      COL_TRC(Error.description());
      ErrorOccurred = true;
   }
   TST_ASSERT(ErrorOccurred);
}

void testXMLparserDataException()
{
   XMLparserTestDataException TestParser;
   bool ErrorOccurred = false;
   try
   {
      TestParser.parse("<TST>SOMEDATA</TST>");
   }
   catch(COLerror& Error)
   {
      COL_TRC(Error.description());
      ErrorOccurred = true;
   }
   TST_ASSERT(ErrorOccurred);
}

TSTtestable* getXMLparserTests()
{
   TST_START_COLLECTION("testXMLparser")
      TST_ADD_TEST_FUNCTION(testXMLparserStartElementException)
      TST_ADD_TEST_FUNCTION(testXMLparserEndElementException)
      TST_ADD_TEST_FUNCTION(testXMLparserDataException)
   TST_END_COLLECTION
}
