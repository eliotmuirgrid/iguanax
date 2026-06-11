//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLexpatAnsiParser
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   May 2005
//
//---------------------------------------------------------------------------

#include "XMLprecomp.h"
#pragma hdrstop

#include <COL/COLstring.h>
#include <COL/COLendian.h>
#include <EXP/expat.h>
#include <XML/XMLexpatParser.h>
#include <XML/XMLexpatAnsiParser.h>
#include <COL/COLerror.h>
#include <COL/COLauto.h>
#include <LEG/LEGvector.h>
#include <XML/XMLptrArray.h>

// This type hierachy defers operations 
// so we can use polymorphism instead of 
// if statements with the given union in
// XMLexpatAnsiTranscodedString
class XMLexpatAnsiTranscodedString;
class XMLexpatAnsiTranscodedStringType
{
public:
   virtual void construct(XMLexpatAnsiTranscodedString* pThis, const char* pInputChar) const = 0;
   virtual void copy(XMLexpatAnsiTranscodedString* pThis, const XMLexpatAnsiTranscodedString* pThisOrig) const = 0;
   virtual void cleanup(XMLexpatAnsiTranscodedString* pThis) const = 0;
   virtual const char* c_str(const XMLexpatAnsiTranscodedString* pThis) const = 0;
   virtual COLstring& colString(XMLexpatAnsiTranscodedString* pThis) const = 0;
   virtual bool isColString() const = 0;
   virtual COLuint32 size(const XMLexpatAnsiTranscodedString* pThis) const = 0;
};

// This class contains a union which points to the correct 
// implementation (either plain const char* or a COLstring)
// it provides minimal overhead by avoiding virtual functions
// when constructing the wrapper.
class XMLexpatAnsiTranscodedString
{
public:
   friend class XMLexpatAnsiTranscodedStringTypeConstChar;
   friend class XMLexpatAnsiTranscodedStringTypeColString;

   union TTransodeStringUnion
   {
      const char* pConstChar;
      COLstring* pCOLstring;
   };

   XMLexpatAnsiTranscodedString();
   XMLexpatAnsiTranscodedString(const XMLexpatAnsiTranscodedStringType* ipType, const char* pString);
   ~XMLexpatAnsiTranscodedString();
   XMLexpatAnsiTranscodedString(const XMLexpatAnsiTranscodedString& Orig);
   XMLexpatAnsiTranscodedString& operator=(const XMLexpatAnsiTranscodedString& Orig);
   const char* c_str() const;
   COLstring& colString();
   bool isColString() const;
   COLuint32 size() const;

private:
   const XMLexpatAnsiTranscodedStringType* pType;
   TTransodeStringUnion ContainedString;
};


class XMLexpatAnsiTranscodedStringTypeColString : public XMLexpatAnsiTranscodedStringType
{
public:
   XMLexpatAnsiTranscodedStringTypeColString() {}

   static const XMLexpatAnsiTranscodedStringTypeColString* instance()
   {
      static const XMLexpatAnsiTranscodedStringTypeColString Instance;
      return &Instance;
   }

   virtual void construct(XMLexpatAnsiTranscodedString* pThis, const char* pInputChar) const
   {
      pThis->ContainedString.pCOLstring = new COLstring;
      *pThis->ContainedString.pCOLstring = pInputChar;
   }

   virtual void copy(XMLexpatAnsiTranscodedString* pThis, const XMLexpatAnsiTranscodedString* pThisOrig) const
   {
      if (pThisOrig->pType != this) //switching types
      {
         cleanup(pThis);
         pThis->pType = pThisOrig->pType;
         pThis->pType->construct(pThis,pThis->c_str());
      }
      else
      {
         *pThis->ContainedString.pCOLstring = *pThisOrig->ContainedString.pCOLstring;
      }
   }

   virtual void cleanup(XMLexpatAnsiTranscodedString* pThis) const
   {
      delete pThis->ContainedString.pCOLstring;
      pThis->ContainedString.pCOLstring = NULL;
   }

   virtual const char* c_str(const XMLexpatAnsiTranscodedString* pThis) const
   {
      return pThis->ContainedString.pCOLstring->c_str();
   }

   virtual COLstring& colString(XMLexpatAnsiTranscodedString* pThis) const
   {
      return *pThis->ContainedString.pCOLstring;
   }

   virtual bool isColString() const
   {
      return true;
   }

   virtual COLuint32 size(const XMLexpatAnsiTranscodedString* pThis) const
   {
      return pThis->ContainedString.pCOLstring->size();
   }
};

class XMLexpatAnsiTranscodedStringTypeConstChar : public XMLexpatAnsiTranscodedStringType
{
public:
   XMLexpatAnsiTranscodedStringTypeConstChar() {}

   static const XMLexpatAnsiTranscodedStringTypeConstChar* instance()
   {
      static const XMLexpatAnsiTranscodedStringTypeConstChar Instance;
      return &Instance;
   }

   virtual void construct(XMLexpatAnsiTranscodedString* pThis, const char* pInputChar) const
   {
      pThis->ContainedString.pConstChar = pInputChar;
   }

   virtual void copy(XMLexpatAnsiTranscodedString* pThis, const XMLexpatAnsiTranscodedString* pThisOrig) const
   {
      if (pThisOrig->pType != this) //switching types
      {
         cleanup(pThis);
         pThis->pType = pThisOrig->pType;
         pThis->pType->construct(pThis,pThisOrig->c_str());
      }
      else
      {
         pThis->ContainedString.pConstChar = pThisOrig->ContainedString.pConstChar;
      }
   }

   virtual void cleanup(XMLexpatAnsiTranscodedString* pThis) const
   {
      pThis->ContainedString.pConstChar = NULL;
   }

   virtual const char* c_str(const XMLexpatAnsiTranscodedString* pThis) const
   {
      return pThis->ContainedString.pConstChar;
   }

   virtual COLstring& colString(XMLexpatAnsiTranscodedString* pThis) const
   {
      COL_ERROR_STREAM("Invalid Call",0);
   }

   virtual bool isColString() const
   {
      return false;
   }

   virtual COLuint32 size(const XMLexpatAnsiTranscodedString* pThis) const
   {
      return strlen(pThis->ContainedString.pConstChar);
   }
};

//XMLexpatAnsiTranscodedString
XMLexpatAnsiTranscodedString::XMLexpatAnsiTranscodedString() 
   : pType(XMLexpatAnsiTranscodedStringTypeConstChar::instance())
{
   pType->construct(this,NULL);
}

XMLexpatAnsiTranscodedString::XMLexpatAnsiTranscodedString
(
   const XMLexpatAnsiTranscodedStringType* ipType, 
   const char* pString
)
   : pType(ipType)
{
   pType->construct(this,pString);
};

XMLexpatAnsiTranscodedString::~XMLexpatAnsiTranscodedString()
{
   pType->cleanup(this);
};

XMLexpatAnsiTranscodedString::XMLexpatAnsiTranscodedString(const XMLexpatAnsiTranscodedString& Orig)
   : pType(XMLexpatAnsiTranscodedStringTypeConstChar::instance())
{
   pType->copy(this,&Orig);
}

XMLexpatAnsiTranscodedString& XMLexpatAnsiTranscodedString::operator=(const XMLexpatAnsiTranscodedString& Orig)
{
   if (&Orig != this)
   {
      pType->copy(this,&Orig);
   }
   return *this;
}

const char* XMLexpatAnsiTranscodedString::c_str() const
{
   return pType->c_str(this);
}

COLstring& XMLexpatAnsiTranscodedString::colString()
{
   return pType->colString(this);
}

bool XMLexpatAnsiTranscodedString::isColString() const
{
   return pType->isColString();
}

COLuint32 XMLexpatAnsiTranscodedString::size() const
{
   return pType->size(this);
}

class XMLexpatAnsiParserPrivate : public XMLexpatParser
{
public:
   XMLexpatAnsiParserPrivate(XMLexpatAnsiParser* ipOwner) : pOwner(ipOwner){}

   // pOutput will contain the  ANSI char from the given input 
   // returns the number of bytes read from the input
   COLuint32 utf8CharToAnsiChar(const COLuint8* pInput, COLuint8* pOutput)
   {
      while (*pInput != '\0')
      {  
         if ( *pInput & 0x80 )
         {
            const COLuint8* pOrig = pInput;
            // start the conversion, we are doing either 
            // 2, 3, or 4 byte conversion
            // this algorithm is probably not the most efficient
            // but we hope not to call it too many times anyway.
            
            // determine bytes to read and take appropriate action
            // 'v' represents the bits we want, 

            if ( !( (*pInput & 0xF0) ^ 0xF0) ) 
            {
               //4 bytes -> 21 bits
               //11110vvv 10vvvvvv 10vvvvvv 10vvvvvv

               COLuint8 Total[4];
               Total[0] = pInput[0];
               COLPRECONDITION(pInput[1] != '\0')
               Total[1] = pInput[1];
               COLPRECONDITION(pInput[2] != '\0')
               Total[2] = pInput[2];
               COLPRECONDITION(pInput[3] != '\0')
               Total[3] = pInput[3];
               
               Total[3] &= 0x3F;
               Total[3] |= (Total[2] & 0x03) << 6; 
               Total[2] &= 0x3F;
               Total[2] >>= 2;
               Total[2] |= Total[1] << 4;
               Total[1] >>= 4;
               Total[1] &= 0x03;
               Total[1] |= (Total[0] << 2) & 0x1C;
               Total[0] = 0;

               if ((Total[0] > 0) || 
                   (Total[1] > 0) ||
                   (Total[2] > 0)) //if any of the higher bytes contains any ones, it exceeds 255
               {
                  COL_ERROR_STREAM(COL_T("Unable to transcode stream at ") << reinterpret_cast<const char*>(pOrig),0);
               }

               *pOutput = Total[3];
               return 4;
            }           
            else if ( !((*pInput & 0xE0) ^ 0xE0) )
            {
               //3 bytes -> 16 bits
               //1110vvvv 10vvvvvv 10vvvvvv

               COLuint8 Total[3];
               Total[0] = pInput[0];
               COLPRECONDITION(pInput[1] != '\0')
               Total[1] = pInput[1];
               COLPRECONDITION(pInput[2] != '\0')
               Total[2] = pInput[2];
               
               Total[2] &= 0x3F;
               Total[2] |= (Total[1] & 0x03) << 6; 
               Total[1] &= 0x3F;
               Total[1] >>= 2;
               Total[1] |= Total[0] << 4;
               Total[0] = 0;

               if ((Total[0] > 0) || (Total[1] > 0)) //if any of the higher bytes contains any ones, it exceeds 255
               {
                  COL_ERROR_STREAM(COL_T("Unable to transcode stream at ") << reinterpret_cast<const char*>(pOrig),0);
               }

               *pOutput = Total[2];
               return 3;
            }
            else if ( !((*pInput & 0xC0) ^ 0xC0) )//equivalent to checking if all upper 2 bits are set
            {
               //2 bytes -> 11 bits
               //110vvvvv 10vvvvvv -> 00000vvv vvvvvvvv

               COLuint8 Total[2];
               Total[0] = pInput[0];
               COLPRECONDITION(pInput[1] != '\0')
               Total[1] = pInput[1];
               
               Total[1] &= 0x3F;
               Total[1] |= (Total[0] & 0x03) << 6; 
               Total[0] = (Total[0] >> 2) & 0x07;

               if (Total[0] > 0) //if high byte contains any ones, it exceeds 255
               {
                  COL_ERROR_STREAM(COL_T("Unable to transcode stream at ") << reinterpret_cast<const char*>(pOrig),0);
               }

               *pOutput = Total[1];
               return 2;
            }
            else
            {
               COL_ERROR_STREAM(COL_T("Unable to transcode stream at ") << reinterpret_cast<const char*>(pOrig),0);
            }
            COLPRECONDITION(false); //should never be here
            return (COLuint32)-1;
         }
         else
         {
            //single byte
            *pOutput = *pInput;
            return 1;
         }
      }
      COLPRECONDITION(false); //should never be here
      return (COLuint32)-1;
   }

   //this will always return a copy of the given string
   XMLexpatAnsiTranscodedString utf8ToAnsiColString
   (
      //both parameters must be in the same string
      const char* pOrigString, //the beginning of the entire string
      const char* pStartTranscodeString //the first character at which transcoding should start
   )
   {
      XMLexpatAnsiTranscodedString Output(XMLexpatAnsiTranscodedStringTypeColString::instance(),NULL);
      //write all data not needing transcoding
      Output.colString().write(pOrigString,pStartTranscodeString-pOrigString);
      //start transcode at the pStartPoint
      COLuint8 CurrentChar = 0;
      while (*pStartTranscodeString != '\0')
      {
         pStartTranscodeString += utf8CharToAnsiChar(reinterpret_cast<const COLuint8*>(pStartTranscodeString),&CurrentChar);
         Output.colString().write(&CurrentChar,sizeof(CurrentChar));
      }
      return Output;
   }

   XMLexpatAnsiTranscodedString utf8ToAnsi(const char* pInput)
   {
      const char* pOrig = pInput;
      while (*pInput != '\0')
      {
         if ((static_cast<COLuint8>(*pInput)) & 0x80)
         {
            return utf8ToAnsiColString(pOrig,pInput);
         }
         pInput++;
      }
      XMLexpatAnsiTranscodedString UnTranscoded(XMLexpatAnsiTranscodedStringTypeConstChar::instance(), pOrig);
      return UnTranscoded;
   }

   const char** utf8PtrArrayToAnsi(const char** pPtrArrayIn)
   {
      TranscodedAttributes.clear();
      const char** pPtrArrayOrig = pPtrArrayIn;
      bool UseOrigArray = true;
      while (*pPtrArrayIn != NULL)
      {
         TranscodedAttributes.push_back();
         TranscodedAttributes.back() = utf8ToAnsi(*pPtrArrayIn);
         if (TranscodedAttributes.back().isColString() && UseOrigArray)
         {
            //basically indicates at least one of the values
            //was copied out
            //so we will have to rebuild the array.
            UseOrigArray = false;
         }
         pPtrArrayIn++;
      }

      if (UseOrigArray)
      {
         return pPtrArrayOrig;
      }
      
      //rebuild 
      TranscodedAttributePtrArray.clear();
      TranscodedAttributePtrArray.resize( TranscodedAttributes.size() );
      for (COLuint32 AttrIndex = 0; AttrIndex < TranscodedAttributes.size(); AttrIndex++)
      {
         TranscodedAttributePtrArray[AttrIndex] = TranscodedAttributes[AttrIndex].c_str();
      }
      return TranscodedAttributePtrArray.array();
   }

   //transcode and defer these functions
   virtual void onStartElement(const char* Name, const char** attr)
   {
      XMLexpatAnsiTranscodedString TransName;
      TransName = utf8ToAnsi(Name);
      const char** pNewAttr = utf8PtrArrayToAnsi(attr);
      pOwner->onStartElement(TransName.c_str(),pNewAttr);
   }

   virtual void onEndElement(const char* Name)
   {
      XMLexpatAnsiTranscodedString TransName;
      TransName = utf8ToAnsi(Name);
      pOwner->onEndElement(TransName.c_str());
   }

   virtual void onCharacterData(const char* pData, int Length)
   {
      //this is unfortunate that a COLstring has to be used here
      //because we need the null termination
      CharacterData.clear();
      CharacterData.write(pData,Length);

      //we also assume that we get at least one valid UTF-8 char
      //in this call, and we don't have to buffer, and
      //that multiple bytes of utf-8 chars don't come in multiple calls
      XMLexpatAnsiTranscodedString TransData;
      TransData = utf8ToAnsi(CharacterData.c_str());
      
      pOwner->onCharacterData(TransData.c_str(),TransData.size());
   }
   
   virtual void onComment(const char* Data)
   {
      XMLexpatAnsiTranscodedString TransComment;
      TransComment = utf8ToAnsi(Data);
      pOwner->onComment(TransComment.c_str());
   }

   virtual void onStartCdataSection()
   {
      pOwner->onStartCdataSection();
   }

   virtual void onEndCdataSection()
   {
      pOwner->onEndCdataSection();
   }

   COLstring CharacterData;
   LEGvector<XMLexpatAnsiTranscodedString> TranscodedAttributes;
   XMLptrArray<const char> TranscodedAttributePtrArray;

   XMLexpatAnsiParser* pOwner;
};

XMLexpatAnsiParser::XMLexpatAnsiParser()
{
   pMember = new XMLexpatAnsiParserPrivate(this);
}

XMLexpatAnsiParser::~XMLexpatAnsiParser()
{
   delete pMember;
}

void XMLexpatAnsiParser::parse(const COLstring& XMLdata)
{
   pMember->parse(XMLdata);
}

void XMLexpatAnsiParser::init()
{
   pMember->init();
}

void XMLexpatAnsiParser::destroy()
{
   pMember->destroy();
}

void XMLexpatAnsiParser::parseBuffer(const char* Buffer, COLuint32 SizeOfBuffer, bool IsFinal)
{
   pMember->parseBuffer(Buffer,SizeOfBuffer,IsFinal);
}

void XMLexpatAnsiParser::flush()
{
   pMember->flush();
}

COLuint32 XMLexpatAnsiParser::currentLineNumber()
{
   return pMember->currentLineNumber();
}

COLuint32 XMLexpatAnsiParser::currentColumnNumber()
{
   return pMember->currentColumnNumber();
}

XMLexpatParserError XMLexpatAnsiParser::onError
(
   const COLstring& ErrorText,
   int Line, int Column, long Byte, int Length
)
{
   return pMember->onError(ErrorText, Line, Column, Byte, Length);
}

const char** XMLexpatAnsiParser::utf8PtrArrayToAnsi(const char** pPtrArrayIn)
{
   return pMember->utf8PtrArrayToAnsi(pPtrArrayIn);
}

COLstring XMLexpatAnsiParser::utf8ToAnsiColString
(
   //both parameters must be in the same string
   const char* pOrigString, //the beginning of the entire string
   const char* pStartTranscodeString //the first character at which transcoding should start
)
{
   return pMember->utf8ToAnsiColString(pOrigString,pStartTranscodeString).colString();   
}

COLuint32 XMLexpatAnsiParser::utf8CharToAnsiChar(const COLuint8* pInput, COLuint8* pOutput)
{
   return pMember->utf8CharToAnsiChar(pInput,pOutput);
}

COLstring XMLexpatAnsiParser::utf8ToAnsi(const char* pInput)
{
   return pMember->utf8ToAnsi(pInput).c_str();
}
