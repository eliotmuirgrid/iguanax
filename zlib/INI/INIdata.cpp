// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INIdata
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 24 March 2023 - 08:43AM
// ---------------------------------------------------------------------------

#include <INI/INIdata.h>

#include <FIL/FILutils.h>

#include <COL/COLarray.h>
#include <COL/COLsplit.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring INIformatSsh(const COLstring& SshCommand) {
	COL_FUNCTION(INIformatSsh);
	COLstring Command = SshCommand;
	size_t PathStart = Command.find("-i");
	if(PathStart == npos || SshCommand[SshCommand.size() - 1] == '"') { return Command; }
	Command.insert(PathStart + 3, "\"");
	Command.append("\"");
	return Command;

}

void INIformat(const INIdata& Config, COLstring* pOut){
   COL_FUNCTION(INIformat);
   COLstring& Out = *pOut;
   auto& Data = Config.PrivateData;
   for (auto i = Data.cbegin(); i != Data.cend(); i++ ){
      Out += '[' + i->first + COL_T("]\n");
      for (auto j = i->second.cbegin(); j != i->second.cend(); j++){
			if(j->first == "sshCommand") {
				Out += '\t' + j->first + COL_T(" = ") + INIformatSsh(j->second) + '\n';
			}else {
				Out += '\t' + j->first + COL_T(" = ") + j->second + '\n';
			}
      }
   }
   COL_VAR(Out);
   COL_DUMP("Data:", Out.c_str(), Out.size());
}

bool INIisSection(const COLstring& Line){
   COL_FUNCTION(INIisSection);
   bool IsSection = (Line.size() > 0 && Line[0] == '[');
   COL_VAR2(IsSection, Line);
   return IsSection;
}

COLstring INIgetSection(const COLstring& Line){
   COL_FUNCTION(INIgetSection);
   COLstring Section;
   Section = Line.strip(COLstring::Both, ' ');
   Section = Section.strip(COLstring::Trailing, ']');
   Section = Section.strip(COLstring::Leading,  '[');
   COL_VAR2(Line, Section);
   return Section;
}

bool INIisValue(const COLstring& Line){
   COL_FUNCTION(INIisValue);
   bool IsValue = Line.find("=") != npos;
   COL_VAR2(IsValue, Line);
   return IsValue;
}

void INIparseValue(const COLstring& Line, COLstring* pKey, COLstring* pValue){
   COL_FUNCTION(INIparseValue);
   Line.split(*pKey, *pValue, "=");
   *pKey = pKey->strip(COLstring::Both, '\t');
   *pKey = pKey->strip(COLstring::Both, ' ');
   *pValue = pValue->strip(COLstring::Both, '\t');
   *pValue = pValue->strip(COLstring::Both, ' ');
   COL_VAR3(*pKey, *pValue, Line);
}

// My algorithm is basic and dumb - but probably 'good enough'
void INIparse(const COLstring& Input, INIdata* pConfig){
   COL_FUNCTION(INIparse);
   auto& Collection = pConfig->PrivateData;
   COLarray<COLstring> Lines;
   COLstring Data = Input;
   Data.replace("\r\n", "\n");  // For windoze
   COLsplit(&Lines, Data, "\n");
   if (Lines.size() == 0){
      COL_TRC("Empty config file.");
      return;
   }
   COLstring CurrentSection;
   for(int i=0; i < Lines.size(); i++){
      COLstring Line = Lines[i];
      if (INIisSection(Line)){
         COL_TRC(Line << " is section.");
         CurrentSection = INIgetSection(Line);
         COL_VAR(CurrentSection);
      } else if (INIisValue(Line)){
         COLstring Key, Value;
         INIparseValue(Line, &Key, &Value);
         Collection[CurrentSection][Key] = Value;
      }
   }
}

COLstring INIsectionName(const COLstring& Section, const COLstring& SubSection){
   COL_FUNCTION(INIsectionName);
   COLstring Result = Section;
   if (!SubSection.is_null()){
      Result += COL_T(" \"") + SubSection + COL_T("\"");
   }
   COL_VAR(Result);
   return Result;
}

COLstring INIdataKeyGet(const INIdata& Config, const COLstring& Section, const COLstring& SubSection, const COLstring& Key){
   COL_FUNCTION(INIdataKeyGet);
   COL_VAR3(Section,SubSection,Key);
   COLstring SectionKey = INIsectionName(Section, SubSection);
   COLstring Result;
   if (Config.PrivateData.count(SectionKey) != 0){
      if (Config.PrivateData[SectionKey].count(Key) != 0){
         COL_TRC("Section/Value exists");
         Result = Config.PrivateData[SectionKey][Key];
      }
   }
   COL_VAR(Result);
   return Result;
}

void INIdataKeySet(INIdata* pData, const COLstring& Section, const COLstring& SubSection, const COLstring& Key, const COLstring& Value){
   COL_FUNCTION(INIdataKeySet);
   COL_VAR4(Section,SubSection,Key,Value);
   COLstring SectionKey = INIsectionName(Section, SubSection);
   pData->PrivateData[SectionKey][Key] = Value;
}

void INIdataKeyRemove(INIdata* pConfig, const COLstring& Section, const COLstring& SubSection, const COLstring& Key){
   COL_FUNCTION(INIdataKeyRemove);
   INIdata& Config = *pConfig;
   COL_VAR3(Section,SubSection, Key);
   COLstring SectionKey = INIsectionName(Section, SubSection);
   if (Config.PrivateData.count(SectionKey) != 0){
      Config.PrivateData[SectionKey].removeKey(Key);
   }
}

void INIdataSectionRemove(INIdata* pConfig, const COLstring& Section, const COLstring& SubSection){
   COL_FUNCTION(INIdataSectionRemove);
   COL_VAR(Section);
   COLstring Key = INIsectionName(Section, SubSection);
   COL_VAR(Key);
   pConfig->PrivateData.erase(Key);
}

COLostream& operator<<(COLostream& Stream, const INIdata& Data){
   COLstring Out;
   INIformat(Data, &Out);
   Stream << Out;
   return Stream;
}
