#ifndef __EDI_MAGIC_STRING_H__
#define __EDI_MAGIC_STRING_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDImagicString
//
// Description:
//
// Functions to convert <segment group>_$IFW$_<message name> nodes in a collection
// to chopped versions <segment group>_<index>.
//
// Author: Vismay Shah 
// Date:   Friday 05 September 2025 - 02:58PM
// ---------------------------------------------------------------------------
#include <COL/COLmap.h>
class EDIcollection;

void EDIcalculateMagicStringReplacements(EDIcollection* pOutput, COLmap<COLstring, COLstring>* pReplacements);
void EDIapplyMagicStringReplacements(EDIcollection* pOutput, const COLmap<COLstring, COLstring>& Replacements);

#endif // end of defensive include
