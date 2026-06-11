#ifndef __DOC_RANDOM_TEST_H__
#define __DOC_RANDOM_TEST_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCrandomTest
//
// Description:
//
// API to generate random test data into a component
//
// Author: Eliot Muir 
// Date:   Sunday 24 August 2025 - 03:17PM
// ---------------------------------------------------------------------------

class COLwebRequest;
class COLstring;
class DOCdocument;

void DOCdocumentCreateRandom(DOCdocument* pDocument);

void DOCrandomTestGenerate(const COLstring& Id);
void DOCrandomTest(const COLwebRequest& Request);

#endif // end of defensive include
