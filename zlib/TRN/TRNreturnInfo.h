#ifndef __TRN_RETURN_INFO_H__
#define __TRN_RETURN_INFO_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNreturnInfo
//
// Description:
//
// TRNreturnInfo holds the starting and ending positions of the lua stack for 
// function return values
//
// Author: Eliot Muir
// Date:   Wednesday, November 24th, 2010 @ 11:24:53 AM
//---------------------------------------------------------------------------

// This is really just a container for two ints. 
struct TRNreturnInfo{
   TRNreturnInfo() : StartParamIndex(1), EndParamIndex(0) {}

   void clear() { StartParamIndex=1; EndParamIndex=0; }

   int StartParamIndex;
   int EndParamIndex;

   bool noReturns() const { return (EndParamIndex < StartParamIndex); }
};


#endif // end of defensive include