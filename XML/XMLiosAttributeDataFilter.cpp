#include "XMLprecomp.h"
#pragma hdrstop

#include <XML/XMLiosAttributeDataFilter.h>
#include <FMT/FMTbinaryDump.h>
#include <COL/COLostream.h>

typedef void (*fpEscapeChar)( XMLiosAttributeDataFilter* pMember, const char InputChar, COLsink& Output);

/*
 * BNF for XML attribute value - http://www.w3.org/TR/2004/REC-xml11-20040204/
 * ---------------------------------------------------------------------------------
 * [10] AttValue  ::= '"' ([^<&"] | Reference)* '"'  |  "'" ([^<&'] | Reference)* "'" 
 * [67] Reference ::= EntityRef | CharRef
 * [66] CharRef   ::= '&#' [0-9]+ ';' | '&#x' [0-9a-fA-F]+ ';'
 * [68] EntityRef ::= '&' Name ';'
 * 
 * This basically says that an attribute value can be anything except '<', '&', and '"'. So these
 * are the only characters that need to be escaped.
 * 
 * Section 3.3.3 form the spec named Attribute-Value Normalization points out that a parser
 * must normalize the whitespace in the attribute value...
 * "For a white space character (#x20, #xD, #xA, #x9), append a space character (#x20) to the 
 *  normalized value"
 *
 * Also, multiple spaces are removed...
 * "...the XML processor MUST further process the normalized attribute value by discarding any 
 *  leading and trailing space (#x20) characters, and by replacing sequences of space (#x20) 
 *  characters by a single space (#x20) character."
 *
 * This class escapes < & " ' \r \n \t and sequences of spaces.
 */

#define XMLcharacterRange 256
struct XMLiosAttributeDataFilterPrivate
{
   XMLiosAttributeDataFilterPrivate()
   {
      //set escapes
      //readable ascii is not escaped
      COLuint32 Index = 0;
      for( Index = 0; Index < 128; ++Index ) 
      {
         EscapeFunctions[ Index ] = noEscape;
      }
      //extended ascii is escaped
      for( Index = 128; Index < XMLcharacterRange; ++Index ) 
      {
         EscapeFunctions[ Index ] = defaultEscape;
      }
      EscapeFunctions['&'] = ampersand;
      EscapeFunctions['<'] = lessThan;
      EscapeFunctions['\''] = apostrophe;
      EscapeFunctions['\"'] = quote;
      EscapeFunctions['\r'] = defaultEscape;
      EscapeFunctions['\n'] = defaultEscape;
      EscapeFunctions['\t'] = defaultEscape;
      EscapeFunctions[' '] = spaceEscape;
   }

   ~XMLiosAttributeDataFilterPrivate(){}
   fpEscapeChar EscapeFunctions[XMLcharacterRange];

   inline static void ampersand(XMLiosAttributeDataFilter* pMember, const char Input,COLsink& Output)
   {
      static const COLstring Escape("&amp;");
      Output.write(Escape.c_str(),Escape.size());
   }

   inline static void lessThan(XMLiosAttributeDataFilter* pMember, const char Input,COLsink& Output)
   {
      static const COLstring Escape("&lt;");
      Output.write(Escape.c_str(),Escape.size());
   }

   inline static void greaterThan(XMLiosAttributeDataFilter* pMember, const char Input,COLsink& Output)
   {
      static const COLstring Escape("&gt;");
      Output.write(Escape.c_str(),Escape.size());
   }

   inline static void quote(XMLiosAttributeDataFilter* pMember, const char Input,COLsink& Output)
   {
      static const COLstring Escape("&quot;");
      Output.write(Escape.c_str(),Escape.size());
   }

   inline static void apostrophe(XMLiosAttributeDataFilter* pMember, const char Input,COLsink& Output)
   {
      static const COLstring Escape("&apos;");
      Output.write(Escape.c_str(),Escape.size());
   }
   
   inline static void defaultEscape(XMLiosAttributeDataFilter* pMember, const char Input, COLsink& Output)
   {
      static const COLstring Front("&#x");
      static const COLstring Back(";");
      COLostream Stream( Output ); 
      Stream << Front; 
      FMTtoHex( Stream, (COLuint32)((COLuint8)Input));
      Stream << Back;
   }
   
   inline static void spaceEscape(XMLiosAttributeDataFilter* pMember, const char Input, COLsink& Output)
   {
      if( !pMember->SpaceFlag )
      {
         pMember->SpaceFlag = true;
         Output.write(&Input,sizeof(char));
      }
      else
      {
         defaultEscape( pMember, Input, Output );
      }
   }
   
   inline static void noEscape(XMLiosAttributeDataFilter* pMember, const char Input, COLsink& Output)
   {
      static const COLuint32 charLen = sizeof(char);
      pMember->SpaceFlag = false;
      Output.write(&Input,charLen);
   }

   inline static XMLiosAttributeDataFilterPrivate& instance()
   {
      static XMLiosAttributeDataFilterPrivate Instance;
      return Instance;
   }
};


XMLiosAttributeDataFilter::XMLiosAttributeDataFilter(COLsink* ipSink)
: XMLiosFilter(ipSink),SpaceFlag(false)
{   
   pMember = &XMLiosAttributeDataFilterPrivate::instance();
}

XMLiosAttributeDataFilter::XMLiosAttributeDataFilter()
{
   pMember = &XMLiosAttributeDataFilterPrivate::instance();
}

XMLiosAttributeDataFilter::~XMLiosAttributeDataFilter()
{
}

COLuint32 XMLiosAttributeDataFilter::write(const void* cpBuffer, COLuint32 SizeOfBuffer)
{
   COLPRECONDITION(m_pSink != 0);
   COLuint8 Character;
   for (COLuint32 CharIndex = 0; CharIndex < SizeOfBuffer; CharIndex++)
   {
      Character = ((COLuint8*)cpBuffer)[CharIndex];
      (*pMember->EscapeFunctions[Character])( this, Character, *m_pSink );
   }

   return SizeOfBuffer;
}
