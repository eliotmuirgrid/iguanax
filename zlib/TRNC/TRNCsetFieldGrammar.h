#ifndef __TRNC_SET_FIELD_GRAMMAR_H__
#define __TRNC_SET_FIELD_GRAMMAR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: TRNCsetFieldGrammar
//
// Author: Matthew Sobkowski
//
// Merges a new trnc impl with current field values -- used in translator gui for config.json
//---------------------------------------------------------------------------

class COLwebRequest;

void TRNCsetFieldGrammar(const COLwebRequest& Request);

#endif