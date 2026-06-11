#ifndef _IOS_XML_TAG_FILTER_PRIVATE_H_
#define _IOS_XML_TAG_FILTER_PRIVATE_H_
//---------------------------------------------------------------------------
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
//
// Description:
//
// Private class so we can subclass easily
//
// Author: Nasron Cheong
// Date:   Nov 2003
//
//---------------------------------------------------------------------------
#include <COL/COLminimumInclude.h>
#include <COL/COLerror.h>

typedef void (*fpEscapeChar)(const char InputChar,COLsink& OutputBuffer);

struct XMLiosTagFilterPrivate
{
   XMLiosTagFilterPrivate(char XmlDelimiter)
   {
      //set escapes
      COLuint32 CharIndex = 0;
      while(CharIndex < 128)
      {      

         if ( CharIndex == '_' || CharIndex == '.'  ||
             (CharIndex >= 'A' && CharIndex <= 'Z') || // [A-Z]
             (CharIndex >= 'a' && CharIndex <= 'z') || // [a-z]
             (CharIndex >= '0' && CharIndex <= '9')    // [0-9]
            )
         {  //valid
            EscapeFunctions[CharIndex] = NoEscape;
         }
         else if ( CharIndex == ':' || CharIndex == ';' ) //semi-valid?
         {
            EscapeFunctions[CharIndex] = UnderscoreEscape;
         }
         else //invalid
         {
            EscapeFunctions[CharIndex] = NullEscape;
         }
         CharIndex++;
      }

      //also remove any characters that are in the extended ASCII
      //note that this will probably break under unicode
      CharIndex = 128;
      while (CharIndex < 256)
      {
         EscapeFunctions[CharIndex] = NullEscape;
         CharIndex++;
         if (CharIndex == 0)
         {
            break;
         }      
      }
      EscapeFunctions[XmlDelimiter] = NullEscape;
      this->XmlDelimiter = XmlDelimiter;
   }

   ~XMLiosTagFilterPrivate(){}
   fpEscapeChar EscapeFunctions[256];
   char XmlDelimiter;

   inline static void NullEscape(const char Input, COLsink& Output)
   {
      return;
   }

   inline static void NoEscape(const char Input, COLsink& Output)
   {
      static const COLuint32 CharLength = sizeof(Input);
      Output.write(&Input,CharLength);
   }

   inline static void UnderscoreEscape(const char Input, COLsink& Output)
   {
      static const COLstring Escape("_");
      Output.write(Escape.c_str(),Escape.size());
   }
};
#endif
