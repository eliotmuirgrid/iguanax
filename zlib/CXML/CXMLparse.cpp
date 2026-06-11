//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: CXMLparse
//
// Author: Eliot Muir
//
// Description:
//
// Code to parse XML using expat into a COLvar
//---------------------------------------------------------------------------

#include "CXMLparse.h"

#include <EXP/expat.h>

#include <COL/COLstack.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

class CXMLstate{
public:
   CXMLstate() : m_IsRoot(true){
      COL_METHOD(CXMLstate::CXMLstate);

   }

   bool m_IsRoot;
   COLstack<COLvar*> m_VarStack;
private:
};

static void StartElement(void* pUserData, const XML_Char* Name, const XML_Char** atts){
   COL_FUNCTION(StartElement);
   CXMLstate* pState = (CXMLstate*)pUserData;
   COLvar& Current = *pState->m_VarStack.top();
   COLstring Key(Name);
   COL_VAR(Key);
   COLvar* pNext;
   if (!Current.exists(Key)){
      if (pState->m_IsRoot){
         pNext = &Current[Key];   // Root is a single tag so we don't put it into an array. 
         pState->m_IsRoot = false;
      } else {
         pNext = &Current[Key][0];
      }
   } else {
      pNext = &Current[Key][Current[Key].array().size()];
   }
   COLvar& Next = *pNext;
   COLvar& Attributes = Next["@"];
   pState->m_VarStack.push(pNext);

   for (const XML_Char** i = atts; *i != NULL; i+=2){
      COL_TRC(*i << " = " << *(i+1));
      Attributes[*i] = *(i+1);
   }
}

static void EndElement(void* pUserData, const XML_Char* Name){
   COL_FUNCTION(EndElement);
   CXMLstate* pState = (CXMLstate*)pUserData;
   COL_VAR(Name);
   pState->m_VarStack.pop();
}

static void CXMLonCharacterData(void* pUserData, const XML_Char *s, int len){
   COL_FUNCTION(CXMLonCharacterData);
   CXMLstate* pState = (CXMLstate*)pUserData;
   COL_DUMP("Data:", s, len);
   COLstring Current =  pState->m_VarStack.top()->operator[]("text");
   Current.append(s, len);
   pState->m_VarStack.top()->operator[]("text") = Current;
}

bool CXMLparse(const char* pXml, int Size, COLvar* pResult){
   COL_FUNCTION(CXMLparse);
   CXMLstate State;
   XML_Parser Parser = ::XML_ParserCreate(NULL);
   ::XML_SetUserData(Parser, &State); 
   ::XML_SetElementHandler(Parser, StartElement,EndElement); 
   ::XML_SetCharacterDataHandler(Parser, CXMLonCharacterData);
   State.m_VarStack.push(pResult);
   bool Result = ::XML_Parse(Parser, pXml, Size, true);
   COL_VAR(Result);
   if ( ! Result ) {
      COL_TRC("XML ErrMsg: " << (char*) XML_ErrorString(XML_GetErrorCode(Parser)) );
      // dump the previous 128 B, 16 B starting at Index, and following 128 B (272 B in total)
      COL_DUMP("Dump XML (" << XML_GetCurrentByteIndex(Parser) << "):", XML_GetCurrentByteIndex(Parser) + pXml - 256, 528);
      COL_TRC( XML_GetErrorCode(Parser) );
      COL_TRC( XML_GetCurrentLineNumber(Parser) );
      COL_TRC( XML_GetCurrentColumnNumber(Parser) );
   }
      /* throw onError(XML_ErrorString(XML_GetErrorCode(Parser)),
                    XML_GetCurrentLineNumber(Parser),
                    XML_GetCurrentColumnNumber(Parser),
                    XML_GetCurrentByteIndex(Parser),
                    XML_GetCurrentByteCount(Parser));
                    */
   ::XML_ParserFree(Parser); 
   return Result;
}

bool COLvar::parseXml(const char* pXml, int Size){
   COL_METHOD(COLvar::parseXml);
   return CXMLparse(pXml, Size, this);  
}

bool COLvar::parseXml(const COLstring& Data){
   COL_METHOD(COLvar::parseXml);
   return CXMLparse(Data.c_str(), Data.size(), this);
}