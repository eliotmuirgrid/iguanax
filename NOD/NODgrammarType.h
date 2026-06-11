#ifndef __NOD_GRAMMAR_TYPE_H__
#define __NOD_GRAMMAR_TYPE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODgrammarType
//
// Description:
//
// NODgrammarType class.
//
// Author: Eliot Muir
// Date:   Wednesday, May 12th, 2010 @ 02:06:16 PM
//
//---------------------------------------------------------------------------

class COLstring;

enum NODgrammarType
{
   NOD_SIMPLE,
   NOD_VECTOR,
   NOD_COMPLEX
};

COLstring NODgrammarAsString(NODgrammarType GrammarType);

#endif // end of defensive include
