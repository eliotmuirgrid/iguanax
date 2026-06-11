//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIxmlTagEncodeFilter
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

#include "SFIxmlTagEncodeFilter.h"

typedef void (*fpEscapeChar)(unsigned char Input, COLsink& Output);
typedef bool (*fpEscapeFirstChar)(unsigned char Input, COLsink& Output);

#define SFI_XML_CHARACTER_RANGE 256

// See the following URL for the specification of a valid XML tag:
//
//   http://www.w3.org/TR/2004/REC-xml-20040204/#NT-Name
//
class SFIxmlTagEncodeFilterPrivate
{
public:
   SFIxmlTagEncodeFilterPrivate()
   {
      COLuint32 EscapeIndex = 0;
      for(EscapeIndex = 0; EscapeIndex < SFI_XML_CHARACTER_RANGE; ++EscapeIndex)
      {
         if((EscapeIndex >= 'A' && EscapeIndex <= 'Z')
            || (EscapeIndex >= 'a' && EscapeIndex <= 'z')
            || (EscapeIndex >= '0' && EscapeIndex <= '9')
            || EscapeIndex == '.' || EscapeIndex == '-'
            || EscapeIndex == '_' || EscapeIndex == ':')
         {
            EscapeFunctions[EscapeIndex] = noEscape;
            if((EscapeIndex >= '0' && EscapeIndex <= '9')
               || EscapeIndex == '.' || EscapeIndex == '-')
            {
               FirstCharacterEscapeFunctions[EscapeIndex] = firstCharacterUnderscoreEscape;
            }
            else
            {
               FirstCharacterEscapeFunctions[EscapeIndex] = firstCharacterNoEscape;
            }
         }
         else
         {
           EscapeFunctions[EscapeIndex] = nullEscape;
           FirstCharacterEscapeFunctions[EscapeIndex] = firstCharacterNullEscape;
         }
      }
   }

   bool StartOfData;
   fpEscapeChar EscapeFunctions[SFI_XML_CHARACTER_RANGE];
   fpEscapeFirstChar FirstCharacterEscapeFunctions[SFI_XML_CHARACTER_RANGE];

   inline static void noEscape(unsigned char Input, COLsink& Output)
   {
      Output.write(&Input, sizeof(char));
   }

   inline static void nullEscape(unsigned char Input, COLsink& Output)
   {
   }
   
   inline static bool firstCharacterUnderscoreEscape(unsigned char Input, COLsink& Output)
   {
      Output.write("_", 1);
      return false;
   } 
   
   inline static bool firstCharacterNoEscape(unsigned char Input, COLsink& Output)
   {
      Output.write(&Input, 1);
      return false;
   }

   inline static bool firstCharacterNullEscape(unsigned char Input, COLsink& Output)
   {
      return true;
   }
};

SFIxmlTagEncodeFilter::SFIxmlTagEncodeFilter(COLsink* ipNext) : COLfilter(ipNext)
{
   pMember = new SFIxmlTagEncodeFilterPrivate;
   pMember->StartOfData = true;
}

SFIxmlTagEncodeFilter::~SFIxmlTagEncodeFilter()
{
   delete pMember;
}

COLuint32 SFIxmlTagEncodeFilter::write(const void* cpBuffer, COLuint32 SizeOfBuffer)
{
   unsigned char Character;
   for(COLuint32 CharacterIndex = 0; CharacterIndex < SizeOfBuffer; CharacterIndex++)
   {
      Character = ((unsigned char*)cpBuffer)[CharacterIndex];
      if(pMember->StartOfData)
      {
         pMember->StartOfData = (*pMember->FirstCharacterEscapeFunctions[Character])(Character,next());
      }
      else
      {
         (*pMember->EscapeFunctions[Character])(Character,next());
      }      
   }

   return SizeOfBuffer;
}

void SFIxmlTagEncodeFilter::resetFilter()
{
   pMember->StartOfData = true;
}

COLstring SFIxmlTagEncode(const COLstring& Input){
   COLstring Output;
   COLsinkString Sink(&Output, false);
   SFIxmlTagEncodeFilter Filter(&Sink);
   Filter.write(Input.c_str(), Input.size());
   return Output;
}
