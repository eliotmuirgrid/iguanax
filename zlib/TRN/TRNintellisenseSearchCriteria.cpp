//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisenseSearchCriteria
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, December 21st, 2010 @ 06:58:44 AM
//---------------------------------------------------------------------------
#include <REX/REXmatcher.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void TRNintellisenseSearchCriteria(COLstring SearchString, REXmatcher& SearchRegex, int& SearchIndex){
   COL_FUNCTION(TRNintellisenseSearchCriteria);
	// Get the index represented by the search string, if the search string is a number.
   REXmatcher IndexMatcher("^\\d+$");
   COLPRECONDITION(IndexMatcher.isExpressionValid());
   SearchIndex = -1;
   if (IndexMatcher.doesMatch(SearchString.c_str())){
      SearchIndex = atoi(SearchString.c_str()) - 1;
   }
   // Create regex for search string - we must escape regex meta-characters
   REXmatcher MetaCharacterRegex("([][(){}\\\\^.$|?*+-])");
   COLPRECONDITION(MetaCharacterRegex.isExpressionValid());
   MetaCharacterRegex.regexReplace(SearchString, "\\$1");  // Escape all regex meta-characters.
   SearchString = '(' + SearchString + ')';
   SearchRegex.init(SearchString, REXmatcher::CaseInsensitive);
   COLPRECONDITION(SearchRegex.isExpressionValid());
}
