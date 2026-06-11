#ifndef __TRN_INTELLISENSE_ALTER_CODE_H__
#define __TRN_INTELLISENSE_ALTER_CODE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisenseAlterCode
//
// Description:
//
// This code adds the lines:
//   _IntellisenseObject = T
//   _IntellisenseObject = T
// to enable intellisense.
//
// Author: Eliot Muir
// Date:   Monday, December 20th, 2010 @ 02:27:26 PM
//---------------------------------------------------------------------------

class COLstring;

void TRNintellisenseAlterCode(int LineBegin, const COLstring& LuaExpression, char IndexChar, COLstring* pCode);

#endif // end of defensive include
