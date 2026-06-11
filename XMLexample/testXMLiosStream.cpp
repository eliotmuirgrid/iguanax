//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testXMLiosStream
//
// Description: Unittest
//
// Author: Nasron Cheong
// Date:   Wed 05/01/2002
//
//---------------------------------------------------------------------------
#include <XMLexample/testXMLiosStream.h>

#include <XML/XMLiosStream.h>
#include <XML/XMLiosNullDataFilter.h>

#include <COL/COLlog.h>
#include <COL/COLstring.h>
#include <COL/COLsinkStandard.h>
#include <COL/COLsinkString.h>
COL_LOG_MODULE;

#include <TST/TSThelpers.h>
#include <TST/TSTtestFixture.h>
#include <TST/TSTtestMethod.h>
#include <FMT/FMTbinaryDump.h>

class testXMLbidirSink : public COLsink
{
public:
   virtual COLuint32 write(const void* pData,COLuint32 Size)
   {
      Log.write(pData,Size);
      Data.write(pData,Size);

      return Size;
   }

   COLstring Log;
   COLstring Data;
};

void testXMLiosStream::basicExample()
{
   COLstring Newline;
   COLostream NewLineStream(Newline);
   NewLineStream << newline;

   //the stream we are writing data to, in this case we use cout as the sink
   testXMLbidirSink Sink;
   XMLiosStream XmlStream('.',&Sink,false);

   //Tags are written like this. The result will be <Tag1>
   XmlStream << startTag << "Tag1" << newline;
   TST_ASSERT(Sink.Data == "<Tag1>"+ Newline);
   Sink.Data.clear();

   //Escaping is also done for tags if they start with numbers resulting in <_Tag2>
   XmlStream << startTag << "8Ta$g2!@#$%^&*(){}\'\"?><~`-+=|\\/*;:" << newline;
   TST_ASSERT(Sink.Data == "<_Tag2__>"+ Newline);
   Sink.Data.clear();

   //Tag delimiters can also be escaped. This results in Tag3.Data
   //elementdata allows us to switch to data mode without adding a new line
   XmlStream << startTag << "Ta.g3" << delimiter << "Data" << elementData;
   TST_ASSERT(Sink.Data == "<Tag3.Data>");
   Sink.Data.clear();

   //data is then written like this. Since we did a newline before this call, we are now
   //escaping data, and not tags
   COLstring TestColstring("TestColstring");
   XmlStream << "Data for Tag 2 Int: " << 99 << " double: " << 9.999 << " Colstring: " << TestColstring << newline;
   TST_ASSERT(Sink.Data == "Data for Tag 2 Int: 99 double: 9.999000 Colstring: TestColstring" + Newline);
   Sink.Data.clear();

   //escaping can be seen as follows
   XmlStream << "Escaped: "<< " & % \" ` " << newline;
   TST_ASSERT(Sink.Data == "Escaped:  &amp; % &quot; ` " + Newline);
   Sink.Data.clear();

   //data can also be written unescaped directly to the sink
   XmlStream << unescaped << " Unescaped: "<< " & % \" ` " << newline;
   TST_ASSERT(Sink.Data == " Unescaped:  & % \" ` " + Newline);
   Sink.Data.clear();

   //if tag escaping is desired without the actual < >  brackets (for making xsds for example)
   //escapeTag is available
   XmlStream << "Escaped Tag " << escapeTag << "8Ta$g2" << elementData;
   TST_ASSERT(Sink.Data == "Escaped Tag _Tag2");
   Sink.Data.clear();

   //End tags are similiar to start tags
   XmlStream << endTag << "Ta.g3" << delimiter << "Data" << newline;
   TST_ASSERT(Sink.Data == "</Tag3.Data>" + Newline);
   Sink.Data.clear();

   XmlStream << endTag << "8Ta$g2" << newline;
   TST_ASSERT(Sink.Data == "</_Tag2>" + Newline);
   Sink.Data.clear();

   XmlStream << endTag << "Tag1" << newline;
   TST_ASSERT(Sink.Data == "</Tag1>" + Newline);
   Sink.Data.clear();

   //flush will call flush on attached sink
   XmlStream << newline << flush;
   COL_TRC(Sink.Log);
}


void testXMLiosStream::testAttributeEscapes()
{
   {
      //attribute data should not escape value text
      COLstring Outvalue;
      COLsinkString Sink(Outvalue);
      XMLiosStream XmlStream( '.', &Sink );
      COLstring ValidText = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789`~!@#$%^*(){}|:>?/.,;\\][=-";
      XmlStream << attributeData << ValidText << flush;
      TST_ASSERT_MESSAGE( Outvalue == ValidText, "valid attribute text was escaped" );
   }

   {
      //test line feed escaping
      COLstring Outvalue;
      COLsinkString Sink(Outvalue);
      XMLiosStream XmlStream( '.', &Sink );
      COLstring Text = "Line one\r\nLine two\r\n";
      XmlStream << attributeData << Text << flush;
      TST_ASSERT_MESSAGE( Outvalue == "Line one&#x0D;&#x0A;Line two&#x0D;&#x0A;", "line feed escaping is not correct" );
   }

   {
      //test extra space escaping
      COLstring Outvalue;
      COLsinkString Sink(Outvalue);
      XMLiosStream XmlStream( '.', &Sink );
      COLstring Text = "sp   aces";
      XmlStream << attributeData << Text << flush;
      TST_ASSERT_MESSAGE( Outvalue == "sp &#x20;&#x20;aces", "space escaping is not correct" );
   }
   {
      //test mixed whitespace
      COLstring Outvalue;
      COLsinkString Sink(Outvalue);
      XMLiosStream XmlStream( '.', &Sink );
      COLstring Text = " \r  \t \n";
      XmlStream << attributeData << Text << flush;
      TST_ASSERT_MESSAGE( Outvalue == " &#x0D;&#x20;&#x20;&#x09;&#x20;&#x0A;", "space escaping is not correct" );
   }
}

void testXMLiosStream::testRangeEscape()
{
   COLstring String;
   COLsinkString Sink(String);
   XMLiosStream XmlStream('.',&Sink,false);
   XmlStream <<  " Unescaped: "<< (char)0x92 << newline;
   String.clear();
}

static COLstring testDataFilter(XMLiosDataFilter* pFilter /*takes ownership*/)
{
   COLstring Sink;
   COLsinkString RealSink(Sink);
   XMLiosStream XmlStream('.',&RealSink,false);

   //set to not escape any data
   XmlStream.setDataFilter(pFilter);

   XmlStream << startTag << "TestElement" << newline;

   //ensure that data is not escaped, written out as is, only if data is outside printable range
   //if not, things like &, and < > should still be escaped
   COLstring TestColstring("TestColstring");
   XmlStream << "Data for Tag 2 Int: " << 99 << " double: " << 9.999 << " Colstring: " << TestColstring << newline;
   XmlStream << "Escaped: "<< " & % \" < > ` " << newline;
   XmlStream << "Extended: ";

   //0 - 32 && 127 - 256 are escaped by calling XMLiosDataFilter::onDefaultEscape
   for (COLuint8 TestChar = 0; TestChar != 32; TestChar++)
   {
      XmlStream << TestChar;
   }
   for (COLuint8 TestChar = 127; TestChar != 0; TestChar++) //wrapping around
   {
      XmlStream << TestChar;
   }

   XmlStream << newline;
   XmlStream << endTag << "TestElement" << newline;
   XmlStream << newline << flush;

   return Sink;
}

void testXMLiosStream::testUnescapedData()
{
   {
      const COLstring TestWithEscape(testDataFilter(new XMLiosDataFilter()));
      COLstring ReferenceTest;
      COLostream RefStream(ReferenceTest);
      RefStream << "<TestElement>" << newline
                << "Data for Tag 2 Int: 99 double: 9.999000 Colstring: TestColstring" << newline
                << "Escaped:  &amp; % &quot; &lt; &gt; ` " << newline
                << "Extended: ";
      for (COLuint8 TestChar = 0; TestChar != 32; TestChar++)
      {
         RefStream << "&#x";
         FMTtoHex(RefStream,TestChar);
         RefStream << ";";
       }
      for (COLuint8 TestChar = 127; TestChar != 0; TestChar++) //wrapping around
      {
         RefStream << "&#x";
         FMTtoHex(RefStream,TestChar);
         RefStream << ";";
      }
      RefStream << newline << "</TestElement>" << newline << newline;

      TST_ASSERT( ReferenceTest == TestWithEscape )
   }
   {
      const COLstring TestWithEscape(testDataFilter(new XMLiosNullDataFilter()));
      COLstring ReferenceTest;
      COLostream RefStream(ReferenceTest);
      RefStream << "<TestElement>" << newline
                << "Data for Tag 2 Int: 99 double: 9.999000 Colstring: TestColstring" << newline
                << "Escaped:  &amp; % &quot; &lt; &gt; ` " << newline
                << "Extended: ";
      for (COLuint8 TestChar = 0; TestChar != 32; TestChar++)
      {
         RefStream << TestChar;
       }
      for (COLuint8 TestChar = 127; TestChar != 0; TestChar++) //wrapping around
      {
         RefStream << TestChar;
      }
      RefStream << newline << "</TestElement>" << newline << newline;

      TST_ASSERT( ReferenceTest == TestWithEscape )
   }
}


TSTtestable* testXMLiosStream::getTests()
{
  TST_START_FIXTURE(testXMLiosStream)
      TST_ADD_TEST_METHOD(basicExample);
      TST_ADD_TEST_METHOD(testAttributeEscapes);
      TST_ADD_TEST_METHOD(testRangeEscape);
      TST_ADD_TEST_METHOD(testUnescapedData)
  TST_END_FIXTURE
}
