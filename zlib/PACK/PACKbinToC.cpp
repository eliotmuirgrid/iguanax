// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PACKbinToC
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 16 August 2023 - 10:07AM
// ---------------------------------------------------------------------------

#include <PACK/PACKbinToC.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void PACKbinLine(COLstring* pOut, const char* pStart, const char* pEnd){
   COL_FUNCTION(PACKbinLine);
   COL_VAR(pEnd-pStart);
   static const char Hex[] = "0123456789abcdef?";
   for (const char* pCursor = pStart; pCursor < pEnd; pCursor++){
      char Digit1 = Hex[(*pCursor & 0xf0) >> 4];
      char Digit2 = Hex[(*pCursor & 0x0f)];
      *pOut += COL_T("0x") + Digit1 + Digit2 + ',';
   }
   *pOut += '\n';
}

void PACKbinToC(const COLstring& In, COLstring* pOut){
   COL_FUNCTION(PACKbinToC);
   COLstring& Out = *pOut;
   Out = "/* GENERATED FILE */\nunsigned char PACKdata[] = {\n";
   int Line = 0;
   int Columns = 8;
   COL_VAR(In.size());
   int RoundEnd = (In.size() / Columns) * Columns;
   COL_VAR(RoundEnd); 
   const char* pEnd = In.c_str() + RoundEnd;
   for (const char* pStart=In.c_str(); pStart < pEnd; pStart +=Columns){
      Line++;
      PACKbinLine(pOut, pStart, pStart + Columns);
   }
   PACKbinLine(pOut, In.c_str()+RoundEnd, In.c_str()+ In.size());
   Out.setSize(Out.size()-2);
   COL_VAR(Line);
   Out += "\n};\n\n";
   Out += "unsigned int PACKdataSize = " + COLintToString(In.size()) + ";";
}