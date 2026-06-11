//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Tests for XMLexpatAnsiParser
//
// Author: Nasron Cheong
// Date:   June 2005
//
//---------------------------------------------------------------------------

#include <XML/XMLexpatAnsiParser.h>

#include <COL/COLlog.h>
#include <COL/COLstring.h>
#include <LEG/LEGvector.h>
#include <COL/COLsinkStandard.h>
#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>
#include <FMT/FMTbinaryDump.h>
#include <XML/XMLptrArray.h>
COL_LOG_MODULE;

class XMLexpatAnsiParserTestAttribute
{
public:
   XMLexpatAnsiParserTestAttribute(){};
   XMLexpatAnsiParserTestAttribute(const XMLexpatAnsiParserTestAttribute& Orig)
   {
      Key = Orig.Key;
      Value = Orig.Value;
   }
   XMLexpatAnsiParserTestAttribute& operator=(const XMLexpatAnsiParserTestAttribute& Orig)
   {
      Key = Orig.Key;
      Value = Orig.Value;
      return *this;
   }
   ~XMLexpatAnsiParserTestAttribute(){}

   COLstring Key;
   COLstring Value;
};

class XMLexpatAnsiParserTest : public XMLexpatAnsiParser
{
public:

   COLstring ExpectedStartElement;
   COLstring ExpectedEndElement;
   COLstring CurrentData;

   LEGvector<XMLexpatAnsiParserTestAttribute> ExpectedAttributes;

   virtual void onStartElement(const char* Name, const char** attr)
   {
      TST_ASSERT(ExpectedStartElement == Name);
      LEGvector<const char*> KeyArray;
      LEGvector<const char*> ValueArray;
      int KeyIndex = 0;
      while (*attr)
      {
         if (KeyIndex % 2 == 0)
         {
            KeyArray.add(*attr);
         }
         else
         {
            ValueArray.add(*attr);
         }
         KeyIndex++;
         attr++;
      }
      TST_ASSERT(KeyArray.size() == ValueArray.size());
      TST_ASSERT(KeyArray.size() == ExpectedAttributes.size());
      for (KeyIndex = 0; KeyIndex < KeyArray.size(); KeyIndex++)
      {
         COL_TRC(ExpectedAttributes[KeyIndex].Key << " vs " << KeyArray[KeyIndex]);
         COL_TRC(ExpectedAttributes[KeyIndex].Value << " vs " << ValueArray[KeyIndex]);

         TST_ASSERT(ExpectedAttributes[KeyIndex].Key == KeyArray[KeyIndex])
         TST_ASSERT(ExpectedAttributes[KeyIndex].Value == ValueArray[KeyIndex])
      }
   }

   virtual void onEndElement(const char* Name)
   {
      TST_ASSERT(ExpectedStartElement == Name);
   }

   virtual void onCharacterData(const char* Data, int Length)
   {
      CurrentData.write(Data,Length);      
   }

   static COLstring ansiToUtf8(COLuint8 AnsiChar, COLuint32 ByteRepresentation /*2,3,4*/)
   {
      if (AnsiChar < 128)
      {
         return COLstring(1, AnsiChar);
      }
      else
      {
         switch(ByteRepresentation)
         {
         case 2:
            //110vvvvv 10vvvvvv
            {
               COLuint8 Output[2];
               //second byte
               Output[1] = AnsiChar & (0x3F); //and with 00111111, to get 00vvvvvv
               Output[1] |= (0x80); //or with 10000000, to get 10vvvvvv
               Output[0] = AnsiChar;
               Output[0] >>= 6; //get last 2 bytes to appear as first 2, 000000vv
               Output[0] |= 0xC0; //gives 110000vv
               COLstring OutputStr;
               OutputStr.write(Output,sizeof(Output));
               return OutputStr;
            }
            break;
         case 3:
            // 1110vvvv 10vvvvvv 10vvvvvv
            {
               COLuint8 Output[3];
               //third byte
               Output[2] = AnsiChar & (0x3F); //and with 00111111, to get 00vvvvvv
               Output[2] |= (0x80); //or with 10000000, to get 10vvvvvv
               Output[1] = AnsiChar;
               Output[1] >>= 6; //get last 2 bytes to appear as first 2, 000000vv
               Output[1] |= 0x80; //gives 100000vv
               Output[0] = 0xE0; //11100000
               COLstring OutputStr;
               OutputStr.write(Output,sizeof(Output));
               return OutputStr;               
            }
            break;
         case 4:
            //11110vvv 10vvvvvv 10vvvvvv 10vvvvvv
            {
               COLuint8 Output[4];
               //fourth byte
               Output[3] = AnsiChar & (0x3F); //and with 00111111, to get 00vvvvvv
               Output[3] |= (0x80); //or with 10000000, to get 10vvvvvv
               Output[2] = AnsiChar;
               Output[2] >>= 6; //get last 2 bytes to appear as first 2, 000000vv
               Output[2] |= 0x80; //gives 100000vv
               Output[1] = 0x80; //10000000
               Output[0] = 0xF0; //11110000
               COLstring OutputStr;
               OutputStr.write(Output,sizeof(Output));
               return OutputStr;               
            }
            break;
         default:
            COL_ERROR_STREAM("Invalid Byte representation " << ByteRepresentation,0);
         }
      }
   }

   static void testAnsiCharConversion()
   {
      {
         //test the ansiToUtf8 here
         COLstring UtfVal1 = ansiToUtf8(194,2);
         COLstring UtfVal1Compare;
         COLuint8 Compare[2];
         Compare[0] = 0xC3;
         Compare[1] = 0x82;
         UtfVal1Compare.write(Compare,sizeof(Compare));

         COLstring Log;
         COLostream LogStream(Log);
         FMTbinaryDump(LogStream,(const COLuint8*)UtfVal1.c_str(),UtfVal1.size());
         COL_TRC(Log);
         Log.clear();
         FMTbinaryDump(LogStream,(const COLuint8*)UtfVal1Compare.c_str(),UtfVal1Compare.size());
         COL_TRC(Log);

         TST_ASSERT(UtfVal1 == UtfVal1Compare);
      }
      
      {
         //test the ansiToUtf8 here
         COLstring UtfVal1 = ansiToUtf8(194,3);
         COLstring UtfVal1Compare;
         COLuint8 Compare[3];
         Compare[0] = 0xE0;
         Compare[1] = 0x83;
         Compare[2] = 0x82;
         UtfVal1Compare.write(Compare,sizeof(Compare));

         COLstring Log;
         COLostream LogStream(Log);
         FMTbinaryDump(LogStream,(const COLuint8*)UtfVal1.c_str(),UtfVal1.size());
         COL_TRC(Log);
         Log.clear();
         FMTbinaryDump(LogStream,(const COLuint8*)UtfVal1Compare.c_str(),UtfVal1Compare.size());
         COL_TRC(Log);

         TST_ASSERT(UtfVal1 == UtfVal1Compare);
      }
      
      {
         //test the ansiToUtf8 here
         COLstring UtfVal1 = ansiToUtf8(194,4);
         COLstring UtfVal1Compare;
         COLuint8 Compare[4];
         Compare[0] = 0xF0;
         Compare[1] = 0x80;
         Compare[2] = 0x83;
         Compare[3] = 0x82;
         UtfVal1Compare.write(Compare,sizeof(Compare));

         COLstring Log;
         COLostream LogStream(Log);
         FMTbinaryDump(LogStream,(const COLuint8*)UtfVal1.c_str(),UtfVal1.size());
         COL_TRC(Log);
         Log.clear();
         FMTbinaryDump(LogStream,(const COLuint8*)UtfVal1Compare.c_str(),UtfVal1Compare.size());
         COL_TRC(Log);

         TST_ASSERT(UtfVal1 == UtfVal1Compare);
      }

      {
         XMLexpatAnsiParserTest Parser;

         //test all chars from 0 - 127
         COLuint8 Input = 1;
         for (Input = 1; Input < 128; Input++)
         {
            COLuint8 InputStr[2];
            InputStr[0] = Input;
            InputStr[1] = '\0';
            COLuint8 Output;
            TST_ASSERT( Parser.utf8CharToAnsiChar(reinterpret_cast<const COLuint8*>(InputStr),&Output) == 1);
            TST_ASSERT(Output == Input);
         }

         //test all chars from 128 - 255
         for (Input = 128; Input != 0; Input++)
         {
            //test every possible numbyte encoding
            for (COLuint32 ByteRepresentation = 2; ByteRepresentation <= 4; ByteRepresentation++ )
            {
               COLstring Utf8Encoded = ansiToUtf8(Input,ByteRepresentation);
               COLuint8 Output;
               TST_ASSERT( Parser.utf8CharToAnsiChar(reinterpret_cast<const COLuint8*>(Utf8Encoded.c_str()),&Output) == ByteRepresentation);
               TST_ASSERT(Output == Input);               
            }
         }

      }
      
      {
         XMLexpatAnsiParserTest Parser;

         COLstring InputUtf8 = "Driver";
         COLuint8 UtfSegment[4];
         UtfSegment[0] = 195;
         UtfSegment[1] = 130;
         UtfSegment[2] = 194;
         UtfSegment[3] = 180;         
         InputUtf8.write(UtfSegment,sizeof(UtfSegment));
         InputUtf8 += "s License Number";

         COLstring CompareString = "Driver";
         COLuint8 AnsiSegment[2];
         AnsiSegment[0] = 194;
         AnsiSegment[1] = 180;
         CompareString.write(AnsiSegment,sizeof(AnsiSegment));
         CompareString += "s License Number";

         COLstring Output = Parser.utf8ToAnsi(InputUtf8.c_str());
         TST_ASSERT(CompareString == Output);
      }

      {
         XMLexpatAnsiParserTest Parser;
         XMLptrArray<const char> PtrArray;
         COLuint8 UtfSegment[5];
         UtfSegment[0] = 195;
         UtfSegment[1] = 130;
         UtfSegment[2] = 194;
         UtfSegment[3] = 180;         
         UtfSegment[4] = 0;  
         
         TST_ASSERT(strlen((const char*)UtfSegment) == 4)
         
         COLuint8 UtfSegment2[5];
         UtfSegment2[0] = 195;
         UtfSegment2[1] = 130;
         UtfSegment2[2] = 194;
         UtfSegment2[3] = 180;
         UtfSegment2[4] = 0;   

         PtrArray.add((const char*)(UtfSegment));
         PtrArray.add((const char*)(UtfSegment2));

         XMLptrArray<const char> PtrCompareArray;
         
         COLuint8 AnsiSegment[3];
         AnsiSegment[0] = 194;
         AnsiSegment[1] = 180;
         AnsiSegment[2] = 0;

         COLuint8 AnsiSegment2[3];
         AnsiSegment2[0] = 194;
         AnsiSegment2[1] = 180;
         AnsiSegment2[2] = 0;

         PtrCompareArray.add((const char*)AnsiSegment);
         PtrCompareArray.add((const char*)AnsiSegment2);
         
         const char** pCompareAttr = PtrCompareArray.array();
         const char** pAttr = Parser.utf8PtrArrayToAnsi(PtrArray.array());

         COLuint32 OutputSize = 0;
         while (*pAttr != 0)
         {
            TST_ASSERT(*pCompareAttr != 0)
            TST_ASSERT( strcmp(*pAttr,*pCompareAttr) == 0)
            OutputSize++;
            pAttr++;
            pCompareAttr++;
         }         
         TST_ASSERT(*pCompareAttr == 0)
      }
   }

   static void testXmlStream()
   {
      // with some hardcoded xml stream data, ensure that we get the correct data
      // we don't check for countOfStartElements == countOfEndElements because
      // we assume expat is right.
      
      //test readable ascii ( < 127)
      {
         COLstring Input = "<DOCUMENT Attr=\"hello\" Attr2=\"hello2\">DATA</DOCUMENT>";
         XMLexpatAnsiParserTest Parser;
         Parser.ExpectedStartElement = "DOCUMENT";
         Parser.ExpectedEndElement = "DOCUMENT";
         Parser.ExpectedAttributes.push_back();
         Parser.ExpectedAttributes.back().Key = "Attr";
         Parser.ExpectedAttributes.back().Value = "hello";
         Parser.ExpectedAttributes.push_back();
         Parser.ExpectedAttributes.back().Key = "Attr2";
         Parser.ExpectedAttributes.back().Value = "hello2";

         Parser.parse(Input);

         TST_ASSERT(Parser.CurrentData == "DATA");
      }
      //test non readable ascii (>= 127)
      {
         COLstring Input = "<ALWAYSVALID Attr=\"hel&#x9B;lo\" Attr2=\"hel&#x9B;lo2\">DA&#x9B;TA</ALWAYSVALID>";
         XMLexpatAnsiParserTest Parser;
         Parser.ExpectedStartElement = "ALWAYSVALID";
         Parser.ExpectedEndElement = "ALWAYSVALID";
         Parser.ExpectedAttributes.push_back();
         Parser.ExpectedAttributes.back().Key = "Attr";
         Parser.ExpectedAttributes.back().Value = "hel\x9Blo";
         Parser.ExpectedAttributes.push_back();
         Parser.ExpectedAttributes.back().Key = "Attr2";
         Parser.ExpectedAttributes.back().Value = "hel\x9Blo2";

         Parser.parse(Input);

         TST_ASSERT(Parser.CurrentData == "DA\x9BTA");
      }
      
   }
   

};

TSTtestable* getXMLansiParserTests()
{
   TST_START_COLLECTION("testXMLansiParser")
      TST_ADD_TEST_FUNCTION(XMLexpatAnsiParserTest::testAnsiCharConversion)
      TST_ADD_TEST_FUNCTION(XMLexpatAnsiParserTest::testXmlStream)
   TST_END_COLLECTION
}
