//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisenseOptions
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, December 20th, 2010 @ 02:09:31 PM
//---------------------------------------------------------------------------
#include "TRNintellisenseOptions.h"
#include "TRNtraverser.h"
#include "TRNconstant.h"
#include <THTM/THTMprinter.h>

#include <DEP/DEPlist.h>
#include <NOD/NODplace.h>
#include <LUAL/LUALutils.h>
#include <NOX/NOXconstant.h>

#include <NODB/NODBconstant.h> // TODO - shouldn't really need this information here.

#include <TXT/TXTutils.h>
#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <ctype.h>

#define TRN_MAX_COUNT_OF_OPTION 100


TRNintellisenseOption::TRNintellisenseOption(int iProtocolType, int iNodeType, int iLevel, ePriority iPriority)
: ProtocolType(iProtocolType), NodeType(iNodeType), IsSimple(true), Level(iLevel), Index(0), Priority(iPriority)
{
   COL_METHOD(TRNintellisenseOption::TRNintellisenseOption);
}

TRNintellisenseOption::TRNintellisenseOption(const TRNtraverser& Traverser, int iLevel, ePriority iPriority)
: Level(iLevel), Index(0), Priority(iPriority)
{
   COL_METHOD(TRNintellisenseOption::TRNintellisenseOption);
   Traverser.getTypeInfo(ProtocolType, NodeType);
   Traverser.getLuaKey(LuaKey);
   IsSimple = Traverser.isLeaf();
}

// Was in TXTnaturalCompareStrings - moved to eliminate TXT dependendency.
int TRNnaturalCompareStrings(const COLstring& Left, const COLstring& Right) {
   COL_FUNCTION(TRNnaturalCompareStrings);
   const char* pLeft  = (const char*)Left.c_str();
   const char* pRight = (const char*)Right.c_str();

   COLPRECONDITION( pLeft );
   COLPRECONDITION( pRight );

   {
      int CurrentLeft;
      int CurrentRight;

      while( (CurrentLeft = (unsigned char)*pLeft) && (CurrentRight = (unsigned char)*pRight) ) {
         if( isdigit(CurrentLeft) && isdigit(CurrentRight) )  {
            long  LeftNumber = strtol(pLeft,  (char**)&pLeft,  10);
            long RightNumber = strtol(pRight, (char**)&pRight, 10);

            if( LeftNumber != RightNumber ) {
               return LeftNumber - RightNumber;
            }
         } else {
            CurrentLeft  = tolower(CurrentLeft);
            CurrentRight = tolower(CurrentRight);

            if( CurrentLeft != CurrentRight ) {
               return CurrentLeft - CurrentRight;
            }

            ++pLeft;
            ++pRight;
         }
      }
   }

   return (unsigned char)*pLeft - (unsigned char)*pRight;
}


int TRNintellisenseOptionCompareClass::compare(const TRNintellisenseOption& Left, const TRNintellisenseOption& Right){
   if (Left.Priority != Right.Priority){
      return (Left.Priority > Right.Priority ? -1 : 1);
   } else if (Left.Level != Right.Level){
      return (Left.Level < Right.Level ? -1 : 1);
   } else if (!Left.LuaKey.is_null() || !Right.LuaKey.is_null()){
      // Sort by key (keys beginning with '_' have the lowest priority).
      bool LeftUnderscore = (!Left.LuaKey.is_null() && Left.LuaKey[0] == '_');
      bool RightUnderscore = (!Right.LuaKey.is_null() && Right.LuaKey[0] == '_');
      if (LeftUnderscore != RightUnderscore){
         return LeftUnderscore ? 1 : -1;
      } else {
         int NaturalCompareResult = TRNnaturalCompareStrings(Left.LuaKey, Right.LuaKey);
         // If strings are "naturally" equal, use standard string comparison.
         if (NaturalCompareResult){
            return NaturalCompareResult;
         } else {
            // Fall back to regular string comparison, and compare
            // the "full text" rather than just the last key (#21517).
            // The Options' FullText properties should never be the
            // same - and if they are, they are literally identical
            // so we're ok returning 0 here in that case.
            return Left.FullText.compare(Right.FullText);
         }
      }
   } else if (Left.Index != Right.Index){
      return (Left.Index < Right.Index ? -1 : 1);
   } else{
      return 0; // no two indexes should be the same, so we should never really get here.
   }
}

static bool TRNoptionIsDeprecatedFunction(const TRNintellisenseOption& Option){
   COL_FUNCTION(TRNoptionIsDeprecatedFunction);
   COLarray<COLstring> DeprecatedFunctions;
   DEPreadLua(&DeprecatedFunctions);
   for (auto it : DeprecatedFunctions){
      // The cache returns deprecated.txt with an extra empty line. The it.is_null check skips that one.
      if (!it.is_null() && it.equals(Option.FunctionName)) { 
         COL_DBG(it);
         COL_DBG(Option.FunctionName);
         return true;
      }  
   }  
   return false;
}

void TRNsortedOptionsList::add(TRNintellisenseOption& Option){
   bool AllowDeprecated = false;
   if (getenv("IFW_ALLOW_DEPRECATED")){
      COL_TRC("Warning IFW_ALLOW_DEPRECATED defined so we allow deprecated translator functions!");
      AllowDeprecated = true;
   }
   if (!AllowDeprecated && TRNoptionIsDeprecatedFunction(Option)){
      COL_TRC("Deprecated function " + Option.FunctionName + " -- skip");
      return;
   }
   Option.Index = m_NextIndex++;
   if (m_SortedOptions.size() == TRN_MAX_COUNT_OF_OPTION){
      COLavlTreePlace LastPlace = m_SortedOptions.last();
      const TRNintellisenseOption& LastOption = m_SortedOptions.key(LastPlace);
      if (TRNintellisenseOptionCompareClass::compare(Option, LastOption) < 0){
         m_OptionsTruncated = true;
         m_SortedOptions.remove(LastPlace);
         m_SortedOptions.addUnique(Option, NULL);
      } // else, avoid adding the option to the list at all.
   } else {
      m_SortedOptions.addUnique(Option, NULL);
   }
}
