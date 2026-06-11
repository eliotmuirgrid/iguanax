#ifndef __EDI_SAVE_H__
#define __EDI_SAVE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Routine to translate EDIgrammar into JSON which is carefully formatted for
// human readability.
//
// Author:   Eliot Muir
// Date:     26th July 2022
//---------------------------------------------------------------------------

class COLstring;
class EDIcollection;

bool EDIsave(const EDIcollection& Grammar, COLstring* pOutput, COLstring* pError);

#endif // end of defensive include
