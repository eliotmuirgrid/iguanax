#ifndef __TRN_AUTO_COMPLETE_INFO_H__
#define __TRN_AUTO_COMPLETE_INFO_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2026 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNautoCompleteInfo
//
// Description:
//
// A new data structure to contain autocomplete results 
//
// Author: Eliot Muir 
// Date:   Friday 06 March 2026 - 03:07PM
// ---------------------------------------------------------------------------

#include <COL/COLlist.h>
#include <COL/COLvar.h>

class TRNoption{
public:
   COLstring Type;
   COLstring TextExpression;
   COLvar    Value;   // For now let's do this.
};

class TRNautoCompleteInfo{
public:
   COLlist<TRNoption> Options;
};



#endif // end of defensive include
