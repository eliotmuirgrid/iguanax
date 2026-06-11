// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIformat
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 05 October 2022 - 08:29PM
// ---------------------------------------------------------------------------

#include <EDI/EDIformat.h>
#include <EDI/EDIdata.h>
#include <EDI/EDIgrammar.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// TODO - not bothering to support redefinition of HL7 special characters.
// it is a special case which may not be worth complicating the core of the design
// (one can convert to different special characters using string substitution afterwards or pre-processing HL7 data)
void EDIescapeData(COLstring* pOut, const COLstring& Data){
   COL_FUNCTION(EDIescapeData);   
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
}

void EDIformatSubField(const EDIdata& Data, COLstring* pOut){
    COL_FUNCTION(EDIformatSubField);
    if (Data.isLeaf()){
      COL_TRC("we have a leaf");
      EDIescapeData(pOut, Data.Data);  
      return;
    }
    int Max = Data.size();
    for (int i=0; i < Max; i++){
       EDIescapeData(pOut,Data[i].value());
       pOut->append("&");
    }
    pOut->setSize(pOut->size()-1);
}

void EDIformatField(const EDIdata& Data, COLstring* pOut){
    COL_FUNCTION(EDIformatField);
    if (Data.isLeaf()){
      // we have a leaf
      EDIescapeData(pOut, Data.value());
      return;
    }
    int Max = Data.size();
    for (int i=0; i < Max; i++){
       EDIformatSubField(Data[i], pOut);
       pOut->append("^");
    }
    pOut->setSize(pOut->size()-1);
}

void EDIformatRepeatingField(const EDIdata& Data, COLstring* pOut){
    COL_FUNCTION(EDIformatRepeatingField);
    int Max = Data.size();
    if (Max == 0){
       return;
    }
    for (int i=0; i < Max; i++){
       Data[i].flatwire(pOut);
       pOut->append("~");
    }
    pOut->setSize(pOut->size()-1);
}


void EDIformatSegment(const EDIdata& Data, COLstring* pOut){
   COL_FUNCTION(EDIformatSegment);
   if (Data.isNull()){
      return;
   }
   int Max = Data.size();
   *pOut += Data.name();
   for (int i=0; i < Max; i++){
      *pOut += '|';  // field separator
      Data[i].flatwire(pOut);
   }
   *pOut += COL_T("|\r"); // default to trailing field character   
}

void EDIformatMessage(const EDIdata& Data, COLstring* pOut){
   COL_FUNCTION(EDIformatMessage);
   int Max = Data.size();
   for (int i=0; i < Max; i++){
      Data[i].flatwire(pOut);
   }
}