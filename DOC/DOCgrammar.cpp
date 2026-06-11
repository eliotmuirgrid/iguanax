// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCgrammar
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 26 August 2025 - 12:54PM
// ---------------------------------------------------------------------------

#include <DOC/DOCgrammar.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

DOCgrammar::DOCgrammar(NodeType iType, const COLstring& iName, bool iRequired) 
: Type(iType), Name(iName), Required(iRequired){
   COL_METHOD(DOCgrammar::DOCgrammar);
}

const DOCgrammar* DOCgrammarFindChild(const DOCgrammar& Grammar, const COLstring& Name){
   COL_FUNCTION(DOCgrammarFindChild);
   for (int i =0; i < Grammar.Children.size(); i++){
      if (Grammar.Children[i].Name == Name){
         return &Grammar.Children[i];
      }
   }
   return NULL;
}

bool DOCgrammarCheckDict(const COLvar& Data, const DOCgrammar& Grammar, COLstring* pError){
   COL_FUNCTION(DOCgrammarCheckDict);
   if (!Data.isMap()){
      *pError = "This should be a table.";
      return false;
   }  
   for (auto i = Data.map().cbegin(); i != Data.map().cend(); i++){
      // we do a inefficient loop through all the Children
      const DOCgrammar* pChild = DOCgrammarFindChild(Grammar, i->first);
      if (!pChild){
         *pError = "Did not expect key " + i->first;
         return false;
      } else {
         if (!DOCgrammarCheck(i->second, *pChild, pError)){
            *pError = i->first + *pError;
            return false;
         }
      }
   }
      // iterate through all the children grammar and check for the existence of required members
   for (int i=0; i < Grammar.Children.size(); i++){
      if (Grammar.Children[i].Required && !Data.exists(Grammar.Children[i].Name)){
         *pError = "Required key " + Grammar.Children[i].Name + " not found.";
         return false;
      }
   }
   return true;
}

bool DOCgrammarCheckArray(const COLvar& Data, const DOCgrammar& Grammar, COLstring* pError){
   COL_FUNCTION(DOCgrammarCheckArray);
   if (!Data.isArray()){
      return false;
   }
   if (Grammar.Children.size() != 1){
      *pError = "Array grammar needs one child.";
   }
   const COLvar::COLvarVec& List = Data.array();
   for (int i=0; i < List.size(); i++){
      if (!DOCgrammarCheck(List[i], Grammar.Children[0], pError)){
         *pError = "[" + COLintToString(i+1) + "] " + *pError; 
         return false;
      }
   }
   return true;
}

bool DOCgrammarCheckString(const COLvar& Data, const DOCgrammar& Grammar, COLstring* pError){
   COL_FUNCTION(DOCgrammarCheckString);
   if (!Data.isString()) {
      *pError = "Not string";
      return false;
   }
   return true; 
}

bool DOCgrammarCheckInteger(const COLvar& Data, const DOCgrammar& Grammar, COLstring* pError){
   COL_FUNCTION(DOCgrammarCheckInteger);
   if (!Data.isInteger()) {
      *pError = "Not integer";
      return false;
   }
   return true; 
}

bool DOCgrammarCheckBool(const COLvar& Data, const DOCgrammar& Grammar, COLstring* pError){
   COL_FUNCTION(DOCgrammarCheckBool);
   if (!Data.isBool()) {
      *pError = "Not boolean";
      return false;
   }
   return true;
}

bool DOCgrammarCheck(const COLvar& Data, const DOCgrammar& Grammar, COLstring* pError){
   COL_FUNCTION(DOCgrammarCheck);
   switch (Grammar.Type){
   case DOCgrammar::Dictionary: return DOCgrammarCheckDict   (Data, Grammar, pError); 
   case DOCgrammar::Array:      return DOCgrammarCheckArray  (Data, Grammar, pError);
   case DOCgrammar::String:     return DOCgrammarCheckString (Data, Grammar, pError);
   case DOCgrammar::Integer:    return DOCgrammarCheckInteger(Data, Grammar, pError);
   case DOCgrammar::Bool:       return DOCgrammarCheckBool   (Data, Grammar, pError);
   default: COL_ERR("We haven't coded this type!"); return false;
   };
   return true;
}
