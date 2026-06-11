//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIxmlAttributeDataEncodeFilter
//
// Description:
//
// Implementation
//
// Author: Matt Lawrence
// Date:   Wednesday, July 26th, 2006 @ 02:16:28 PM
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#include "SFIxmlAttributeDataEncodeFilter.h"

#include <COL/COLsinkString.h>

typedef void (*fpEscapeChar)(unsigned char Input, COLsink& Output);

#define SFI_XML_CHARACTER_RANGE 256

// See the following URL for the definition of the attribute data:
//
//   http://www.w3.org/TR/2004/REC-xml-20040204/#NT-AttValue

class SFIxmlAttributeDataEncodeFilterPrivate
{
public:
   SFIxmlAttributeDataEncodeFilterPrivate()
   {
      // Only things which we need to escape are single/double quotes,
      // ampersands, and the less than symbol.
      COLuint32 EscapeIndex = 0;
      for(EscapeIndex = 0; EscapeIndex < SFI_XML_CHARACTER_RANGE; ++EscapeIndex)
      {
         EscapeFunctions[EscapeIndex] = noEscape;
      }
      EscapeFunctions['<'] = lessThan;
      EscapeFunctions['&'] = ampersand;
      EscapeFunctions['\''] = apostrophe;
      EscapeFunctions['"'] = quote;

      // Due to Attribute-Value Normalization, tabs, newlines and carriage returns get
      // replaced with spaces when reading the XML.  They are escaped here to prevent that.
      EscapeFunctions['\t'] = tab;
      EscapeFunctions['\n'] = newline;
      EscapeFunctions['\r'] = carriageReturn;
   }

   ~SFIxmlAttributeDataEncodeFilterPrivate() {}
   fpEscapeChar EscapeFunctions[SFI_XML_CHARACTER_RANGE];

   inline static void noEscape(unsigned char Input, COLsink& Output)
   {
      Output.write(&Input, sizeof(char));
   }

   inline static void lessThan(unsigned char Input, COLsink& Output)
   {
      Output.write("&lt;", 4);
   }

   inline static void apostrophe(unsigned char Input, COLsink& Output)
   {
      Output.write("&apos;", 6);
   }

   inline static void quote(unsigned char Input, COLsink& Output)
   {
      Output.write("&quot;", 6);
   }

   inline static void ampersand(unsigned char Input, COLsink& Output)
   {
      Output.write("&amp;", 5);
   }

   inline static void tab(unsigned char Input, COLsink& Output)
   {
      Output.write("&#x9;", 5);
   }

   inline static void newline(unsigned char Input, COLsink& Output)
   {
      Output.write("&#xA;", 5);
   }

   inline static void carriageReturn(unsigned char Input, COLsink& Output)
   {
      Output.write("&#xD;", 5);
   }
};

SFIxmlAttributeDataEncodeFilter::SFIxmlAttributeDataEncodeFilter(COLsink* ipNext)
: COLfilter(ipNext)
{
   pMember = new SFIxmlAttributeDataEncodeFilterPrivate;
}

SFIxmlAttributeDataEncodeFilter::~SFIxmlAttributeDataEncodeFilter()
{
   delete pMember;
}

COLuint32 SFIxmlAttributeDataEncodeFilter::write(const void* cpBuffer, COLuint32 SizeOfBuffer)
{
   unsigned char Character;
   for(COLuint32 CharacterIndex = 0; CharacterIndex < SizeOfBuffer; CharacterIndex++)
   {
      Character = ((unsigned char*)cpBuffer)[CharacterIndex];
      (*pMember->EscapeFunctions[Character])(Character,next());
   }

   return SizeOfBuffer;
}

// Eliot - this is not the most efficient - but it allows me to quickly refactor and get COLsink out of COLstring
void SFIxmlAttributeDataEncode(const COLstring& Input, COLstring* pOutput){
   COLsinkString Sink(pOutput, false);
   SFIxmlAttributeDataEncodeFilter Filter(&Sink);
   Filter.write(Input.c_str(), Input.size());
}

COLstring SFIxmlAttributeDataEncode(const COLstring& Input){
   COLstring Output;
   SFIxmlAttributeDataEncode(Input, &Output);
   return Output;
}
