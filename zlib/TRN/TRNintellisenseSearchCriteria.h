#ifndef __TRN_INTELLISENSE_SEARCH_CRITERIA_H__
#define __TRN_INTELLISENSE_SEARCH_CRITERIA_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisenseSearchCriteria
//
// Description:
//
// Helper used in intellisense
//
// Author: Eliot Muir
// Date:   Tuesday, December 21st, 2010 @ 06:58:44 AM
//---------------------------------------------------------------------------

class REXmatcher;

void TRNintellisenseSearchCriteria(COLstring SearchString, REXmatcher& SearchRegex, int& SearchIndex);

#endif // end of defensive include
