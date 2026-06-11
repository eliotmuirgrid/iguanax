#ifndef __TRN_VARIABLE_AUTO_COMPLETION_H__
#define __TRN_VARIABLE_AUTO_COMPLETION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNshouldDoVariableAutocompletion
//
// Description:
//
// Helper function used in intellisense logic.
//
// Author: Eliot Muir
// Date:   Monday, December 20th, 2010 @ 02:27:26 PM
//---------------------------------------------------------------------------

class LUAXautocompletionContext;

bool TRNshouldDoVariableAutocompletion(const LUAXautocompletionContext& Context);

#endif // end of defensive include
