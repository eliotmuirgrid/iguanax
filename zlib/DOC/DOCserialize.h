#ifndef __DOC_SERIALIZE_H__
#define __DOC_SERIALIZE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCserialize
//
// Description:
//
// Code to translate a DOCdocument object to and from JSON.
//
// Author: Eliot Muir 
// Date:   Tuesday 19 August 2025 - 09:27AM
// ---------------------------------------------------------------------------

#include <COL/COLarray.h>

class DOCdocument;
class COLstring;
class COLvar;
class DOCrelation;

void DOCtoJson(const DOCdocument& In, COLstring* pOut);
void DOCtoVar(const DOCdocument& In, COLvar* pOut);
void DOCfromJson(const COLstring& In, DOCdocument* pOut);
void DOCfromVar(const COLvar& In, DOCdocument *pOut);
void DOCtagsFromJson(const COLvar& In, DOCdocument* pDoc);
void DOCtagsToJson(const DOCdocument& Doc, COLvar* pOut);

void DOCrelatedFromJson(const COLvar& In, COLarray<DOCrelation>* pRelations);

void DOCsaveDoc(const DOCdocument& Doc, const COLstring& Id);
void DOCsaveOrig(const COLstring& Data, const DOCdocument& Doc, const COLstring& Component);
void DOCsaveMd  (const COLstring& Data, const DOCdocument& Doc, const COLstring& Component);

void DOCreadDoc(const COLstring& ComponentId, const COLstring& Id, const COLstring& CreatedHex, DOCdocument* pDoc);

//void DOCreadDoc (const COLstring& FileName, DOCdocument* pDoc);
//void DOCreadMd  (const COLstring& FileName, COLstring* pMd);

#endif // end of defensive include
