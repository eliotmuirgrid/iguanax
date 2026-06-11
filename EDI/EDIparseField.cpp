// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIparseField
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 16 March 2023 - 12:23PM
// ---------------------------------------------------------------------------

#include <EDI/EDIparseField.h>
#include <EDI/EDIdata.h>
#include <EDI/EDIgrammar.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void EDIparseLeaf(EDIdata* pNode, const char* pData, int Size, COLarray<COLstring>* pHere){
   COL_FUNCTION(EDIparseLeaf);
   COL_DUMP("Token:", pData, Size);
   COLstring Value;
   const char* i = pData;
   while (i != pData + Size){
      if (*i == '\\' && *(i+2) == '\\'){
         switch (*(i+1)){
            case 'T': Value += "\\T\\";  i = i+2; break;
            case 'S': Value += "\\S\\";      i = i+2; break;
            case 'F': Value += "\\F\\";      i = i+2; break;
            case 'R': Value += "\\R\\";      i = i+2; break;
            case 'E': Value += '\\';     i = i+2; break;
            case 'H': Value += "\\H\\";  i = i+2; break;
            case 'N': Value += "\\N\\";  i = i+2; break;
            default:
               COL_TRC("Hit uncaught escaped sequence");
               break;
         }
      } else {
         Value += *i;
      }
      i++;
   }
   pNode->setValue(Value, pHere);
}

// We use C style strings to avoid unnecessary memory allocation during parsing.
void EDIparseSubField(EDIdata* pNode, const char* pData, int Size, COLarray<COLstring>* pHere){
   COL_FUNCTION(EDIparseSubField);
   COL_DUMP("Subfield Data:", pData, Size);
   if (Size == 0){
      COL_TRC("Zero size - so we return.");
      return;
   }
   const char* pEnd = pData + Size;
   const char* i = pData;
   const char* pStart = i;
   if (pNode->isLeaf()){
      COL_VAR(*pNode->pParentType);
      COL_TRC("There are no subsubfields in this subfield.");
      while (i != pEnd && *i != '&'){
         i++;
      }
      EDIparseLeaf(pNode, pData, i-pData, pHere);
      return;
   }
   int Count =0;
   while (i != pEnd){
      if (Count == pNode->grammarSize()){
         COL_TRC("We have reached our subfield limit."); // TODO we could make a collection of these warnings
         return;
      }
      if (*i == '&'){
         // pHere->push_back(COLintToString(Count + 1));
         pHere->push_back((*pNode)[Count].name());
         EDIparseLeaf(&(*pNode)[Count], pStart, i-pStart, pHere);
         pHere->pop_back();
         i++;
         pStart = i;
         Count++;
         if (*i == '&') { continue; } // this deals with multiple & in a row
      }
      if (i > pEnd) { pStart = i; break; } // this deals with a trailing &
      i++;
   }
   // pHere->push_back(COLintToString(Count + 1));
   pHere->push_back((*pNode)[Count].name());
   EDIparseLeaf(&(*pNode)[Count], pStart, i-pStart, pHere);
   pHere->pop_back();
}

bool EDIcharIsFieldTerminator(unsigned char C){
   switch(C){
   case '^':
   case '&':
   case '\r':
      return true;  
   default: return false;
   }
}

void EDIparseField(EDIdata* pNode, const char* pData, int Size, COLarray<COLstring>* pHere){
   COL_FUNCTION(EDIparseField);
   COL_TRC_DUMP("Field Data:", pData, Size);
   if (Size == 0){
      COL_TRC("Zero size so we return.");
      return;
   }
   const char* pEnd = pData + Size;
   const char* i = pData;
   const char* pStart = i;
   int Count = 0;
   if (pNode->isLeaf()){
      COL_TRC("There are no subfields in this field.");
      while (i != pEnd && !EDIcharIsFieldTerminator(*i)){   // TODO - could we have a unit that checks if ~ code may be correct - not sure.
         i++;
      }
      EDIparseLeaf(pNode, pData, i-pData, pHere);
      return;
   }
   while (i < pEnd){   // TODO - deal with trailing ^ boundary error - Vismay : after work on IX-2918 I think this is tested and handled
      if (*i == '^'){
         // pHere->push_back(COLintToString(Count + 1));
         pHere->push_back((*pNode)[Count].name());
         EDIparseSubField(&(*pNode)[Count], pStart, i-pStart, pHere);
         pHere->pop_back();
         COL_VAR2(pNode->size(), pNode->grammarSize());
         pStart = i+1;
         Count++;
      }
      i++;
      if (Count == pNode->grammarSize()){
         COL_VAR3(Count, pNode->size(), pNode->grammarSize());
         COL_TRC("We have reached our field limit."); // TODO we could make a collection of these warnings OR we could just accept the extra data and not label it.
         return;
      }
   }
   COL_TRC("Exited while loop");
   if (i > pEnd){
      COL_TRC("Past end of field, so returning.");
      return;
   }
   // TODO - TEST boundary condition when the next statement puts us over our limit
   // pHere->push_back(COLintToString(Count + 1));
   pHere->push_back((*pNode)[Count].name());
   EDIparseSubField(&(*pNode)[Count], pStart, i-pStart, pHere);
   pHere->pop_back();
   COL_VAR2(pNode->size(), pNode->grammarSize());
}

void EDIparseRepeatingField(EDIdata* pNode, const char* pData, int Size, COLarray<COLstring>* pHere){
   COL_FUNCTION(EDIparseRepeatingField);
   COL_DUMP("Repeating Field Data:", pData, Size);
   if (Size == 0){
      return;
   }
   const char* pEnd = pData + Size;
   const char* i = pData;
   const char* pStart = i;
   int Count = 0;
   while (i != pEnd){    
      if (*i == '~'){
         COL_DUMP("Field:", pStart, i-pStart);
         EDIparseField(&(*pNode)[Count], pStart, i-pStart, pHere);
         COL_VAR(pNode->size());
         Count++;
         i++;
         pStart = i;
         if (*i == '~') { continue; } // this deals with multiple ~ in a row, ex: ID1^Eliot~ID2^Bro~~~IDX 
      }
      if (i > pEnd) { break; } // this deals with trailing ~ boundary error
      i++;
   }
   COL_DUMP("Field:", pStart, i-pStart);
   EDIparseField(&(*pNode)[Count], pStart, i-pStart, pHere);
   COL_VAR2(pNode->size(), pNode->grammarSize());
}