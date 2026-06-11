//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOXTtestXml
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Thursday, September 30th, 2010 @ 12:22:40 PM
//
//---------------------------------------------------------------------------
#include "NOXTprecomp.h"
#pragma hdrstop

#include "NOXTtestXml.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <NODT/NODTutils.h>

#include <NOD/NODplace.h>
#include <NOD/NODoperation.h>

#include <NOX/NOXparse.h>
#include <NOX/NOXconstant.h>
#include <NOX/NOXtreeValue.h>

#include <FIL/FILfile.h>
#include <FIL/FILutils.h>

#include <COL/COLref.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static NODplace& NOXTgetNamedChildRepeat(NODplace& Place, const COLstring& ChildName, int RepeatIndex){
   return Place.child(NODgetNamedChildRepeat(Place, ChildName, RepeatIndex));
}

static COLstring NOXTtreeValue(const NODplace& Place){
   COLstring Result;
   COLostream ResultStream(Result);
   NOXtreeValue(ResultStream, Place, false);
   return Result;
}


#define NOXT_ASSERT_ANY_EXCEPTION(_Expression) \
   try{\
      _Expression;\
      TSTexception __Error("Expression did not throw.",__LINE__,__FILE__,TSTexception::eAssertionFailure);\
      throw __Error;\
   } catch (const COLerror& Error) {}


static NODplace* NOXTparseFile(const COLstring& FileName){
   COLstring FileContents;
   FILreadFile(FileName, &FileContents);
   return NOXparseString(FileContents);
}

static void NOXTtestParseEmptyXml()
{
   COLref<NODplace> pDocument = NOXparseString("");

   TST_ASSERT_EQUALS(NOX_XML, pDocument->protocolType());
   TST_ASSERT_EQUALS("xml", pDocument->protocolName());
   TST_ASSERT_EQUALS(false, pDocument->isNull());
   TST_ASSERT_EQUALS(true, pDocument->isComplex());
   TST_ASSERT_EQUALS(false, pDocument->isReadOnly());
   TST_ASSERT_EQUALS(NOX_DOCUMENT, pDocument->nodeType());
   TST_ASSERT_EQUALS("document", pDocument->nodeTypeName());
   TST_ASSERT_EQUALS("Document", pDocument->name());
   TST_ASSERT_EQUALS(0, pDocument->size());
}

static void NOXTtestReadOnly()
{
   COLref<NODplace> pDocument = NOXTparseFile("../NOXT/data/example.xml");

   TST_ASSERT_EQUALS(NOX_XML, pDocument->protocolType());
   TST_ASSERT_EQUALS("xml", pDocument->protocolName());
   TST_ASSERT_EQUALS(false, pDocument->isNull());
   TST_ASSERT_EQUALS(true, pDocument->isComplex());
   TST_ASSERT_EQUALS(false, pDocument->isReadOnly());
   TST_ASSERT_EQUALS(NOX_DOCUMENT, pDocument->nodeType());
   TST_ASSERT_EQUALS("document", pDocument->nodeTypeName());
   TST_ASSERT_EQUALS("Document", pDocument->name());
   TST_ASSERT_EQUALS(1, pDocument->size());
   
   // Take a look at the first element.
   NODplace& RootElement = pDocument->child(0);
   TST_ASSERT_EQUALS(false, RootElement.isNull());
   TST_ASSERT_EQUALS(true, RootElement.isComplex());
   TST_ASSERT_EQUALS(false, RootElement.isReadOnly());
   TST_ASSERT_EQUALS(NOX_ELEMENT, RootElement.nodeType());
   TST_ASSERT_EQUALS("element", RootElement.nodeTypeName());
   TST_ASSERT_EQUALS("example", RootElement.name());
   TST_ASSERT_EQUALS(5, RootElement.size());

   // Now index it by name.
   TST_ASSERT_EQUALS(&RootElement, &pDocument->child("example"));

   // Check the attribute of the root element.
   NODplace& RootAttribute = RootElement.child(0);
   TST_ASSERT_EQUALS(false, RootAttribute.isNull());
   TST_ASSERT_EQUALS(true, RootAttribute.isSimple());
   TST_ASSERT_EQUALS(NOX_ATTRIBUTE, RootAttribute.nodeType());
   TST_ASSERT_EQUALS("attribute", RootAttribute.nodeTypeName());
   TST_ASSERT_EQUALS(false, RootAttribute.isReadOnly());
   TST_ASSERT_EQUALS("patient", RootAttribute.name());
   TST_ASSERT_EQUALS("Senn", RootAttribute.value());
   TST_ASSERT_EQUALS("Senn", NOXTtreeValue(RootAttribute));
   
   // Index by name a node which does not exist.
   TST_ASSERT_EXCEPTION(RootElement.child("foo"), COLerror("'foo' member does not exist. Named members are patient, title, patient, patient, patient.", NOD_NO_SUCH_MEMBER));

   // Look at repeated names (1 attribute and 3 elements).
   TST_ASSERT_EQUALS(4, NODcountOfNamedChildRepeat(RootElement, "patient"));
   TST_ASSERT_EQUALS(NOX_ATTRIBUTE, NOXTgetNamedChildRepeat(RootElement, "patient", 0).nodeType());
   TST_ASSERT_EQUALS(NOX_ELEMENT, NOXTgetNamedChildRepeat(RootElement, "patient", 1).nodeType());
   TST_ASSERT_EQUALS(NOX_ELEMENT, NOXTgetNamedChildRepeat(RootElement, "patient", 2).nodeType());
   TST_ASSERT_EQUALS(NOX_ELEMENT, NOXTgetNamedChildRepeat(RootElement, "patient", 3).nodeType());

   // Check a text node.
   NODplace& TextNode = NOXTgetNamedChildRepeat(RootElement, "patient", 1).child(3).child(1).child(0);
   TST_ASSERT_EQUALS(false, TextNode.isNull());
   TST_ASSERT_EQUALS(true, TextNode.isSimple());
   TST_ASSERT_EQUALS(NOX_TEXT, TextNode.nodeType());
   TST_ASSERT_EQUALS("text", TextNode.nodeTypeName());
   TST_ASSERT_EQUALS("Text", TextNode.name());
   TST_ASSERT_EQUALS("Inconclusive <try again>", TextNode.value());
   TST_ASSERT_EQUALS("Inconclusive &lt;try again&gt;", NOXTtreeValue(TextNode));

   // Check a CDATA node.
   NODplace& CdataNode = NOXTgetNamedChildRepeat(RootElement, "patient", 3).child(4).child(1).child(1);
   TST_ASSERT_EQUALS(false, CdataNode.isNull());
   TST_ASSERT_EQUALS(true, CdataNode.isSimple());
   TST_ASSERT_EQUALS(NOX_CDATA, CdataNode.nodeType());
   TST_ASSERT_EQUALS("cdata", CdataNode.nodeTypeName());
   TST_ASSERT_EQUALS("CDATA", CdataNode.name());
   TST_ASSERT_EQUALS("COLarray<> is much faster now.", CdataNode.value());
   TST_ASSERT_EQUALS("<![CDATA[COLarray<> is much faster now.]]>", NOXTtreeValue(CdataNode));

   // Check that indexing by "Text" and "CDATA" doesn't work.
   TST_ASSERT_EXCEPTION(NOXTgetNamedChildRepeat(RootElement, "patient", 3).child(4).child(1).child("Text"), COLerror("'Text' member does not exist. ", NOD_NO_SUCH_MEMBER));
   TST_ASSERT_EXCEPTION(NOXTgetNamedChildRepeat(RootElement, "patient", 3).child(4).child(1).child("CDATA"), COLerror("'CDATA' member does not exist. ", NOD_NO_SUCH_MEMBER));
}

static void NOXTtestSetters()
{
   COLref<NODplace> pDocument = NOXTparseFile("../NOXT/data/example.xml");
   NODplace& RootElement = pDocument->child(0);

   // RootElement's second child is an element with a single text node.  Let's
   // change the text of that text node!
   TST_ASSERT_EQUALS("<title>Test Results</title>", NOXTtreeValue(RootElement.child(1)));
   TST_ASSERT_EQUALS(1, RootElement.child(1).size());
   RootElement.child(1).child(0).setValue("Hello World");
   TST_ASSERT_EQUALS("<title>Hello World</title>", NOXTtreeValue(RootElement.child(1)));
   RootElement.child(1).child(0).setTreeValue("Foo Bar");
   TST_ASSERT_EQUALS("<title>Foo Bar</title>", NOXTtreeValue(RootElement.child(1)));

   // Try setting a text node's inner XML with elements.  Should just be escaped.
   RootElement.child(1).child(0).setTreeValue("<b>fish & chips</b>");
   TST_ASSERT_EQUALS("<title>&lt;b&gt;fish &amp; chips&lt;/b&gt;</title>", NOXTtreeValue(RootElement.child(1)));

   // Now try setting the element's inner XML.
   RootElement.child(1).setTreeValue("Set via setTreeValue()");
   TST_ASSERT_EQUALS("<title>Set via setTreeValue()</title>", NOXTtreeValue(RootElement.child(1)));

   // Now let's get a little crazier and throw some elements in there.
   RootElement.child(1).setTreeValue("<tag attr=\"value\">inner text</tag><br />");
   COLstring ExpectedValue("<title>\n   <tag attr=\"value\">inner text</tag>\n   <br></br>\n</title>");
   TST_ASSERT_EQUALS(ExpectedValue, NOXTtreeValue(RootElement.child(1)));
   TST_ASSERT_EQUALS(2, RootElement.child(1).size());
   {
      NODplace& Tag = RootElement.child(1).child(0);
      NODplace& Br = RootElement.child(1).child(1);
      TST_ASSERT_EQUALS(2, Tag.size());
      TST_ASSERT_EQUALS("value", Tag.child(0).value());
      TST_ASSERT_EQUALS("inner text", Tag.child(1).value());
      TST_ASSERT_EQUALS(0, Br.size());
      TST_ASSERT_EQUALS(true, Br.isComplex());
   }

   // Now let's clear it.
   RootElement.child(1).setTreeValue("");
   TST_ASSERT_EQUALS("<title></title>", NOXTtreeValue(RootElement.child(1)));
   TST_ASSERT_EQUALS(0, RootElement.child(1).size());

   // Now let's play around with setting an attribute.
   NODplace& Nasron = RootElement.child(4);
   NODplace& GeniusTest = Nasron.child(3);
   ExpectedValue = "<test name=\"genius\">\n   <result>Positive</result>\n</test>";
   TST_ASSERT_EQUALS(ExpectedValue, NOXTtreeValue(GeniusTest));
   GeniusTest.child(0).setValue("super-genius");
   ExpectedValue = "<test name=\"super-genius\">\n   <result>Positive</result>\n</test>";
   TST_ASSERT_EQUALS(ExpectedValue, NOXTtreeValue(GeniusTest));
   TST_ASSERT_EQUALS("super-genius", GeniusTest.child(0).value());
   TST_ASSERT_EQUALS("super-genius", NOXTtreeValue(GeniusTest.child(0)));
   GeniusTest.child(0).setTreeValue("\"ultra\" genius");
   ExpectedValue = "<test name=\"&quot;ultra&quot; genius\">\n   <result>Positive</result>\n</test>";
   TST_ASSERT_EQUALS(ExpectedValue, NOXTtreeValue(GeniusTest));
   ExpectedValue = "\"ultra\" genius";
   TST_ASSERT_EQUALS(ExpectedValue, GeniusTest.child(0).value());
   TST_ASSERT_EQUALS("&quot;ultra&quot; genius", NOXTtreeValue(GeniusTest.child(0)));

   // Test setting bad XML.
   // Unclosed tag.
   NOXT_ASSERT_ANY_EXCEPTION(Nasron.setTreeValue("<open>Tag is opened, but not closed!"));
   // Really messed up.
   NOXT_ASSERT_ANY_EXCEPTION(Nasron.setTreeValue("< this is bad & I'm breaking < so many xml rules here >>"));

   // Test setting the entire document's XML.
   pDocument->setTreeValue("<root><message>What a waste, I just wiped out the entire document.</message></root>");
   TST_ASSERT_EQUALS(1, pDocument->size());
   TST_ASSERT_EQUALS(1, pDocument->child(0).size());
   TST_ASSERT_EQUALS(1, pDocument->child(0).child(0).size());
   TST_ASSERT_EQUALS("What a waste, I just wiped out the entire document.", pDocument->child(0).child(0).child(0).value());
   TST_ASSERT_EQUALS("<root>\n   <message>What a waste, I just wiped out the entire document.</message>\n</root>", NOXTtreeValue(*pDocument));
   // A document can't have two root elements.
   NOXT_ASSERT_ANY_EXCEPTION(pDocument->setTreeValue("<root>Root 1</root><root>Root 2</root>"));
}

static void NOXTtestAdd()
{
   COLref<NODplace> pDocument = NOXparseString("<root><elem1 attr1=\"foo\">Some Text</elem1></root>");
   NODplace& RootElement = pDocument->child(0);
   
   // Test error conditions.
   TST_ASSERT_EXCEPTION(RootElement.add(NULL, NULL), COLerror("type, name/data", NOD_BAD_ARGUMENTS));
   TST_ASSERT_EXCEPTION(RootElement.add("foo", NULL), COLerror("Unrecognized node type 'foo'.  Supported types are 'element', 'attribute', 'cdata', and 'text'.", COLerror::PreCondition));
   TST_ASSERT_EXCEPTION(RootElement.add("element", NULL), COLerror("type, name", NOD_BAD_ARGUMENTS));
   TST_ASSERT_EXCEPTION(RootElement.add("attribute", NULL), COLerror("type, name", NOD_BAD_ARGUMENTS));
   TST_ASSERT_EXCEPTION(RootElement.add("cdata", NULL), COLerror("type, data", NOD_BAD_ARGUMENTS));
   TST_ASSERT_EXCEPTION(RootElement.add("text", NULL), COLerror("type, text", NOD_BAD_ARGUMENTS));
   TST_ASSERT_EXCEPTION(pDocument->add("element", "foo"), COLerror("Only element nodes can have children added to them.", COLerror::PreCondition));
   
   // Add an attribute.
   TST_ASSERT_EQUALS(NOX_ELEMENT, RootElement.child(0).nodeType());
   RootElement.add("attribute", "title");
   TST_ASSERT_EQUALS(2, RootElement.size());
   TST_ASSERT_EQUALS(NOX_ATTRIBUTE, RootElement.child(0).nodeType()); // attributes come before elements
   RootElement.child(0).setValue("An XML Document");
   COLstring ExpectedValue("<root title=\"An XML Document\">\n   <elem1 attr1=\"foo\">Some Text</elem1>\n</root>");
   TST_ASSERT_EQUALS(ExpectedValue, NOXTtreeValue(*pDocument));

   // Add an element.
   NODplace& Elem2 = RootElement.add("element", "elem2");
   TST_ASSERT_EQUALS(3, RootElement.size());
   TST_ASSERT_EQUALS(NOX_ELEMENT, RootElement.child(1).nodeType());
   ExpectedValue = "<root title=\"An XML Document\">\n   <elem1 attr1=\"foo\">Some Text</elem1>\n   <elem2></elem2>\n</root>";
   TST_ASSERT_EQUALS(ExpectedValue, NOXTtreeValue(*pDocument));

   // Add some text to the newly added element.
   Elem2.add("text", "Some More Text");
   TST_ASSERT_EQUALS(1, Elem2.size());
   TST_ASSERT_EQUALS(NOX_TEXT, Elem2.child(0).nodeType());
   TST_ASSERT_EQUALS("<elem2>Some More Text</elem2>", NOXTtreeValue(Elem2));

   // Add a third element to the root, and add a CDATA section inside.
   NODplace& Elem3 = RootElement.add("element", "elem3");
   Elem3.add("cdata", "this is a <cdata> section.");
   TST_ASSERT_EQUALS(1, Elem3.size());
   TST_ASSERT_EQUALS(NOX_CDATA, Elem3.child(0).nodeType());
   TST_ASSERT_EQUALS("<elem3><![CDATA[this is a <cdata> section.]]></elem3>", NOXTtreeValue(Elem3));

   // Ensure that name-based list is updated if a new node with the same name is added.
   TST_ASSERT_EQUALS(1, NODcountOfNamedChildRepeat(RootElement, "title"));
   TST_ASSERT_EQUALS(NOX_ATTRIBUTE, RootElement.child("title").nodeType());
   RootElement.add("element", "title");
   TST_ASSERT_EQUALS(2, NODcountOfNamedChildRepeat(RootElement, "title"));
   TST_ASSERT_EQUALS(NOX_ATTRIBUTE, RootElement.child("title").nodeType());
   TST_ASSERT_EQUALS(NOX_ELEMENT, NOXTgetNamedChildRepeat(RootElement, "title", 1).nodeType());

   // Add a duplicate attribute (this isn't really standard XML, but we'll allow it).
   RootElement.add("attribute", "title");
   TST_ASSERT_EQUALS(6, RootElement.size());
   TST_ASSERT_EQUALS(NOX_ATTRIBUTE, RootElement.child(0).nodeType());
   TST_ASSERT_EQUALS("title", RootElement.child(0).name());
   TST_ASSERT_EQUALS("An XML Document", RootElement.child(0).value());
   TST_ASSERT_EQUALS(NOX_ATTRIBUTE, RootElement.child(1).nodeType());
   TST_ASSERT_EQUALS("title", RootElement.child(1).name());
   TST_ASSERT_EQUALS("", RootElement.child(1).value());
}

static void NOXTtestInsert()
{
   COLref<NODplace> pDocument = NOXparseString("<root><elem1 attr1=\"foo\">Some Text</elem1></root>");
   NODplace& RootElement = pDocument->child(0);

   // Test error conditions.
   TST_ASSERT_EXCEPTION(RootElement.insert(0, NULL, NULL), COLerror("type, name/data", NOD_BAD_ARGUMENTS));
   TST_ASSERT_EXCEPTION(RootElement.insert(0, "foo", NULL), COLerror("Unrecognized node type 'foo'.  Supported types are 'element', 'attribute', 'cdata', and 'text'.", COLerror::PreCondition));
   TST_ASSERT_EXCEPTION(RootElement.insert(0, "element", NULL), COLerror("type, name", NOD_BAD_ARGUMENTS));
   TST_ASSERT_EXCEPTION(RootElement.insert(0, "attribute", NULL), COLerror("type, name", NOD_BAD_ARGUMENTS));
   TST_ASSERT_EXCEPTION(RootElement.insert(0, "cdata", NULL), COLerror("type, data", NOD_BAD_ARGUMENTS));
   TST_ASSERT_EXCEPTION(RootElement.insert(0, "text", NULL), COLerror("type, text", NOD_BAD_ARGUMENTS));
   TST_ASSERT_EXCEPTION(pDocument->insert(0, "element", "foo"), COLerror("Only element nodes can have children added to them.", COLerror::PreCondition));
   TST_ASSERT_EXCEPTION(RootElement.insert(-1, "element", "foo"), COLerror("Index -1 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(RootElement.insert(3, "element", "foo"), COLerror("Index 3 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   // Try to insert an element before an attribute.
   TST_ASSERT_EXCEPTION(RootElement.child(0).insert(0, "element", "foo"), COLerror("Index 0 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   // Try to insert an attribute after an element.
   TST_ASSERT_EXCEPTION(RootElement.insert(1, "attribute", "foo"), COLerror("Index 1 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));

   // Insert some elements.
   NODplace& Elem3 = RootElement.insert(1, "element", "elem3");
   NODplace& Elem2 = RootElement.insert(1, "element", "elem2");
   TST_ASSERT_EQUALS(3, RootElement.size());
   TST_ASSERT_EQUALS("elem1", RootElement.child(0).name());
   TST_ASSERT_EQUALS("elem2", RootElement.child(1).name());
   TST_ASSERT_EQUALS("elem3", RootElement.child(2).name());

   // Insert some elements in an element which has an attribute.
   NODplace& Elem1 = RootElement.child(0);
   Elem1.insert(1, "element", "foo");
   Elem1.insert(3, "element", "bar");
   TST_ASSERT_EQUALS(4, Elem1.size());
   COLstring ExpectedValue("<elem1 attr1=\"foo\">\n   <foo></foo>\n   Some Text\n   <bar></bar>\n</elem1>");
   TST_ASSERT_EQUALS(ExpectedValue, NOXTtreeValue(Elem1));

   // Insert some attributes.
   NODplace& TitleAttr = RootElement.insert(0, "attribute", "title");
   TitleAttr.setValue("Test XML Document");
   TST_ASSERT_EQUALS(4, RootElement.size());
   TST_ASSERT_EQUALS(NOX_ATTRIBUTE, RootElement.child(0).nodeType());
   TST_ASSERT_EQUALS("title", RootElement.child(0).name());
   TST_ASSERT_EQUALS("Test XML Document", RootElement.child(0).value());
   TST_ASSERT_EQUALS("elem1", RootElement.child(1).name());
   TST_ASSERT_EQUALS("elem2", RootElement.child(2).name());
   TST_ASSERT_EQUALS("elem3", RootElement.child(3).name());
   NODplace& VersionAttr = RootElement.insert(0, "attribute", "version");
   VersionAttr.setValue("1.0");
   TST_ASSERT_EQUALS(5, RootElement.size());
   TST_ASSERT_EQUALS(NOX_ATTRIBUTE, RootElement.child(0).nodeType());
   TST_ASSERT_EQUALS("version", RootElement.child(0).name());
   TST_ASSERT_EQUALS("1.0", RootElement.child(0).value());
   TST_ASSERT_EQUALS(NOX_ATTRIBUTE, RootElement.child(1).nodeType());
   TST_ASSERT_EQUALS("title", RootElement.child(1).name());
   TST_ASSERT_EQUALS("Test XML Document", RootElement.child(1).value());
   TST_ASSERT_EQUALS("elem1", RootElement.child(2).name());
   TST_ASSERT_EQUALS("elem2", RootElement.child(3).name());
   TST_ASSERT_EQUALS("elem3", RootElement.child(4).name());

   // Insert some text.
   Elem2.insert(0, "text", "Hello world");
   TST_ASSERT_EQUALS(1, Elem2.size());
   TST_ASSERT_EQUALS(NOX_TEXT, Elem2.child(0).nodeType());
   TST_ASSERT_EQUALS("<elem2>Hello world</elem2>", NOXTtreeValue(Elem2));
   Elem2.insert(0, "text", "First line");
   TST_ASSERT_EQUALS(2, Elem2.size());
   TST_ASSERT_EQUALS(NOX_TEXT, Elem2.child(0).nodeType());
   TST_ASSERT_EQUALS("First line", Elem2.child(0).value());
   TST_ASSERT_EQUALS("Text", Elem2.child(0).name());
   TST_ASSERT_EQUALS("<elem2>\n   First line\n   Hello world\n</elem2>", NOXTtreeValue(Elem2));
   TST_ASSERT_EQUALS(NOX_TEXT, Elem2.child(1).nodeType());
   TST_ASSERT_EQUALS("Hello world", Elem2.child(1).value());

   // Insert some text in an element that has an attribute.
   RootElement.insert(2, "text", "This text is sort of floating around in the document.");
   TST_ASSERT_EQUALS(6, RootElement.size());
   TST_ASSERT_EQUALS(NOX_TEXT, RootElement.child(2).nodeType());
   TST_ASSERT_EQUALS("This text is sort of floating around in the document.", RootElement.child(2).value());

   // Insert a CDATA section.
   Elem3.insert(0, "cdata", "Some cdata");
   TST_ASSERT_EQUALS(1, Elem3.size());
   TST_ASSERT_EQUALS("<elem3><![CDATA[Some cdata]]></elem3>", NOXTtreeValue(Elem3));

   // Ensure that getting repeats works after insertions.
   TST_ASSERT_EQUALS(1, NODcountOfNamedChildRepeat(RootElement, "title"));
   RootElement.insert(2, "element", "title");
   RootElement.insert(7, "element", "title");
   TST_ASSERT_EQUALS(8, RootElement.size());
   TST_ASSERT_EQUALS(NOX_ATTRIBUTE, RootElement.child(0).nodeType());
   TST_ASSERT_EQUALS(NOX_ATTRIBUTE, RootElement.child(1).nodeType());
   TST_ASSERT_EQUALS(NOX_ELEMENT, RootElement.child(2).nodeType());
   TST_ASSERT_EQUALS(NOX_ELEMENT, RootElement.child(7).nodeType());
   TST_ASSERT_EQUALS(3, NODcountOfNamedChildRepeat(RootElement, "title"));
   TST_ASSERT_EQUALS(NOX_ATTRIBUTE, NOXTgetNamedChildRepeat(RootElement, "title", 0).nodeType());
   TST_ASSERT_EQUALS(NOX_ELEMENT, NOXTgetNamedChildRepeat(RootElement, "title", 1).nodeType());
   TST_ASSERT_EQUALS(NOX_ELEMENT, NOXTgetNamedChildRepeat(RootElement, "title", 2).nodeType());
}

static void NOXTtestRemoveAttributeByName()
{
   COLref<NODplace> pDocument = NOXparseString("<root attr1=\"foo\" attr2=\"bar\"><elem1></elem1></root>");
   NODplace& RootElement = pDocument->child(0);
   TST_ASSERT_EQUALS(3, RootElement.size());
   RootElement.remove("attr1");
   TST_ASSERT_EQUALS(2, RootElement.size());
   COLstring ExpectedValue("<root attr2=\"bar\">\n   <elem1></elem1>\n</root>");
   TST_ASSERT_EQUALS(ExpectedValue, NOXTtreeValue(*pDocument));

   TST_ASSERT_EXCEPTION(RootElement.remove("attr1"), COLerror("'attr1' member does not exist. Named members are attr2, elem1.", NOD_NO_SUCH_MEMBER));
}

static void NOXTtestRemoveAttributeByIndex()
{
   COLref<NODplace> pDocument = NOXparseString("<root attr1=\"foo\" attr2=\"bar\" attr3=\"blah\">Some Text</root>");
   NODplace& RootElement = pDocument->child(0);
   TST_ASSERT_EQUALS(4, RootElement.size());
   TST_ASSERT_EQUALS("foo", RootElement.child(0).value());
   TST_ASSERT_EQUALS("bar", RootElement.child(1).value());
   TST_ASSERT_EQUALS("blah", RootElement.child(2).value());
   TST_ASSERT_EQUALS("Some Text", RootElement.child(3).value());

   TST_ASSERT_EXCEPTION(RootElement.remove(-1), COLerror("Index -1 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(RootElement.remove(4), COLerror("Index 4 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));

   RootElement.remove(0);
   TST_ASSERT_EQUALS(3, RootElement.size());
   TST_ASSERT_EQUALS("bar", RootElement.child(0).value());
   TST_ASSERT_EQUALS("blah", RootElement.child(1).value());
   TST_ASSERT_EQUALS("Some Text", RootElement.child(2).value());
   COLstring ExpectedValue("<root attr2=\"bar\" attr3=\"blah\">Some Text</root>");
   TST_ASSERT_EQUALS(ExpectedValue, NOXTtreeValue(RootElement));

   RootElement.remove(1);
   TST_ASSERT_EQUALS(2, RootElement.size());
   TST_ASSERT_EQUALS("bar", RootElement.child(0).value());
   TST_ASSERT_EQUALS("Some Text", RootElement.child(1).value());
   ExpectedValue = "<root attr2=\"bar\">Some Text</root>";
   TST_ASSERT_EQUALS(ExpectedValue, NOXTtreeValue(RootElement));
}

static void NOXTtestRemoveDomNodeByName()
{
   COLref<NODplace> pDocument = NOXparseString("<Root>Some Text<Text>foo</Text><Text>bar</Text></Root>");
   NODplace& RootElement = pDocument->child(0);
   TST_ASSERT_EQUALS("Text", RootElement.child(0).name()); // the name of the text node.
   RootElement.remove("Text"); // should remove element, not the text node.
   RootElement.remove("Text"); // should remove element, not the text node.
   TST_ASSERT_EQUALS(1, RootElement.size());
   TST_ASSERT_EQUALS("<Root>Some Text</Root>", NOXTtreeValue(RootElement));

   TST_ASSERT_EXCEPTION(RootElement.remove("Text"), COLerror("'Text' member does not exist. ", NOD_NO_SUCH_MEMBER));
}

static void NOXTtestRemoveDomNodeByIndex()
{
   COLref<NODplace> pDocument = NOXparseString("<Root attr1=\"foo\">Some Text<Text>foo</Text><Text>bar</Text></Root>");
   NODplace& RootElement = pDocument->child(0);
   TST_ASSERT_EQUALS(4, RootElement.size());

   RootElement.remove(1);
   TST_ASSERT_EQUALS(3, RootElement.size());
   COLstring ExpectedValue("<Root attr1=\"foo\">\n   <Text>foo</Text>\n   <Text>bar</Text>\n</Root>");
   TST_ASSERT_EQUALS(ExpectedValue, NOXTtreeValue(RootElement));

   RootElement.remove(2);
   TST_ASSERT_EQUALS(2, RootElement.size());
   ExpectedValue = "<Root attr1=\"foo\">\n   <Text>foo</Text>\n</Root>";
   TST_ASSERT_EQUALS(ExpectedValue, NOXTtreeValue(RootElement));

   RootElement.remove(1);
   TST_ASSERT_EQUALS(1, RootElement.size());
   ExpectedValue = "<Root attr1=\"foo\"></Root>";
   TST_ASSERT_EQUALS(ExpectedValue, NOXTtreeValue(RootElement));
}

static void NOXTtestRemoveAttributeAndElementBySameName()
{
   COLref<NODplace> pDocument = NOXparseString("<root value=\"foo\"><value>bar</value></root>");
   NODplace& RootElement = pDocument->child(0);
   TST_ASSERT_EQUALS(2, RootElement.size());
   RootElement.remove("value"); // should remove attribute
   TST_ASSERT_EQUALS(1, RootElement.size());
   TST_ASSERT_EQUALS("<root>\n   <value>bar</value>\n</root>", NOXTtreeValue(RootElement));
   RootElement.remove("value"); // should remove element
   TST_ASSERT_EQUALS(0, RootElement.size());
   TST_ASSERT_EQUALS("<root></root>", NOXTtreeValue(RootElement));
}

static void NOXTtestNullify()
{
   // We don't need to test this much - the implementation just calls remove().
   // We just want to make sure it actually works.
   COLref<NODplace> pDocument = NOXparseString("<root attr=\"foo\"><elem>bar</elem></root>");
   NODplace& RootElement = pDocument->child(0);

   RootElement.nullify("attr");
   TST_ASSERT_EQUALS(1, RootElement.size());
   TST_ASSERT_EQUALS("<root>\n   <elem>bar</elem>\n</root>", NOXTtreeValue(RootElement));

   RootElement.nullify(0);
   TST_ASSERT_EQUALS(0, RootElement.size());
   TST_ASSERT_EQUALS("<root></root>", NOXTtreeValue(RootElement));
}

static void NOXTtestClear()
{
   COLref<NODplace> pDocument = NOXparseString("<root attr=\"foo\"><elem>bar</elem></root>");
   NODplace& RootElement = pDocument->child(0);

   RootElement.clear();
   TST_ASSERT_EQUALS(0, RootElement.size());
   TST_ASSERT_EQUALS("<root></root>", NOXTtreeValue(RootElement));

   TST_ASSERT_EXCEPTION(RootElement.child("attr"), COLerror("'attr' member does not exist. ", NOD_NO_SUCH_MEMBER));
}

static void NOXTtestSetTreeValueWithAttribute()
{
   COLref<NODplace> pDocument = NOXparseString("<root attr=\"foo\"></root>");
   NODplace& RootElement = pDocument->child(0);

   RootElement.setTreeValue("<elem id=\"1\"></elem><elem id=\"2\"></elem>");
   TST_ASSERT_EQUALS("attr", RootElement.child(0).name()); // first child should still be attribute.
   TST_ASSERT_EQUALS(3, RootElement.size()); // 4 == 1 attribute + empty text node + 2 child elements.
}

TSTtestable* NOXTtestXml()
{
   TST_START_COLLECTION("NOXTtestXml")
      TST_ADD_TEST_FUNCTION(NOXTtestParseEmptyXml);
      TST_ADD_TEST_FUNCTION(NOXTtestReadOnly);
      TST_ADD_TEST_FUNCTION(NOXTtestSetters);
      TST_ADD_TEST_FUNCTION(NOXTtestAdd);
      TST_ADD_TEST_FUNCTION(NOXTtestInsert);
      TST_ADD_TEST_FUNCTION(NOXTtestRemoveAttributeByName);
      TST_ADD_TEST_FUNCTION(NOXTtestRemoveAttributeByIndex);
      TST_ADD_TEST_FUNCTION(NOXTtestRemoveDomNodeByName);
      TST_ADD_TEST_FUNCTION(NOXTtestRemoveDomNodeByIndex);
      TST_ADD_TEST_FUNCTION(NOXTtestRemoveAttributeAndElementBySameName);
      TST_ADD_TEST_FUNCTION(NOXTtestNullify);
      TST_ADD_TEST_FUNCTION(NOXTtestClear);
      TST_ADD_TEST_FUNCTION(NOXTtestSetTreeValueWithAttribute);
   TST_END_FIXTURE
}
