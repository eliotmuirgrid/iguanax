//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNaddSpecificNodeTypeOptions
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, December 20th, 2010 @ 02:09:31 PM
//
//---------------------------------------------------------------------------
#include "TRNaddSpecificNodeTypeOptions.h"
#include "TRNintellisenseOptions.h"
#include "TRNconstant.h"
#include <THTM/THTMprinter.h>

#include <NOD/NODplace.h>
#include <NODB/NODBconstant.h>
#include <NOD/NODprintCallback.h>
#include <NOX/NOXconstant.h>
#include <NTAB/NTABconstant.h>
#include <LUAL/LUALutils.h>
#include <COL/COLstring.h>
#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// We could do this for every node type (ie, not just XML), but XML is the only one
// that will have multiple children with the same name.
static void TRNaddXmlOptions(const NODplace& Place, const COLstring& RootVar, TRNsortedOptionsList& Options){
   COL_FUNCTION(TRNaddXmlOptions);
   COLhashmap<COLstring, int> RepeatsByName;
   for (int i = 0, n = Place.size(); i < n; i++){
      const NODplace& Child = Place.child(i);
      if (Child.isNamed()){
         const COLstring& Name = Place.child(i).name();
         COLhashmapPlace NamePlace = RepeatsByName.find(Name);
         if (NamePlace){
            RepeatsByName[NamePlace]++;
         } else {
            RepeatsByName.add(Name, 1);
         }
      }
   }
   for (COLhashmapPlace NamePlace = RepeatsByName.first(); NamePlace; NamePlace = RepeatsByName.next(NamePlace)){
      if (RepeatsByName[NamePlace] > 1){
         TRNintellisenseOption Option(LUAL_LUA_PROTOCOL, LUA_TNUMBER, 0, TRNintellisenseOption::MAX);
         COLstring ChildCountCall = "childCount(\"" + RepeatsByName.key(NamePlace) + "\")";
         Option.FullText = RootVar + ":" + ChildCountCall;
         COLostream DisplayTextStream(Option.DisplayText);
         DisplayTextStream << ChildCountCall << ' ' << TRN_BEGIN_TREE_VALUE << RepeatsByName[NamePlace] << TRN_END_TREE_VALUE;
         Options.add(Option);
      }
   }
}

void TRNaddSpecificNodeTypeOptions(const NODplace& Place, const COLstring& RootVar, TRNsortedOptionsList& Options){
   if ( (Place.protocolType() == NODB_DB && Place.nodeType() == NODB_RESULT_SET) ||
      //   (Place.protocolType() == NOTG_TABLE_GRAMMAR && Place.isVector()) ||
        (Place.protocolType() == NTAB_TABLE_PROTOCOL && Place.isVector()) ){
      TRNintellisenseOption Option(LUAL_LUA_PROTOCOL, LUA_TNUMBER, 0, TRNintellisenseOption::MAX);
      COLostream DisplayTextStream(Option.DisplayText);
      DisplayTextStream << COL_T("Number of Rows ") << TRN_BEGIN_TREE_VALUE;
      THTMbeginSpan(DisplayTextStream, NOD_SPAN_NUMBER);
      DisplayTextStream << Place.size();
      THTMendSpan(DisplayTextStream);
      DisplayTextStream << TRN_END_TREE_VALUE;
      Option.FullText = '#' + RootVar;
      Options.add(Option);
   } else if (Place.protocolType() == NOX_XML && Place.nodeType() == NOX_ELEMENT){
      TRNaddXmlOptions(Place, RootVar, Options);
   }
}


