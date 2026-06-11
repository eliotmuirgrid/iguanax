//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HELPobject
//
// Description:
//
// Implementation
//
// Author: Orkhan Amikishiyev
//---------------------------------------------------------------------------
#include "HELPmodel.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLostream& operator<<(COLostream& Stream, const HELPobject& Object){
   Stream << "Help:"                                     << newline
          << "SummaryLine: "    << Object.SummaryLine    << newline
          << "Description: "    << Object.Description    << newline
          << "ParameterTable: " << Object.ParameterTable << newline;
   Stream << newline << "Parameters: ";
   for (auto Parameter : Object.Parameters){ Stream << "-> " << Parameter << newline; }
   return Stream;
}

COLostream& operator<<(COLostream& Stream, const HELPparameter& Parameter){
   Stream << "Name: " << Parameter.Name << " Required: " << Parameter.Required
          << " Description: " << Parameter.Description;
   return Stream;
}