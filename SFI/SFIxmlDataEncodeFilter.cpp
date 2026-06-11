//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIxmlDataEncodeFilter
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

#include "SFIxmlDataEncodeFilter.h"

#include <COL/COLsinkString.h>

typedef void (*fpEscapeChar)(unsigned char Input, COLsink& Output);

#define SFI_XML_CHARACTER_RANGE 256

// See the following URL for the definition of the data:
//
//   http://www.w3.org/TR/2004/REC-xml-20040204/#NT-content

struct SFIxmlDataEncodeFilterPrivate
{
   SFIxmlDataEncodeFilterPrivate()
   {
      // Only things which we need to escape are single/double quotes,
      // ampersands, and the less than symbol.
      COLuint32 EscapeIndex = 0;
      for(EscapeIndex = 0; EscapeIndex < SFI_XML_CHARACTER_RANGE; ++EscapeIndex)
      {
         EscapeFunctions[EscapeIndex] = noEscape;
      }
      EscapeFunctions['<'] = lessThan;
      EscapeFunctions['>'] = greaterThan;
      EscapeFunctions['&'] = ampersand;
      EscapeFunctions['\''] = apostrophe;
      EscapeFunctions['"'] = quote;
   }

   ~SFIxmlDataEncodeFilterPrivate() {}
   fpEscapeChar EscapeFunctions[SFI_XML_CHARACTER_RANGE];

   inline static void noEscape(unsigned char Input, COLsink& Output)
   {
      Output.write(&Input, sizeof(unsigned char));
   }

   inline static void lessThan(unsigned char Input, COLsink& Output)
   {
      Output.write("&lt;", 4);
   }

   inline static void greaterThan(unsigned char Input, COLsink& Output)
   {
      Output.write("&gt;", 4);
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
};

SFIxmlDataEncodeFilter::SFIxmlDataEncodeFilter(COLsink* ipNext) : COLfilter(ipNext)
{
   pMember = new SFIxmlDataEncodeFilterPrivate;
}

SFIxmlDataEncodeFilter::~SFIxmlDataEncodeFilter()
{
   delete pMember;
}

COLuint32 SFIxmlDataEncodeFilter::write(const void* cpBuffer, COLuint32 SizeOfBuffer){
   unsigned char Character;
   for(COLuint32 CharacterIndex = 0; CharacterIndex < SizeOfBuffer; CharacterIndex++)
   {
      Character = ((unsigned char*)cpBuffer)[CharacterIndex];
      (*pMember->EscapeFunctions[Character])(Character,next());
   }

   return SizeOfBuffer;
}

void SFIxmlDataEncode(const COLstring& Input, COLstring* pOutput){
   COLsinkString Sink(pOutput, false);
   SFIxmlDataEncodeFilter Filter(&Sink);
   Filter.write(Input.c_str(), Input.size());
}

COLstring SFIxmlDataEncode(const COLstring& Input){
   COLstring Output;
   SFIxmlDataEncode(Input, &Output);
   return Output;
}
