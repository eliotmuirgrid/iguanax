//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HELPserialize
//
// Description:
//
// Implementation
//
// Author: Orkhan Amikishiyev
//---------------------------------------------------------------------------
#include <HELP/HELPserialize.h>
#include <HELP/HELPmodel.h>

#include <COL/COLvar.h>
#include <COL/COLstring.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void HELPreadParameters(HELPobject* pHelp, const COLvar& Data){
   COL_FUNCTION(HELPreadParameters);
   if (!Data.isArray()){
      COL_TRC("Parameters is not an array so we can return");
      return;
   }
   for (auto ParameterValue : Data.array()){
      HELPparameter Parameter;
      COL_VAR(ParameterValue);
      if (ParameterValue.exists("Name"))        { Parameter.Name        = ParameterValue["Name"];        }
      if (ParameterValue.exists("Description")) { Parameter.Description = ParameterValue["Description"]; }
      if (ParameterValue.exists("Required"))    { Parameter.Required    = ParameterValue["Required"];    }
      COL_VAR(Parameter);
      pHelp->Parameters.push_back(Parameter);
   }
}

void HELPmapParameters(COLvar* pOut, const COLarray<HELPparameter>& Parameters) {
   COL_FUNCTION(HELPmapParameters);
   COLvar& Data = *pOut;
   for (const auto& Parameter : Parameters){
      COL_VAR(Parameter);
      COLvar ParameterData;
      ParameterData["Required"]    = Parameter.Required;
      ParameterData["Name"]        = Parameter.Name;
      ParameterData["Description"] = Parameter.Description;
      COL_VAR(ParameterData);
      Data.push_back(ParameterData);
   }
   COL_VAR(Data);
}


void HELPtoObject (HELPobject* pHelp, const COLstring& Content){
   COL_FUNCTION(HELPtoObject);
   COL_VAR(Content);
   COLvar Data;
   Data.parse(Content);
   HELPmapObject(pHelp, Data);
}

void HELPmapObject(HELPobject* pHelp, const COLvar& Data){
   COL_FUNCTION(HELPmapObject);
   COL_VAR(Data.json(1)); // Orkhan - How to check if Data has valid JSON format?
   HELPobject& Help = *pHelp;
   if (Data.exists("SummaryLine"))    { Help.SummaryLine    = Data["SummaryLine"];     }
   if (Data.exists("Description"))    { Help.Description    = Data["Description"];     }
   if (Data.exists("ParameterTable")) { Help.ParameterTable = Data["ParameterTable"];  }
   if (Data.exists("Name"))           { Help.Name           = Data["Name"];            }
   if (Data.exists("Parameters"))     { HELPreadParameters(pHelp, Data["Parameters"]); }
   COL_VAR(Help);
}

void HELPtoVar(COLvar* pOut, const HELPobject& Help) {
   COL_VAR(Help);
   COLvar& Data           = *pOut;
   Data["Name"]           = Help.Name;
   Data["SummaryLine"]    = Help.SummaryLine;
   Data["Description"]    = Help.Description;
   Data["ParameterTable"] = Help.ParameterTable;
   HELPmapParameters (&Data["Parameters"], Help.Parameters);
   COL_VAR(Data.json(1));
}