// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INIsyntaxCheck
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Thursday 24 August 2023 - 01:39PM
// ---------------------------------------------------------------------------

#include <INI/INIsyntaxCheck.h>
#include <INI/INIdata.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Need to create a map of the expected keys 

// First check to see if there is a section name that we do not expect or if there are no sections

// If we see a section we didn’t expect, say the sections we expect

// So if we create a ini file with a [life] section it will tell us the sections we know

// If it is empty say we recognize these sections

// Have an external link in all cases to documentation on it.

// If we see a key in a section we don’t expect

// List all keys that we know.

bool INIcontainsNoSections(const INIdata& Data){
   COL_FUNCTION(INIcontainsNoSections);
   bool NoSections = Data.PrivateData.size() == 0;
   COL_VAR(NoSections);
   if(NoSections){
      COL_TRC("Contains no section headers");
   }
   else{
      COL_TRC("Contains section headers");
   }
   return NoSections;
}

bool INIcontainsUnexpectedSection(const INIdata& Expected, const INIdata& Data){
   COL_FUNCTION(INIcontainsUnexpectedSection);
   for (auto i=Data.PrivateData.begin(); i != Data.PrivateData.end(); ++i){
      COLstring Section = i->first;
      COL_VAR(Section);
      if(Expected.PrivateData.count(Section) == 0){
         COL_TRC("Found a section in the provided data that we did not expect: " << Section);
         return true;
      }
   }
   return false;
}

bool INIsectionContainsUnexpectedKey(const INIdata& Expected, const INIdata& Data, const COLstring& Section){
   COL_FUNCTION(INIsectionContainsUnexpectedKey);
   const COLmap<COLstring, COLstring>& ExpectedSection = Expected.PrivateData[Section];
   const COLmap<COLstring, COLstring>& DataSection     = Data.PrivateData[Section];
   for (auto i=DataSection.begin(); i != DataSection.end(); ++i){
      COLstring Key = i->first;
      if(ExpectedSection.count(Key) == 0){
         COL_TRC("Found a key in the '" << Section << "' section of the provided data that we did not expect: " << Key);
         return true;
      }
   }
   return false;
}

void INIoutputExpectedKeysInline(const INIdata& Expected, const COLstring& Section, COLostream& Out){
   COL_FUNCTION(INIoutputExpectedKeysInline);
   const COLmap<COLstring, COLstring>& ExpectedSection = Expected.PrivateData[Section];
   Out << "<";
   for (auto i=ExpectedSection.begin(); i != ExpectedSection.end(); ++i){
      if(i != ExpectedSection.begin()){
         Out << "|";
      }
      Out << i->first;
   }
   Out << ">";
}

void INIoutputExpectedSections(const INIdata& Expected, COLostream& Out){
   COL_FUNCTION(INIoutputExpectedSections);
   Out << "These are the expected sections and their keys: "<< newline;
   for (auto i=Expected.PrivateData.begin(); i != Expected.PrivateData.end(); ++i){
      COLstring Section = i->first;
      Out << "   - [" << Section << "]  "; 
      INIoutputExpectedKeysInline(Expected, Section, Out); 
      Out << newline; // maybe list all the keys here same way we do service actions < | >
   }
}

void INIoutputSectionsExpectedKeys(const INIdata& Expected, const COLstring& Section, COLostream& Out){
   COL_FUNCTION(INIoutputSectionsExpectedKeys);
   const COLmap<COLstring, COLstring>& ExpectedSection = Expected.PrivateData[Section];
   Out << "For [" << Section << "] these are the expected keys: " << newline;
   for (auto i=ExpectedSection.begin(); i != ExpectedSection.end(); ++i){
      Out << "   - " << i->first << newline;
   }
}

bool INIcheckAllSectionKeys(const INIdata& Expected, const INIdata& Data, COLostream& Out){
   COL_FUNCTION(INIcheckAllSectionKeys);
   bool ErrorFree = true;
   for (auto i=Expected.PrivateData.begin(); i != Expected.PrivateData.end(); ++i){
      COLstring Section = i->first;
      if(INIsectionContainsUnexpectedKey(Expected, Data, Section)){
         if(ErrorFree){
            Out << "Contains unexpected key(s)." << newline << newline;
         }
         INIoutputSectionsExpectedKeys(Expected, Section, Out);
         ErrorFree = false;
      }
   }
   return ErrorFree;
}

// Returns false if it should error out
bool INIsyntaxCheck(const INIdata& Expected, const INIdata& Data, COLstring* pOutput){
   COL_FUNCTION(INIsyntaxCheck);
   COLstring& Output = *pOutput;
   COLostream Out(Output);
   if(INIcontainsNoSections(Data)){
      Out << "Contains no sections or all the sections are empty." << newline << newline; 
      INIoutputExpectedSections(Expected, Out);
      return false;
   }
   if(INIcontainsUnexpectedSection(Expected, Data)){
      Out << "Contains unexpected section(s)." << newline << newline; 
      INIoutputExpectedSections(Expected, Out);
      return false;
   };
   if(!INIcheckAllSectionKeys(Expected, Data, Out)){
      return false;
   }
   return true;
}