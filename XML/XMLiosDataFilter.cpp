//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc. All Rights Reserved
//
// Description:
//
// A filter than redirects write calls to its sink and does xml data escaping in 
// the write
//
// Author: Nasron Cheong
// Date:   Nov 2003
//
//---------------------------------------------------------------------------
#include "XMLprecomp.h"
#pragma hdrstop

#include <XML/XMLiosDataFilter.h>
#include <FMT/FMTbinaryDump.h>

typedef void (*fpEscapeChar)(const XMLiosDataFilter* pThis, const char InputChar, COLsink& Output);

struct XMLiosDataFilterPrivate
{
   XMLiosDataFilterPrivate()
   {
      for (COLuint32 CharIndex = 0; CharIndex < 256; CharIndex++)
      {      
         switch(CharIndex) 
         {
         case '&':
            EscapeFunctions[CharIndex] = ampersand;
            break;
         case '<':
            EscapeFunctions[CharIndex] = lessThan;
            break;
         case '>':
            EscapeFunctions[CharIndex] = greaterThan;
            break;
         case '\"':
            EscapeFunctions[CharIndex] = quote;
            break;
         case '\'':
            EscapeFunctions[CharIndex] = apostrophe;
            break;
         case '\n':
            EscapeFunctions[CharIndex] = defaultEscape;
            break;
         case '\t':
            EscapeFunctions[CharIndex] = defaultEscape;
            break;
         case '\r':
            EscapeFunctions[CharIndex] = defaultEscape;
            break;
         default:
            if ((CharIndex >= 0 && CharIndex < 32) ||
                (CharIndex >= 127 && CharIndex < 256) )
            {
               EscapeFunctions[CharIndex] = defaultEscape;
            }
            else
            {
               EscapeFunctions[CharIndex] = noEscape;
            }
            break;
         }
      }
   }

   ~XMLiosDataFilterPrivate(){}
   fpEscapeChar EscapeFunctions[256];

   inline static void ampersand(const XMLiosDataFilter* pThis, const char Input,COLsink& Output)
   {
      static const COLstring Escape("&amp;");
      Output.write(Escape.c_str(),Escape.size());
   }

   inline static void lessThan(const XMLiosDataFilter* pThis, const char Input,COLsink& Output)
   {
      static const COLstring Escape("&lt;");
      Output.write(Escape.c_str(),Escape.size());
   }

   inline static void greaterThan(const XMLiosDataFilter* pThis, const char Input,COLsink& Output)
   {
      static const COLstring Escape("&gt;");
      Output.write(Escape.c_str(),Escape.size());
   }

   inline static void quote(const XMLiosDataFilter* pThis, const char Input,COLsink& Output)
   {
      static const COLstring Escape("&quot;");
      Output.write(Escape.c_str(),Escape.size());
   }

   inline static void apostrophe(const XMLiosDataFilter* pThis, const char Input,COLsink& Output)
   {
      static const COLstring Escape("&apos;");
      Output.write(Escape.c_str(),Escape.size());
   }
   
   inline static void noEscape(const XMLiosDataFilter* pThis, const char Input, COLsink& Output)
   {
      static const COLuint32 charLen = sizeof(char);
      Output.write(&Input,charLen);
   }

   inline static void defaultEscape(const XMLiosDataFilter* pThis, const char Input, COLsink& Output)
   {
      pThis->onDefaultEscape(Input,Output);
   }
   
   inline static XMLiosDataFilterPrivate& instance()
   {
      static XMLiosDataFilterPrivate Instance;
      return Instance;
   }
};


XMLiosDataFilter::XMLiosDataFilter(COLsink* ipSink)
: XMLiosFilter(ipSink)
{   
   pMember = &XMLiosDataFilterPrivate::instance();
}

XMLiosDataFilter::XMLiosDataFilter()
{
   pMember = &XMLiosDataFilterPrivate::instance();
}

XMLiosDataFilter::~XMLiosDataFilter()
{
}

COLuint32 XMLiosDataFilter::write(const void* cpBuffer, COLuint32 SizeOfBuffer)
{
   COLPRECONDITION(m_pSink != 0);
   COLuint8 Character;
   for (COLuint32 CharIndex = 0; CharIndex < SizeOfBuffer; CharIndex++)
   {
      Character = ((COLuint8*)cpBuffer)[CharIndex];
      (*pMember->EscapeFunctions[Character])(this,Character,*m_pSink);
   }

   return SizeOfBuffer;
}

void XMLiosDataFilter::onDefaultEscape(const char Input, COLsink& Output) const
{
   static const COLstring Front("&#x");
   static const COLstring Back(";");
   COLostream Stream( Output ); 
   Stream << Front; 
   FMTtoHex( Stream, (COLuint32)((COLuint8)Input));
   Stream << Back;
}
