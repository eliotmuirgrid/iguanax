#include "XMLprecomp.h"
#pragma hdrstop
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc. All Rights Reserved
//
// Description: Implementation
//
// Author: Nasron Cheong
// Date:   Nov 2003
//
//---------------------------------------------------------------------------

#include <XML/XMLiosStream.h>
#include <XML/XMLiosTagFilter.h>
#include <XML/XMLiosDataFilter.h>
#include <XML/XMLiosAttributeDataFilter.h>

#include <COL/COLsinkString.h>

class XMLiosStreamPrivate
{
public:
   enum XMLstreamState
   {
      eInStartTag, //for escaping tags with <
      eInEndTag,   //for escaping tags with </
      eInPlainTag, //for escaping tags without < or </
      eInData,     //for plain xml data
      eInAttributeData,     //for plain xml data
      eUnescaped   //without any escaping
   };

   XMLiosStreamPrivate(char iXmlDelimiter, COLsink* ipTargetSink, bool IsOwner)
      : State(eInData),
        XmlDelimiter(iXmlDelimiter),
        m_CurrentTagSink(CurrentTag),
        OwnsSink(IsOwner),
        pTargetSink(ipTargetSink),
#ifdef _WIN32
        EndLine("\r\n")
#else
        EndLine("\n")
#endif
   {
      pDataFilter = new XMLiosDataFilter(pTargetSink);
      pAttributeDataFilter = new XMLiosAttributeDataFilter(pTargetSink);
      pTagFilter = new XMLiosTagFilter(XmlDelimiter,&m_CurrentTagSink);
   };

   virtual ~XMLiosStreamPrivate()
   {
      delete pTagFilter;
      delete pDataFilter;
      delete pAttributeDataFilter;
      if (OwnsSink)
      {
         delete pTargetSink;
      }
   };

   const char* numberFormat()
   {

      //we do not support hex for now...but we might later.
      //so its here for reference
      /*
      if( NumberBase == COLostream::Hexadecimal )
      {
         static const char* pHexadecimalFormat = "%X";
         return pHexadecimalFormat;
      }
      */
      static const char* pDecimalFormat = "%i";
      return pDecimalFormat;
   }

   void write(const void* Data, COLuint32 CountOfData)
   {
      switch(State)
      {
      case eInData:
         pDataFilter->write(Data, CountOfData);
         break;
      case eInAttributeData:
         pAttributeDataFilter->write(Data, CountOfData);
         break;
      case eInEndTag:
      case eInStartTag:
      case eInPlainTag:
         pTagFilter->write(Data, CountOfData);
         break;
      case eUnescaped:
         pTargetSink->write(Data, CountOfData);
         break;
      default:
         COL_ERROR_STREAM("Invalid State",0);
      }
   }

   //we need this call because tags are outputed during
   //calls to switching state
   void handleTag()
   {
      if (CurrentTag.size())
      {
         //special case for first character
         if ((47 < CurrentTag[0]) && (CurrentTag[0] < 58))
         {
            CurrentTag[0] = '_';
         }

         switch(State)
         {
         case eInEndTag:
            pTargetSink->write("</", 2);
            pTargetSink->write(CurrentTag.c_str(), CurrentTag.size());
            pTargetSink->write(">", 1);
            break;
         case eInStartTag:
            pTargetSink->write("<",1);
            pTargetSink->write(CurrentTag.c_str(), CurrentTag.size());
            pTargetSink->write(">",1);
            break;
         case eInPlainTag:
         case eUnescaped:
            pTargetSink->write(CurrentTag.c_str(), CurrentTag.size());
            break;
            break;
         default:
            COL_ERROR_STREAM("Invalid State",0);
         }
         CurrentTag.clear();
      }
   }

   //owns these filters
   XMLiosTagFilter* pTagFilter;
   XMLiosDataFilter* pDataFilter;
   XMLiosAttributeDataFilter* pAttributeDataFilter;

   //might own this sink
   COLsink* pTargetSink;
   bool OwnsSink;

   //a temp location for the current tag
   COLsinkString m_CurrentTagSink;
   COLstring CurrentTag;

   //the current state
   XMLstreamState State;

   //newline << flushine
   COLstring EndLine;

   //delimiter is cached
   char XmlDelimiter;

   //used for conversions
   char TempBuffer[100];
};

XMLiosStream::XMLiosStream(char XmlDelimiter, COLsink* pSink, bool isOwner)
{
   pMember = new XMLiosStreamPrivate(XmlDelimiter,pSink, isOwner);
}

XMLiosStream::~XMLiosStream()
{
   try
   {
      //we cannot flush if we don't own the sink since we're not guaranteed that the lifetime of the
      //sink exceeds the lifetime of the stream.
      if (pMember->OwnsSink)
      {
         flush(*this);
      }
      delete pMember;
   }
   catch(const COLerror& Error)
   {
      XML_LOG(Error);
   }
}

XMLiosStream& escapeTag(XMLiosStream& _outs)
{
   _outs.pMember->handleTag();
   _outs.pMember->State = XMLiosStreamPrivate::eInPlainTag;
   return _outs;
}

XMLiosStream& elementData(XMLiosStream& _outs)
{
   _outs.pMember->handleTag();
   _outs.pMember->State = XMLiosStreamPrivate::eInData;
   return _outs;
}

XMLiosStream& attributeData(XMLiosStream& _outs)
{
   _outs.pMember->handleTag();
   _outs.pMember->State = XMLiosStreamPrivate::eInAttributeData;
   return _outs;
}

XMLiosStream& startTag(XMLiosStream& _outs)
{
   _outs.pMember->handleTag();
   _outs.pMember->State = XMLiosStreamPrivate::eInStartTag;
   return _outs;
}

XMLiosStream& endTag(XMLiosStream& _outs)
{
   _outs.pMember->handleTag();
   _outs.pMember->State = XMLiosStreamPrivate::eInEndTag;
   return _outs;
}

XMLiosStream& newline(XMLiosStream& _outs)
{
   _outs.pMember->handleTag();
   _outs.pMember->pTargetSink->write(_outs.pMember->EndLine.c_str(),_outs.pMember->EndLine.size());
   _outs.pMember->State = XMLiosStreamPrivate::eInData;
   return _outs;
}

XMLiosStream& unescaped(XMLiosStream& _outs)
{
   _outs.pMember->handleTag();
   _outs.pMember->State = XMLiosStreamPrivate::eUnescaped;
   return _outs;
}

XMLiosStream& delimiter(XMLiosStream& _outs)
{
   COLPRECONDITION(_outs.pMember->State != XMLiosStreamPrivate::eInData);
   COLPRECONDITION(_outs.pMember->State != XMLiosStreamPrivate::eUnescaped);
   _outs.pMember->CurrentTag += _outs.pMember->XmlDelimiter;
   return _outs;
}


XMLiosStream& XMLiosStream::operator<<(XMLiosStream&(*x)(XMLiosStream&))
{
   x(*this);
   return *this;
}

XMLiosStream& XMLiosStream::operator<<(char Character)
{
   pMember->write(&Character,1);
   return *this;
}

XMLiosStream& XMLiosStream::operator<<(unsigned char Character)
{
   pMember->write(&Character,1);
   return *this;
}

XMLiosStream& XMLiosStream::operator<<(const char* pString)
{
   pMember->write(pString, strlen(pString));
   return *this;
}

XMLiosStream& XMLiosStream::operator<<(char* pString)
{
   pMember->write(pString, strlen(pString));
   return *this;
}

XMLiosStream& XMLiosStream::operator<<(const COLstring& String)
{
   pMember->write(String.c_str(), String.size());
   return *this;
}

XMLiosStream& XMLiosStream::operator<<(unsigned long Number)
{
   COLuint32 SizeOfString = snprintf(pMember->TempBuffer, sizeof(pMember->TempBuffer), pMember->numberFormat(), Number);
   pMember->write(pMember->TempBuffer, SizeOfString);
   return *this;
}

XMLiosStream& XMLiosStream::operator<<(long Number)
{
   COLuint32 SizeOfString = snprintf(pMember->TempBuffer, sizeof(pMember->TempBuffer), pMember->numberFormat(), Number);
   pMember->write(pMember->TempBuffer, SizeOfString);
   return *this;
}

XMLiosStream& XMLiosStream::operator<<(short Number)
{
   COLuint32 SizeOfString = snprintf(pMember->TempBuffer, sizeof(pMember->TempBuffer), pMember->numberFormat(), Number);
   pMember->write(pMember->TempBuffer, SizeOfString);
   return *this;
}

#if defined(_WIN64)
XMLiosStream& XMLiosStream::operator<<(size_t Number)
{
   COL_STATIC_ASSERT( sizeof(size_t)==sizeof(__int64) );
   COLuint32 SizeOfString = snprintf(pMember->TempBuffer, sizeof(pMember->TempBuffer),  "%I64i", Number);
   pMember->write(pMember->TempBuffer, SizeOfString);
   return *this;
}
#endif

XMLiosStream& XMLiosStream::operator<<(double Number)
{
   COLuint32 SizeOfString = snprintf(pMember->TempBuffer, sizeof(pMember->TempBuffer), "%f", Number);
   pMember->write(pMember->TempBuffer, SizeOfString);
   return *this;
}

XMLiosStream& XMLiosStream::operator<<(unsigned int Number)
{
   COLuint32 SizeOfString = snprintf(pMember->TempBuffer, sizeof(pMember->TempBuffer), pMember->numberFormat(), Number);
   pMember->write(pMember->TempBuffer, SizeOfString);
   return *this;
}

XMLiosStream& XMLiosStream::operator<<(int Number)
{
   COLuint32 SizeOfString = snprintf(pMember->TempBuffer, sizeof(pMember->TempBuffer), pMember->numberFormat(), Number);
   pMember->write(pMember->TempBuffer, SizeOfString);
   return *this;
}

void XMLiosStream::setTagFilter(XMLiosTagFilter* pTagFilter)
{
   delete pMember->pTagFilter;
   pMember->pTagFilter = pTagFilter;
   pMember->pTagFilter->setSink(&pMember->m_CurrentTagSink);
   setXmlDelimiter(pMember->XmlDelimiter);
}

void XMLiosStream::setDataFilter(XMLiosDataFilter* pDataFilter)
{
   delete pMember->pDataFilter;
   pMember->pDataFilter = pDataFilter;
   pMember->pDataFilter->setSink(pMember->pTargetSink);
}

void XMLiosStream::setSink(COLsink* pSink, bool isOwner)
{
   clear();
   if (pMember->OwnsSink)
   {
      delete pMember->pTargetSink;
   }
   pMember->OwnsSink = isOwner;
   pMember->pTargetSink = pSink;

   //reassign sinks on data filter
   pMember->pDataFilter->setSink(pMember->pTargetSink);
}

void XMLiosStream::write(const void* pData, COLuint32 Size)
{
   pMember->write(pData,Size);
}

void XMLiosStream::setNewLine(const char* NewLine)
{
   pMember->EndLine = NewLine;
}

void XMLiosStream::setXmlDelimiter(char XmlDelimiter)
{
   COLPRECONDITION(pMember->pTagFilter != 0);
   pMember->pTagFilter->setXmlDelimiter(XmlDelimiter);
   pMember->XmlDelimiter = XmlDelimiter;
}

void XMLiosStream::clear()
{
   pMember->CurrentTag.clear();
   pMember->State = XMLiosStreamPrivate::eInData;
}

XMLiosStream& flush(XMLiosStream& _outs)
{
   _outs.pMember->handleTag();
   _outs.pMember->pTargetSink->flush();
   return _outs;
}

void XMLiosStream::addAttribute
(
   const char* pAttributeName,
   const char* pAttribute
)
{
   COLPRECONDITION(pMember->State == XMLiosStreamPrivate::eInStartTag);
   pMember->CurrentTag += ' ';
   pMember->CurrentTag += pAttributeName;
   pMember->CurrentTag += "=\"";
   XMLiosStream( '.', &pMember->m_CurrentTagSink ) << attributeData << pAttribute;
   pMember->CurrentTag += '\"';
}
