// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIformatHtml
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 24 October 2022 - 02:44PM
// ---------------------------------------------------------------------------

#include <EDI/EDIformatHtml.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <EDI/EDIformatHtml.h>
#include <EDI/EDIdata.h>
#include <EDI/EDIgrammar.h>

// TODO - not bothering to support redefinition of HL7 special characters.
// it is a special case which may not be worth complicating the core of the design
// (one can convert to different special characters using string substitution afterwards or pre-processing HL7 data)
void EDIescapeDataHtml(COLstring* pOut, const COLstring& Data){
   COL_FUNCTION(EDIescapeDataHtml);   
   *pOut += COL_T("<span class='clrz_str'>");
   const char* pEnd = Data.c_str() + Data.size();
   for (const char* i = Data.c_str(); i != pEnd; i++){
      COL_VAR(*i);
      switch (*i){
      case '&' : *pOut += COL_T("\\T\\"); break;
      case '^' : *pOut += COL_T("\\S\\"); break;
      case '|' : *pOut += COL_T("\\F\\"); break;
      case '~' : *pOut += COL_T("\\R\\"); break;
      case '\\': *pOut += COL_T("\\E\\"); break;
      default : *pOut += *i; break;
      }
   }
   *pOut += COL_T("</span>");
}

void EDIformatSubFieldHtml(const EDIdata& Data, COLstring* pOut){
    COL_FUNCTION(EDIformatSubFieldHtml);
    if (Data.isLeaf()){
      COL_TRC("we have a leaf");
      EDIescapeDataHtml(pOut, Data.Data);  

      return;
    }
    int Max = Data.size();
    for (int i=0; i < Max; i++){
       EDIescapeDataHtml(pOut,Data[i].value());
       pOut->append("&");
    }
    pOut->setSize(pOut->size()-1);
}

void EDIformatFieldHtml(const EDIdata& Data, COLstring* pOut){
    COL_FUNCTION(EDIformatFieldHtml);
    if (Data.isLeaf()){
      // we have a leaf
      EDIescapeDataHtml(pOut, Data.value());
      return;
    }
    int Max = Data.size();
    for (int i=0; i < Max; i++){
       EDIformatSubFieldHtml(Data[i], pOut);
       pOut->append("^");
    }
    pOut->setSize(pOut->size()-1);
}

void EDIformatRepeatingFieldHtml(const EDIdata& Data, COLstring* pOut){
    COL_FUNCTION(EDIformatRepeatingFieldHtml);
    int Max = Data.size();
    if (Max == 0){
       return;
    }
    for (int i=0; i < Max; i++){
       Data[i].flatwireHtml(pOut);
       pOut->append("~");
    }
    pOut->setSize(pOut->size()-1);
}


void EDIformatSegmentHtml(const EDIdata& Data, COLstring* pOut){
   COL_FUNCTION(EDIformatSegmentHtml);
   int Max = Data.size();
   *pOut += COL_T("<span class='clrz_name'>") + Data.name() + COL_T("</span>"); 
   for (int i=0; i < Max; i++){
      *pOut += '|';  // field separator
      Data[i].flatwireHtml(pOut);
   }
   *pOut += COL_T("|\r"); // default to trailing field character   
}

void EDIformatMessageHtml(const EDIdata& Data, COLstring* pOut){
   COL_FUNCTION(EDIformatMessageHtml);
   int Max = Data.size();
   for (int i=0; i < Max; i++){
      Data[i].flatwireHtml(pOut);
   }
}