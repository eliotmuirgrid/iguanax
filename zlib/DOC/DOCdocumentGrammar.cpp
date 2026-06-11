// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCdocumentGrammar
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 26 August 2025 - 03:18PM
// ---------------------------------------------------------------------------

#include <DOC/DOCdocumentGrammar.h>
#include <DOC/DOCgrammar.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

DOCgrammar DOCdocumentGrammarSetup(){
   DOCgrammar Grammar(DOCgrammar::Dictionary, "", true);
   Grammar.Children.push_back(DOCgrammar(DOCgrammar::String , "data",          true));
   Grammar.Children.push_back(DOCgrammar(DOCgrammar::String , "title",         true));
   Grammar.Children.push_back(DOCgrammar(DOCgrammar::Integer, "created",       true));
   Grammar.Children.push_back(DOCgrammar(DOCgrammar::String, "source",         true));
   Grammar.Children.push_back(DOCgrammar(DOCgrammar::String, "component",      true)); 
   Grammar.Children.push_back(DOCgrammar(DOCgrammar::String, "markdown",       true));
   
   Grammar.Children.push_back(DOCgrammar(DOCgrammar::String, "id",      false));
   Grammar.Children.push_back(DOCgrammar(DOCgrammar::Array, "tags",      true)).Children.push_back(DOCgrammar(DOCgrammar::String));
   
   DOCgrammar& RelationsArray = Grammar.Children.push_back(DOCgrammar(DOCgrammar::Array  , "relations", false));
   DOCgrammar& Relation = RelationsArray.Children.push_back(DOCgrammar(DOCgrammar::Dictionary, "", false));
   Relation.Children.push_back(DOCgrammar(DOCgrammar::String, "id",       true));
   Relation.Children.push_back(DOCgrammar(DOCgrammar::String, "relation", true));
   Relation.Children.push_back(DOCgrammar(DOCgrammar::String, "created",  true));
   Relation.Children.push_back(DOCgrammar(DOCgrammar::String, "title",    true));
   
   return Grammar;
}

const DOCgrammar& DOCdocumentGrammar(){
   static DOCgrammar Grammar = DOCdocumentGrammarSetup();
   return Grammar;
}