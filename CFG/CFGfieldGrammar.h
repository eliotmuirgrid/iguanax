#ifndef __CFG_FIELD_GRAMMAR_H__
#define __CFG_FIELD_GRAMMAR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CFGfieldGrammar
//
// Description: Custom field interface for grammar
//
// Author: Matthew Sobkowski
// Date:   Monday 24 April 2023 - 3:59PM
// ---------------------------------------------------------------------------
#include <COL/COLarray.h>

enum class CFGfieldType { BOOL, FILE, FOLDER, NUMBER, PASSWORD, PORT, STRING, List };

CFGfieldType CFGstringToFieldType(const COLstring& Type);
COLstring    CFGfieldTypeToString(const CFGfieldType& Type);
COLvar       CFGfieldsParseOptions(const COLstring& Options);


COLstring CMPCexpandEnvironmentVariables(const COLstring& TrueValue);

// This maps to the config.json in the translator
class CFGfield {
  public:
   CFGfield(const COLstring& vName, const CFGfieldType& vType, const COLstring& vDefault, const COLstring& vOptions,
            const COLstring& vDescription = "")
       : Name(vName), FieldType(vType), Default(vDefault), Options(vOptions), Description(vDescription) {}
   COLstring    Name;
   CFGfieldType FieldType;
   COLstring    Default;
   COLstring    Options;  // csv list
   COLstring    Description;
};

COLostream& operator<<(COLostream& Stream, const COLarray<CFGfield>& Grammar);

bool CFGfieldIsValid(const COLarray<CFGfield>& Grammar, const COLstring& Name, COLstring& Value, COLstring* pError);

bool CFGfieldCreateGrammar(const COLvar& Data, COLarray<CFGfield>* pGrammar, COLstring* pErr);
bool CFGfieldLoadGrammar(const COLstring& Path, COLarray<CFGfield>* pGrammar, COLstring* pErr);
bool CFGfieldSaveGrammar(const COLstring& Component, const COLarray<CFGfield>& Grammar, COLstring* pErr,
                         bool DevMode = false);

#endif  // end of defensive include
