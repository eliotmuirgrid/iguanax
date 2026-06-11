//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMsink
//
// Description:
//
// Parses XML data to populate the DOM tree.
//
// Author: Rob Moyse
// Date:   November 9, 2004
//
//---------------------------------------------------------------------------
#include "DOMpreCompiled.h"
#pragma hdrstop

#include <COL/COLerror.h>

#include <DOM/DOMnode.h>
#include <DOM/DOMelement.h>
#include <DOM/DOMdocument.h>
#include <DOM/DOMcomment.h>
#include <DOM/DOMcdataSection.h>
#include <DOM/DOMattribute.h>
#include <DOM/DOMtext.h>

#include <DOM/DOMsink.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

class DOMsinkPrivate
{
public:

   DOMdocument* pCurrentDocument;
   DOMnode* pCurrentNode;
   DOMcharacterData* pCurrentCharacterDataNode;
};


DOMsink::DOMsink(DOMdocument& Document, COLuint32 DocumentSize)
{
   COL_METHOD(DOMsink::DOMsink);
   pMember = new DOMsinkPrivate();

   Document.dataBuffer().setCapacity(DocumentSize);
   Document.dataBuffer().setSize(0);
   pMember->pCurrentDocument = &Document;
   pMember->pCurrentNode = &Document;
   pMember->pCurrentCharacterDataNode = NULL;
}


DOMsink::~DOMsink()
{
   COL_METHOD(DOMsink::~DOMsink);
   delete pMember;
}


COLuint32 DOMsink::write( const void* cpBuffer, COLuint32 SizeOfBuffer ) {
   COL_METHOD(DOMsink::write);
   init();
   parseBuffer((char*)cpBuffer, SizeOfBuffer, false);
   return SizeOfBuffer;
}

void DOMsink::flush() {
   COL_METHOD(DOMsink::flush);
   parseBuffer(0,0,true);
}

void DOMsink::onStartElement(const char* pName, const char** ppAttributes) {
   COL_METHOD(DOMsink::onStartElement);
   COL_VAR(pName);
   pMember->pCurrentCharacterDataNode = NULL;
 
   COLuint32 Length;
   const char* pTagName = copyValueToDocumentBuffer(pName, &Length);

   DOMelement* pNewElement = new DOMelement(pMember->pCurrentNode, 
         pMember->pCurrentDocument, pTagName, Length);
      
   pMember->pCurrentNode->appendChild(pNewElement);
   pMember->pCurrentNode = pNewElement;

   // collect the attributes
   while (ppAttributes[0] != 0) {
      COLuint32 AttributeNameLength;
      const char* pAttributeName = copyValueToDocumentBuffer(ppAttributes[0], &AttributeNameLength);

      COLuint32 AttributeValueLength;
      const char* pAttributeValue = copyValueToDocumentBuffer(ppAttributes[1], &AttributeValueLength);

      COL_VAR2(pAttributeName, pAttributeValue);
      pNewElement->setAttribute(pAttributeName, AttributeNameLength, pAttributeValue, AttributeValueLength);
      ppAttributes += 2;
   }
}

void DOMsink::onEndElement(const char* Name)
{
   COL_METHOD(DOMsink::onEndElement);
   COL_VAR(Name);
   pMember->pCurrentCharacterDataNode = NULL;
   pMember->pCurrentNode = pMember->pCurrentNode->parentNode();
}

void DOMsink::onCharacterData(const char* Data, int Length)
{
   COL_METHOD(DOMsink::onCharacterData);
   COL_VAR2(Data, Length);
   if (!pMember->pCurrentCharacterDataNode) {
      DOMtext* pNewText = new DOMtext(pMember->pCurrentNode,
            pMember->pCurrentDocument, NULL, 0);
      
      pMember->pCurrentNode->appendChild(pNewText);
      pMember->pCurrentCharacterDataNode = pNewText;
   }

   COLuint32 LengthCheck;
   pMember->pCurrentCharacterDataNode->nodeValue(&LengthCheck);
   if (LengthCheck == 0) {
      const char* pData = copyValueToDocumentBuffer(Data, Length);
      pMember->pCurrentCharacterDataNode->setNodeValue(pData, Length);
   } else {
      copyValueToDocumentBuffer(Data, Length);
      pMember->pCurrentCharacterDataNode->appendLength(Length);
   }
}

void DOMsink::onComment(const char* pActualData)
{
   COL_METHOD(DOMsink::onComment);
   pMember->pCurrentCharacterDataNode = NULL;

   COLuint32 Length;
   const char* pData = copyValueToDocumentBuffer(pActualData, &Length);

   DOMcomment* pNewComment = new DOMcomment(pMember->pCurrentNode,
         pMember->pCurrentDocument, pData, Length);

   pMember->pCurrentNode->appendChild(pNewComment);
}

void DOMsink::onStartCdataSection() {
   COL_METHOD(DOMsink::onStartCdataSection);
   DOMcdataSection* pNewCdataSection = new DOMcdataSection(
         pMember->pCurrentNode, pMember->pCurrentDocument, NULL, 0);
      
   pMember->pCurrentNode->appendChild(pNewCdataSection);
   pMember->pCurrentCharacterDataNode = pNewCdataSection;
}

void DOMsink::onEndCdataSection() {
   COL_METHOD(DOMsink::onEndCdataSection);
   pMember->pCurrentCharacterDataNode = NULL;
}

const char* DOMsink::copyValueToDocumentBuffer(const char* pName, COLuint32* pLength) const {
   COL_METHOD(DOMsink::copyValueToDocumentBuffer);
   COLPRECONDITION(pMember->pCurrentDocument != NULL)
   COLstring& Buffer = pMember->pCurrentDocument->dataBuffer();

   const char* pReturnValue = (char*)(Buffer.data() + Buffer.size());
   
   COLuint32 Length = strlen(pName);
   Buffer.write(pName, (Length+1) * sizeof(char) );
   *pLength = Length;

   return pReturnValue;
}

const char* DOMsink::copyValueToDocumentBuffer(const char* pName, COLuint32 Length) const {
   COL_METHOD(DOMsink::copyValueToDocumentBuffer);
   COL_VAR2(pName, Length);
   COLPRECONDITION(pMember->pCurrentDocument != NULL)
   COLstring& Buffer = pMember->pCurrentDocument->dataBuffer();

   const char* pReturnValue = (char*)(Buffer.data() + Buffer.size());
   Buffer.write(pName, Length * sizeof(char));
   return pReturnValue;
}

